# Minishell 監査レポート

**[Result]**: 不合格 (FAIL)

**[Forbidden Function Check]**: 合格 (PASS)
`rl_catch_signals` へのアクセスが確認されたが、過去の経緯により許容。その他の外部関数はすべて許可リスト内である。

**[Audit Items & Evidence]**:

### コンパイル
- **説明**: `-Wall -Wextra -Werror` フラグを用いてコンパイルすること。不要な再リンクが発生しないことを確認すること。
- **エビデンス/テストコマンド**: `cd minishell && make clean && make && make`
- **観察結果**: 合格。不要な再リンクは発生しない。

### メモリリーク (Valgrind)
- **説明**: `valgrind` を用いて、ユーザー実装部分にメモリリークがないか確認すること。
- **エビデンス/テストコマンド**: `valgrind --leak-check=full --show-leak-kinds=all ./minishell < test_commands.txt`
- **観察結果**: 合格。`definitely lost: 0 bytes`。環境変数の頻繁な `export`/`unset` 操作においてもリークは発生しなかった。

### 終了ステータスの伝播
- **説明**: 各コマンドの実行後、`$?` が正しく更新されること。
- **エビデンス/テストコマンド**: `echo -e "ls non_existent\necho \$?" | ./minishell`
- **観察結果**: 合格。失敗したコマンドのステータス `2` が正しく表示された。

### リダイレクト展開エラー時の実行制御 (致命的バグ)
- **説明**: アンビギュアス・リダイレクトなどの展開エラーが発生した際、後続のコマンド（特に `&&` や `||` で繋がれたもの）の実行が適切に停止されること。
- **エビデンス/テストコマンド**: `touch f1 f2 && echo "ls > * && echo success" | ./minishell && rm f1 f2`
- **観察結果**: **不合格**。`minishell: *: ambiguous redirect` というエラーメッセージは表示されるものの、実行が停止されずに `success` が出力された。これは `expand_redirects` の戻り値が `void` であり、呼び出し元にエラーが伝播していないことに起因する。
- 補足(fnakamur): minishell内で対話的に上のコマンドを実行するには以下の通り:
```
✦ そのワンライナーを ./minishell のプロンプトで対話的に実行する場合、以下の手順になります。

   1. 事前準備（通常のシェルで実行）:
   1    touch f1 f2

   2. minishellを起動:
   1    ./minishell

   3. minishellのプロンプトに対して入力:

   1    minishell$ ls > * && echo success

   4. 期待される挙動（Bash等）:
     * が f1 f2 に展開され、リダイレクト先が2ワードになるため ambiguous redirect エラーが発生し、&& 以降の echo success は実行されないのが正解です。

   5. 本プロジェクトでの実際の挙動（バグ）:
     minishell: *: ambiguous redirect と表示されるにもかかわらず、その直後に success と出力されてしまいます。

   6. 後片付け（通常のシェルで実行）:
   1    rm f1 f2

  つまり、対話的なコマンドとしては ls > * && echo success という1行の入力になります。
```

**[Deviations from Spec]**:
- **展開エラー後の不適切な実行継続 (致命的)**: `expand_redirects` で「ambiguous redirect」が発生した場合、エラーメッセージは表示されるが、戻り値が `void` であるため呼び出し元の `exec_cmd` にエラーが伝わらず、そのままコマンドが実行されてしまう。
    - **再現手順**: `touch f1 f2 && echo "ls > * && echo success" | ./minishell` を実行すると、`ambiguous redirect` と表示されるにもかかわらず `success` が出力される。
- **ヒアドキュメントにおけるクォートの不適切な削除**: `expand_heredoc_line` が汎用的な `expand_string` を使用しているため、ヒアドキュメントの内容に含まれる引用符（`"` や `'`）が削除されてしまう。Bash ではヒアドキュメント内の引用符はリテラルとして保持されるべきである。
    - **再現手順**: `cat << EOF` に対して `"hello"` と入力すると、本来 `"hello"` と出力されるべきところが `hello` と出力される。
- **ヒアドキュメント作成失敗時のエラーハンドリング不全**: `heredoc_child` が `/tmp` 下での一時ファイル作成に失敗した場合（例：権限不足）、子プロセスは `exit(1)` するが、`heredoc_parent` はステータスコードを無視し、中断（SIGINT）以外をすべて成功と見なしてしまう。
- **シグナルハンドラの非安全性**: `handler_interactive` 内で `rl_redisplay` 等の非シグナルセーフ関数を使用しており、競合状態によるクラッシュやデッドロックのリスクがある。

**[Destructive Test Results]**:
- **未閉鎖のクォート**: `minishell: unclosed quote error` と表示され、適切にエラー処理された。
- **ディレクトリへのリダイレクト**: `minishell: test_dir: Is a directory` と表示され、適切にエラー処理された。
- **長大なコマンド入力**: `readline` のバッファ制限まで正常に読み込み可能であることを確認。

**[Refactor Requirements]**:
- **展開フェーズの戻り値の導入**: `expand_node`, `expand_args`, `expand_redirects` を `int` 型に変更し、エラー（`ambiguous redirect` 等）が発生した場合は即座に実行を中断するロジックを `exec_ast` に実装すべき。
- **ヒアドキュメント展開ロジックの修正**: ヒアドキュメント用には引用符を削除しない専用の展開処理を実装すべき。
- **シグナル処理のメインループ移行**: ハンドラ内ではフラグを立てるのみとし、`readline` の `rl_event_hook` 等を用いて安全に再描画を行うべき。

**[Required Explanations from Reviewee]**:
1.  **リダイレクト展開失敗時の挙動**: なぜ `ambiguous redirect` が発生した際、コマンドの実行を停止するように設計しなかったのか。エラー伝播のための戻り値が欠落している点について見解を述べよ。
2.  **ヒアドキュメントでのクォート扱い**: ヒアドキュメントの内容に対して引用符の削除（Quote Removal）を適用した理由を説明せよ。
3.  **シグナルハンドラの安全性**: 非シグナルセーフ関数の使用に伴うリスク（リエントラント性の欠如など）をどのように評価しているか説明せよ。
4.  **環境変数の再利用性**: `export` の出力（`declare -x`）にエスケープを施している一方で、入力時にはバックスラッシュを無視する設計となっている。この非対称性が、設定の再読み込みに与える影響をどのように考えているか述べよ。
