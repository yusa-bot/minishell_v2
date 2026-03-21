/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:59 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 10:59:08 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

volatile sig_atomic_t	g_sig = 0;

static void		read_prompt(t_env **env_list, struct termios *default_term);

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
		line = readline("何か打ち込んでみろ! ");
		if (line == NULL)
		{
			printf("exit\n");
			break ;
		}
		if (g_sig == SIGINT)
		{
			update_exit_status(env_list, EXIT_SIGINT);
			g_sig = 0;
		}
		if (*line != '\0')
			add_history(line);
		exec_multiline(line, env_list);
		free(line);
	}
}
