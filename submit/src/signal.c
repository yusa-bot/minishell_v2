/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:55 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:16:23 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// A: Waiting for input at the prompt
void	handler_interactive(int signum);
void	set_signal_interactive(void);

// B: Command executing (child process)
void	set_signal_executing(void);
void	set_signal_child(void);

// A: Waiting for input at the prompt

// SIGINT
void	handler_interactive(int signum)
{
	g_sig = signum;
	write(STDOUT_FILENO, "^C\n", 3);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	set_signal_interactive(void)
{
	signal(SIGINT, handler_interactive);
	signal(SIGQUIT, SIG_IGN);
}

// B: Command executing (child process)

static void	handler_executing(int signum)
{
	g_sig = signum;
}

// Parent process
void	set_signal_executing(void)
{
	signal(SIGINT, handler_executing);
	signal(SIGQUIT, SIG_IGN);
}

// Child process
void	set_signal_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
