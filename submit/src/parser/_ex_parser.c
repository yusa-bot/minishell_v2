parse_command時点でのparse例 (葉のため、t_node_typeはNODE_CMD)

1. ls -l -a
typedef struct s_node {
    t_node_type     NODE_CMD
    struct s_node   *left;
    struct s_node   *right;

    // NODE_CMD用のデータ
    char            args[0] = "ls"
					args[1] = "-l"
					args[2] = "-a"
					args[3] = NULL
    t_redirect      NULL
} t_node;

2. echo "hello world" > out.txt
typedef struct s_redirect {
    t_token_type        TK_REDIR_OUT
    char                "out.txt"
    struct s_redirect   NULL
} t_redirect;
typedef struct s_node {
    t_node_type     NODE_CMD
    struct s_node   *left;
    struct s_node   *right;

    // NODE_CMD用のデータ
    char            args[0] = "echo"
					args[1] = "hello world"
					args[2] = NULL
    t_redirect      *redirects;
} t_node;

3. < infile grep "error" >> append.log
typedef struct s_redirect {
    t_token_type        TK_REDIR_IN
    char                "infile"
    struct s_redirect   t_redirect_2
} t_redirect_1;
typedef struct s_redirect {
    t_token_type        TK_REDIR_APPEND
    char                "append.log"
    struct s_redirect   NULL
} t_redirect_2;
typedef struct s_node {
    t_node_type     NODE_CMD
    struct s_node   *left;
    struct s_node   *right;

    // NODE_CMD用のデータ
    char            args[0] = "grep"
					args[1] = "error"
					args[2] = NULL
    t_redirect      *t_redirect_1;  
} t_node;
