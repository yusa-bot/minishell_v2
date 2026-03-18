/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:59 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:25:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

volatile sig_atomic_t	g_sig = 0;

static void		read_prompt(t_env **env_list, struct termios *default_term);
static void		exec_multiline(char *line, t_env **env_list);
static void		exec_line(char *line, t_env **env_list);
static t_node	*build_ast(char *line, t_env **env_list);

// Save terminal information -> env list init -> read prompt -> status return
int	main(int argc, char **argv, char **envp)
{
	t_env			*env_list;
	int				exit_status;
	struct termios	default_term;

	(void)argc;
	(void)argv;
	rl_catch_signals = 0;
	ft_memset(&default_term, 0, sizeof(default_term));
	tcgetattr(STDIN_FILENO, &default_term);
	env_list = env_init(envp);
	read_prompt(&env_list, &default_term);
	rl_clear_history();
	exit_status = ft_atoi(get_env_value(env_list, "?"));
	free_env(env_list);
	return (exit_status);
}

// read prompt -> exec_multiline
static void	read_prompt(t_env **env_list, struct termios *default_term)
{
	char	*line;

	while (1)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, default_term);
		set_signal_interactive();
		line = readline("minishell$ ");
		if (line == NULL)
		{
			printf("exit\n");
			break ;
		}
		if (g_sig == SIGINT)
		{
			update_exit_status(env_list, 130);
			g_sig = 0;
		}
		if (*line != '\0')
			add_history(line);
		exec_multiline(line, env_list);
		free(line);
	}
}

// If bracket paste returns multiple lines, split and execute each
static void	exec_multiline(char *line, t_env **env_list)
{
	char	**lines;
	int		i;

	if (!line || !*line)
		return ;
	if (!ft_strchr(line, '\n'))
	{
		exec_line(line, env_list);
		return ;
	}
	lines = ft_split_c(line, '\n');
	if (!lines)
		return ;
	i = 0;
	while (lines[i])
	{
		exec_line(lines[i], env_list);
		i++;
	}
	free_str_array(lines);
}

// 1 prompt exec: AST Construction(build_ast) -> exec_ast
static void	exec_line(char *line, t_env **env_list)
{
	t_node	*node;

	if (!line || !*line)
		return ;
	node = build_ast(line, env_list);
	if (!node)
	{
		update_exit_status(env_list, 2);
		return ;
	}
	set_signal_executing();
	exec_ast(node, env_list, node);
	if (g_sig == SIGINT)
	{
		ft_putstr_fd("\n", STDERR_FILENO);
		g_sig = 0;
	}
	free_ast(node);
}

// lexer -> parser -> process_heredoc -> return root AST
static t_node	*build_ast(char *line, t_env **env_list)
{
	t_token	*tokens;
	t_token	*head;
	t_node	*node;

	tokens = tokenize(line);
	if (!tokens)
		return (NULL);
	head = tokens;
	node = parse(&tokens);
	free_tokens(head);
	if (!node)
		return (NULL);
	if (process_heredoc(node, *env_list))
	{
		free_ast(node);
		return (NULL);
	}
	return (node);
}
