#ifndef MINISHELL_H
# define MINISHELL_H

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
    TK_HEREDOC      // <<
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


#endif
