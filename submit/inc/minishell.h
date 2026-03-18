#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/resource.h>
# include <sys/stat.h>
# include <dirent.h>
# include <signal.h>
# include <errno.h>
# include <limits.h>
# include <string.h>
# include <termios.h>
# include <sys/ioctl.h>

// Lexer ------------------------------------------------
typedef enum e_token_type {
    TK_WORD,        // コマンド名、引数
    TK_PIPE,        // |
    TK_AND,         // &&
    TK_OR,          // ||
    TK_LPAREN,      // (
    TK_RPAREN,      // )
    TK_REDIR_IN,    // <
    TK_REDIR_OUT,   // >
    TK_REDIR_APPEND,// >>
    TK_HEREDOC,     // <<
    TK_REDIR_RDWR   // <> (read-write, 構文チェック用)
} t_token_type;

typedef struct s_token {
    t_token_type    type;
    char            *value;
    struct s_token  *next;
} t_token;

// Parser ------------------------------------------------
typedef enum e_node_type {
    NODE_CMD,       // 単一コマンド
    NODE_PIPE,      // パイプライン
    NODE_AND,       // 論理積 (&&)
    NODE_OR,        // 論理和 (||)
    NODE_SUBSHELL   // 括弧によるサブシェル ()
} t_node_type;

typedef struct s_redirect {
    t_token_type        type;       // リダイレクトの種類 (TK_REDIR_OUT, TK_HEREDOC など)
    char                *filename;  // 対象のファイル名、または区切り文字(EOF)
    int                 quoted;     // heredoc区切り文字がクォートされていた場合1 (変数展開なし)
    struct s_redirect   *next;      // 次のリダイレクトへのポインタ
} t_redirect;

typedef struct s_node {
    t_node_type     type;
    struct s_node   *left;
    struct s_node   *right;

    // NODE_CMD用のデータ
    char            **args;       // コマンドと引数の配列
    t_redirect      *redirects;   // リダイレクトのリスト
} t_node;

// env ------------------------------------------------
typedef struct s_env
{
    char            *key;   // 変数名（例: "PATH"）
    char            *value; // 値（例: "/usr/bin:/bin"）
    struct s_env    *next;  // 次の要素へのポインタ
}   t_env;

extern volatile sig_atomic_t g_sig;

// builtin -----------------------------------------------
int	builtin_cd(char **args, t_env **env_list);
int	builtin_echo(char **args);
int	builtin_env(t_env *env_list);
int	builtin_exit(char **args, t_env **env_list, t_node *root);
int	builtin_export(char **args, t_env **env_list);
int	builtin_pwd(void);
int	builtin_unset(char **args, t_env **env_list);

// env -----------------------------------------------

// env_init.c
// **envp -> t_envの連結リスト
t_env *env_init(char **envp);
// malloc を使用して新しいノードを作成
t_env *env_new(char *key, char *value);
// リストの末尾にノードを追加
void env_add_back(t_env **env_list, t_env *new_node);
// シェル終了時にリスト全体のメモリ（key, value, ノード自体）を解放
void free_env(t_env *env_list);

// env_util.c
// リスト構造の環境変数を、再び char ** の配列（KEY=VALUE の形式）に逆変換
char **env_list_to_array(t_env *env_list);
// t_envのノード数をカウントする
int	count_env_nodes(t_env *env_list);
// 指定されたキーを持つ変数を探し、その value のポインタを返す
char *get_env_value(t_env *env_list, char *key);
// 指定されたキーが存在すれば value を上書きし、存在しなければ新規ノードとして追加
void set_env_value(t_env **env_list, char *key, char *value);
// 指定されたキーを持つノードをリストから切り離して free する
void remove_env_node(t_env **env_list, char *key);

// executor -----------------------------------------------

// exec_ast.c
// AST評価 -> type に応じて処理を分岐 (分解より下層は分岐先で再帰) -> $?を返す
int exec_ast(t_node *node, t_env **env_list, t_node *root);

// exec_cmd.c
// 展開 -> リダイレクト適用 -> 実行分岐
int exec_cmd(t_node *node, t_env **env_list, t_node *root);

// heredoc.c
// AST全体を走査し、TK_HEREDOC があれば入力を処理する（再帰）
int	process_heredoc(t_node *node, t_env *env_list);

// pipe.c
// pipe(2つのfdを繋ぐ) -> 左(書), 右(読)の子プロを作成 -> 実行関数へ -> 親プロで子プロを待つ
int	exec_pipeline(t_node *node, t_env **env_list, t_node *root);

// redirect.c
// t_redirect->nextがある間リストを回し、リダイレクトの種類によって処理を振り分け
// 		複数t_redirectでも、最後のFDで上書き
int apply_redirects(t_redirect *redirects);
// バックアップしておいた標準入出力のFDを dup2 で元に戻す
void restore_fds(int saved_stdin, int saved_stdout);

//  expander -----------------------------------------------

// expand.c
// str: args[i], redir->filename を expand_stringに渡す
void	expand_node(t_node *node, t_env *env_list);
// 文字列の末尾に1文字を追加し、元の文字列を解放
char	*append_char(char *str, char c);
// heredocの各行を変数展開する (lineの所有権を受け取り、展開後の文字列を返す)
char	*expand_heredoc_line(char *line, t_env *env_list);

// expand_args.c
// argsの各要素を展開
void	expand_args(t_node *node, t_env *env_list);

// expand_redirects.c
// リダイレクトのファイル名を展開
void	expand_redirects(t_node *node, t_env *env_list);
// 引用符なしの$があるかチェック
int		has_unquoted_dollar(char *str);

// expand_dollers_quotes.c
// $, クォート を適切に展開する
char	*expand_string(char *str, t_env *env_list, int *has_wildcard);

// wildcard.c
// dir内の全ファイル取得 -> 一致判定 -> sort -> sorted_matches配列を返す
char	**expand_wildcard(char *pattern);
// 文字列配列を辞書順(ASCII順)にバブルソートする
void	sort_str_array(char **array, int count);

// wildcard_match.c
// パターンとファイル名が一致するか判定
int		match_pattern(char *pattern, char *str);

//  lexer -----------------------------------------------

// lexer.c
// 1 prompt
t_token *tokenize(char *line);
// トークンの連結リストを全て解放する
void	free_tokens(t_token *tokens);

// lexer_helpers.c
// スペースやタブの間ポインタを進める
void skip_spaces(char **line_ptr);
// malloc を使用して新しいトークンノードのメモリを確保し、種類と文字列をセット
t_token *new_token(t_token_type type, char *value);
// 作成したトークンを、トークンリストの末尾に追加します。
void token_add_back(t_token **head, t_token *new_node);
// 対象文字が演算子か
int     is_operator(char c);
// 文字が区切り文字（空白または演算子）かどうか
int	is_spaces_operator(char c);

//  parser -----------------------------------------------

// parser.c
// 構文解析の入り口
t_node *parse(t_token **tokens);

// parse_command.c
t_node *parse_command(t_token **tokens);

// parser_helpers.c
// ASTの新しいノードを作成し、初期化
t_node *new_node(t_node_type type, t_node *left, t_node *right);
// tokenがリダイレクト演算子かどうかを判定
int  is_redirect(t_token_type type);
//　char **args をメモリ再確保して追加
void append_arg(t_node *node, char *value);

// free_ast.c
// AST 全体のメモリを再帰的に解放する
void	free_ast(t_node *node);
// 文字列配列 (char **) の全要素と枠組みを解放する
void	free_str_array(char **array);

//  signal ----------------------------------------------

// signal.c

// A: プロンプトで入力を待っている
// SIGINT
void handler_interactive(int signum);
void set_signal_interactive(void);

// B: コマンド実行中 (子プロセス)
// 親プロ
void set_signal_executing(void);
// 子プロ
void set_signal_child(void);

// C: heredoc
void set_signal_heredoc(void);

// status.c ----------------------------------------------
// waitpid のステータスマクロ(WIFEXITED等)を使って正確な終了コードを計算して返す
int	calculate_exit_status(int status);
// calculate_exit_status + Quit print
int calculate_exit_status_quit(int status);
// 環境変数の $? を更新
void	update_exit_status(t_env **env_list, int status);
// AST・env・historyを解放してexit (子プロセス・builtin exit用)
void	cleanup_and_exit(int status, t_node *ast, t_env *env);

#endif
