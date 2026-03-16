/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:59 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/16 12:00:23 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// 受信したシグナルのみを示す
volatile sig_atomic_t g_sig = 0;

static t_node	*build_ast(char *line, t_env **env_list);
static void		exec_line(char *line, t_env **env_list);
static void		read_prompt(t_env **env_list);

int	main(int argc, char **argv, char **envp)
{
	t_env	*env_list;
	int		exit_status;

	(void)argc;
	(void)argv;

	env_list = env_init(envp);

	read_prompt(&env_list);

	rl_clear_history();
	exit_status = ft_atoi(get_env_value(env_list, "?"));
	free_env(env_list);

	return (exit_status);
}

// プロンプト入力ループ
static void	read_prompt(t_env **env_list)
{
	char	*line;

	while (1)
	{
		set_signal_interactive();

		line = readline("minishell$ ");
		if (line == NULL) // Ctrl-D (EOF)
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

		exec_line(line, env_list);
		free(line);
	}
}

// 1行を実行: AST構築 -> 実行
static void	exec_line(char *line, t_env **env_list)
{
	t_node	*node;

	node = build_ast(line, env_list);
	if (!node)
		return ;

	// コマンド実行中のシグナルハンドラに切り替え (親プロでは無視)
	set_signal_executing();

	exec_ast(node, env_list);

	free_ast(node);
}

// lexer -> parser -> process_heredoc -> ASTを返す
static t_node	*build_ast(char *line, t_env **env_list)
{
	t_token	*tokens;
	t_node	*node;

	tokens = tokenize(line);
	if (!tokens)
		return (NULL);

	node = parse(&tokens);
	free_tokens(tokens);
	if (!node)
		return (NULL);

	if (process_heredoc(node, *env_list))
	{
		free_ast(node);
		return (NULL);
	}

	return (node);
}
