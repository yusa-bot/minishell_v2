/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:26 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:43:20 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
typedef enum e_token_type
{
	TK_WORD,
	TK_PIPE,
	TK_AND,
	TK_OR,
	TK_LPAREN,
	TK_RPAREN,
	TK_REDIR_IN,
	TK_REDIR_OUT,
	TK_REDIR_APPEND,
	TK_HEREDOC,
	TK_REDIR_RDWR
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

// Parser ------------------------------------------------
typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_AND,
	NODE_OR,
	NODE_SUBSHELL
}	t_node_type;

typedef struct s_redirect
{
	t_token_type		type;
	char				*filename;
	int					quoted;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_node
{
	t_node_type		type;
	struct s_node	*left;
	struct s_node	*right;
	char			**args;
	t_redirect		*redirects;
}	t_node;

// env ------------------------------------------------
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

extern volatile sig_atomic_t	g_sig;

// builtin -----------------------------------------------

int		builtin_cd(char **args, t_env **env_list);
int		builtin_echo(char **args);
int		builtin_env(t_env *env_list);
int		builtin_exit(char **args, t_env **env_list, t_node *root);
int		builtin_export(char **args, t_env **env_list);
void	print_exported_env(t_env *env_list);
int		builtin_pwd(void);
int		builtin_unset(char **args, t_env **env_list);

// env ---------------------------------------------------

// env_init.c
t_env	*env_init(char **envp);
t_env	*env_new(char *key, char *value);
void	env_add_back(t_env **env_list, t_env *new_node);
void	free_env(t_env *env_list);

// env_util.c
char	**env_list_to_array(t_env *env_list);
int		count_env_nodes(t_env *env_list);
void	remove_env_node(t_env **env_list, char *key);

// env_access.c
char	*get_env_value(t_env *env_list, char *key);
void	set_env_value(t_env **env_list, char *key, char *value);

// executor ----------------------------------------------

// exec_ast.c
int		exec_ast(t_node *node, t_env **env_list, t_node *root);

// exec_cmd.c
int		exec_cmd(t_node *node, t_env **env_list, t_node *root);

// exec_external.c
int		exec_external(char **args, t_env **env_list, t_node *root);

// get_exec_path.c
char	*get_cmd_path(char *cmd, t_env *env_list);

// pipe.c
int		exec_pipeline(t_node *node, t_env **env_list, t_node *root);

// redirect.c
int		apply_redirects(t_redirect *redirects);
void	restore_fds(int saved_stdin, int saved_stdout);

// heredoc -----------------------------------------------

// heredoc.c
int		process_heredoc(t_node *node, t_env *env_list);

// generate_tmp_filename.c
char	*generate_tmp_filename(void);

// expander ----------------------------------------------

// expand.c
void	expand_node(t_node *node, t_env *env_list);
char	*append_char(char *str, char c);
char	*expand_heredoc_line(char *line, t_env *env_list);

// expand_args.c
void	expand_args(t_node *node, t_env *env_list);

// expand_args_utils.c
char	**insert_matches(char **args, int index,
			char **matches, int match_count);

// expand_redirects.c
void	expand_redirects(t_node *node, t_env *env_list);
int		has_unquoted_dollar(char *str);

// expand_dollers_quotes.c
char	*expand_string(char *str, t_env *env_list,
			int *has_wildcard);

// wildcard.c
char	**expand_wildcard(char *pattern);

// wildcard_utils.c
int		match_pattern(char *pattern, char *str);
void	sort_str_array(char **array, int count);

// lexer -------------------------------------------------

// lexer.c
t_token	*tokenize(char *line);
void	free_tokens(t_token *tokens);

// classify_operator.c
int		classify_operator(char *s, t_token_type *type, int *len);

// lexer_helpers.c
void	skip_spaces(char **line_ptr);
t_token	*new_token(t_token_type type, char *value);
void	token_add_back(t_token **head, t_token *new_node);
int		is_operator(char c);
int		is_spaces_operator(char c);

// parser ------------------------------------------------

// parser.c
t_node	*parse(t_token **tokens);
t_node	*parse_list(t_token **tokens);

// parse_command.c
t_node	*parse_command(t_token **tokens);

// parser_helpers.c
t_node	*new_node(t_node_type type, t_node *left, t_node *right);
int		is_redirect(t_token_type type);
void	append_arg(t_node *node, char *value);

// free_ast.c
void	free_ast(t_node *node);
void	free_str_array(char **array);

// root --------------------------------------------------

// signal.c
void	handler_interactive(int signum);
void	set_signal_interactive(void);
void	set_signal_executing(void);
void	set_signal_child(void);

// signal_heredoc.c
void	set_signal_heredoc(void);

// status.c ----------------------------------------------

int		calculate_exit_status(int status);
int		calculate_exit_status_quit(int status);
void	update_exit_status(t_env **env_list, int status);
void	cleanup_and_exit(int status, t_node *ast, t_env *env);

#endif
