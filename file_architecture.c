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
            // malloc を使用して新しいノードを作成
            t_env *env_new(char *key, char *value);
            // リストの末尾にノードを追加
            void env_add_back(t_env **env_list, t_env *new_node);
            // シェル終了時にリスト全体のメモリ（key, value, ノード自体）を解放
            void free_env_list(t_env *env_list);
│   └── env_utils.c
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
│   ├── exec_cmd.c
│   ├── heredoc.c
            // AST全体を走査し、TK_HEREDOC があれば入力を処理する（再帰）
            int	process_heredoc(t_node *node, t_env *env_list);
            // 実際に readline を回して一時ファイルに書き込む。ここでは、一時ファイルに書き込むまで
            static int	read_heredoc(t_redirect *redir, t_env *env_list);
│   ├── pipe.c
│   └── redirect.c
├── expander
│   ├── expand.c
│   └── wildcard.c
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
│   └── syntax_err.c
└── signals
    └── signal.c
