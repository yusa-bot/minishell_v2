# v1
.
├── builtins
│   ├── cd.c
│   ├── echo.c
│   ├── env.c
│   ├── exit.c
│   ├── export.c
│   ├── pwd.c
│   └── unset.c
├── env
│   ├── env_init.c
│   └── env_utils.c
├── executor
│   ├── exec_ast.c
│   ├── exec_cmd.c
│   ├── heredoc.c
│   ├── pipe.c
│   └── redirect.c
├── expander
│   ├── expand.c
│   └── wildcard.c
├── lexer
│   └── lexer.c
├── main.c
├── parser
│   ├── parser.c
│   └── syntax_err.c
└── signals
    └── signal.c



.
├── builtins
│   ├── cd.c
│   ├── echo.c
│   ├── env.c
│   ├── exit.c
│   ├── export.c
│   ├── pwd.c
│   └── unset.c
├── env
│   ├── env_init.c
            // リストの基本操作
            // **envp -> t_envの連結リスト
            t_env *init_env(char **envp);
            // malloc を使 用して新しいノードを作成
            t_env *env_new(char *key, char *value);
            // リストの末尾にノードを追加
            void env_add_back(t_env **env_list, t_env *new_node);
            // シェル終了時にリスト全体のメモリ（key, value, ノード自体）を解放
            void free_env_list(t_env *env_list);
│   └── env_utils.c
            // 実行時の配列変換（Executor）
            // リスト構造の環境変数を、再び char ** の配列（KEY=VALUE の形式）に逆変換
            char **env_list_to_array(t_env *env_list);

            // 変数の検索と更新（Expander / Builtins 用）
            // 指定されたキーを持つ変数を探し、その value のポインタを返す
            char *get_env_value(t_env *env_list, char *key)
            // 指定されたキーが存在すれば value を上書きし、存在しなければ新規ノードとして追加
            void set_env_value(t_env **env_list, char *key, char *value)
            // リストの末尾に新しいノードを追加する
            void	env_add_back(t_env **env_list, t_env *new_node)
            // 指定されたキーを持つノードをリストから切り離して free する
            void remove_env_node(t_env **env_list, char *key)
├── executor
│   ├── exec_ast.c (処理分岐) -> pipe.c, exec_cmd.c
            // AST -> &&, ||, () に振り分け
            // AST評価 -> type に応じて処理を分岐 -> $?を返す
            int exec_ast(t_node *node, t_env **env_list);
            // NODE_AND (&&)を処理.
            //		左ノードを実行し、その終了ステータスが0であった場合のみ、右ノードを実行
            int exec_and(t_node *node, t_env **env_list);
            // NODE_OR (||) を処理.
            //		左ノードを実行し、その終了ステータスが0以外であった場合のみ、右ノードを実行
            int exec_or(t_node *node, t_env **env_list);
            // NODE_SUBSHELL (()) を処理.
            //		forkで子プロ生成 -> 子プロでexec_ast() -> 親プロが子プロのステータスを返す
            int exec_subshell(t_node *node, t_env **env_list);
│   ├── exec_cmd.c -> 外部コマンドのパス展開で env_utils.c を使用
            // コマンド実行
            // 展開 -> リダイレクト適用 -> 実行分岐
            int exec_cmd(t_node *node, t_env **env_list);

            // ビルトインコマンド
            // ビルトインコマンドか判定
            int is_builtin(char *cmd);
            // ビルトインコマンドの実装関数を呼び出す
            int exec_builtin(char **args, t_env **env_list);

            // 外部コマンド
            // 子プロでexecve()
            int exec_external(char **args, t_env **env_list);
            // 実行可能ファイルのフルパスを取得
            char *get_cmd_path(char *cmd, t_env *env_list);
│   ├── expand.c
            // 展開系util
            // str: args[i], redir->filename を expand_stringに渡す
            void	expand_node(t_node *node, t_env *env_list);
            // 	str: args[i] か redir->filename の $, クォート を適切に展開する
            char *expand_string(char *str, t_env *env_list);
            //　$と、それに続く文字(2個目の$まで) を返す
            //		* 環境変数は連続出力にも対応する, valueが無い時は空文字扱い
            static char	*handle_dollar(char *res, char *str, int *i, t_env *env_list);
            // 文字列の末尾に1文字を追加し、元の文字列を解放
            static char	*append_char(char *str, char c);
│   ├── heredoc.c
            // AST全体を走査し、TK_HEREDOC があれば入力を処理する（再帰）
            int	process_heredoc(t_node *node, t_env *env_list);
            // 実際に readline を回して一時ファイルに書き込む。ここでは、一時ファイルに書き込むまで
            static int	read_heredoc(t_redirect *redir, t_env *env_list);
│   ├── pipe.c
│   └── redirect.c
├── expander
│   ├── expand.c
            // 展開系util
            // str: args[i], redir->filename を expand_stringに渡す
            void	expand_node(t_node *node, t_env *env_list);
            // 	str: args[i] か redir->filename の $, クォート を適切に展開する
            char *expand_string(char *str, t_env *env_list);
            //　$と、それに続く文字(2個目の$まで) を返す
            //		* 環境変数は連続出力にも対応する, valueが無い時は空文字扱い
            static char	*handle_dollar(char *res, char *str, int *i, t_env *env_list);
            // 文字列の末尾に1文字を追加し、元の文字列を解放
            static char	*append_char(char *str, char c);
│   └── wildcard.c
            // wildcard展開
            // dir内の全ファイル取得 -> 一致判定 -> sort -> sorted_matches配列を返す
            char	**expand_wildcard(char *pattern); // pattern:*.c
            // パターンとファイル名が一致するか判定
            // 		*があったら、patternとstrが不一致の間、match(*に対応するstr部分)に記録し、
            //		patternとstrが一致し、双方が\0で終わったら、一致していると言える。
            int	match_pattern(char *pattern, char *str); // pattern:*.c str:d_name
            // 見つかったファイル名を配列に追加して拡張
            static char	**append_match(char **matches, char *new_str, int *count);
            // 文字列配列を辞書順(ASCII順)にバブルソートする
            static void	sort_str_array(char **array, int count);
├── lexer
│   └── lexer.c
            // tokenize
            // 1prompt
            t_token *tokenize(char *line);
            // |, <, >, &, (, ) -> &&, ||, <<, >>
            t_token *consume_operator(char **line_ptr);
            // 演算子でも空白でもない連続した文字列 -> TK_WORD
            t_token *consume_word(char **line_ptr);
├── main.c
├── parser
│   ├── parser.c
            // parse
            // 構文解析の入り口
            t_node *parse(t_token **tokens);
            // 最も優先順位の低い && と || を処理
            t_node *parse_list(t_token **tokens);
            // 次に優先順位の低い |（パイプ）を処理
            t_node *parse_pipeline(t_token **tokens);
            // 最も優先順位が高い「単一のコマンド」または「サブシェル ()」を処理
            t_node *parse_command(t_token **tokens);
└── signals
    └── signal.c


[AST解析 -> t_nodeを展開 -> コマンド実行]
            ---------- ここの流れ

exec_cmd()
->  expand_node():展開開始
	 │
	expand_string() : $, クォートを展開 (wildcard必要有無flagを取得)
	 ├── !has_wildcard: -> expand_node()で、node->argsを展開後の文字列で上書き
	 └── has_wildcard
				└── expand_wildcard() -> sorted_matchesリストを返す -> expand_node()で、node->argsを展開後の文字列で上書き
