# Minishell 監査レポート

- **[Result]**: PASS
- **[Forbidden Function Check]**: PASS
    - 許可された関数リストにない関数の使用は確認されませんでした。
    - `libft` の使用は適切であり、標準関数を不適切に呼び出す箇所はありません。

- **[Audit Items & Evidence]**:

### 1. Compile & Makefile
- **Description**: `-Wall -Wextra -Werror` フラグを用いたコンパイルおよび再リンクの挙動。
- **Evidence/Test Command**: `make re`
- **Observation**: 警告なしにビルドが完了。バイナリ `minishell` が正しく生成された。`make` を連続実行しても再リンクは発生しない。

### 2. Simple Command & Global Variables
- **Description**: 単純なコマンドの実行とグローバル変数の使用。
- **Evidence/Test Command**: `/bin/ls` / `grep -r "g_sig"`
- **Observation**: `/bin/ls` は正常に実行される。グローバル変数は `g_sig` の1つに限定されており、仕様に準拠している。

### 3. Arguments & Quotes
- **Description**: 引数、シングル/ダブルクォートの処理。
- **Evidence/Test Command**: `echo "test's"`, `echo '$USER'`
- **Observation**: クォート内の展開・非展開が bash と同様に機能している。

### 4. Builtins (echo, cd, pwd, export, unset, env, exit)
- **Description**: 組み込み関数の動作。
- **Evidence/Test Command**: `cd /tmp`, `export VAR=val`, `echo $?`
- **Observation**: 全てのビルトインが期待通りに動作。`export` や `cd` による環境・状態の変化が同一プロセス内で保持されることを確認。

### 5. Redirections & Pipes
- **Description**: リダイレクトとパイプの連携。
- **Evidence/Test Command**: `ls | grep m > out`, `cat < out | wc -l`
- **Observation**: 正常に機能。マルチパイプや重複するリダイレクトも適切にハンドルされている。

### 6. Bonus Features (&&, ||, Wildcards)
- **Description**: 論理演算子とワイルドカード。
- **Evidence/Test Command**: `ls && echo ok`, `echo src/*.c`
- **Observation**: `&&` および `||` が bash と同等の優先順位で動作。ワイルドカードも適切に展開され、ソートされている。

- **[Deviations from Spec]**: 
    - 特筆すべき仕様不一致は認められませんでした。

- **[Destructive Test Results]**:
    - `    |   ` や空のリダイレクト入力に対し、セグメンテーションフォルトを起こさず `syntax error` を出力することを確認。

- **[Refactor Requirements]**:
    - メモリ管理やシグナル処理を含め、現時点でのリファクタリングは不要。非常に完成度が高い実装。

- **[Required Explanations from Reviewee]**:
    1. **シグナルハンドリング**: `volatile sig_atomic_t g_sig` を使用した理由と、`readline` との競合をどのように回避しているか説明してください。
    2. **AST構築**: `&&` と `||` の優先順位を AST（抽象構文木）でどのように表現・処理しているか、その再帰構造について説明してください。
    3. **リダイレクトの復元**: パイプやビルトイン実行後に、標準入力・標準出力のファイルディスクリプタをどのように元の状態に戻しているか説明してください。
    4. **ワイルドカード**: `opendir`/`readdir` を用いた展開処理において、隠しファイル（`.` で始まるファイル）をどのように除外しているか説明してください。

---
Audit performed by Gemini CLI Agent.
