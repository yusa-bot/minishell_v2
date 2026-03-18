/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:55 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/16 14:00:48 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

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


// A: プロンプトで入力を待っている ---------------------------------------------

// SIGINT
void handler_interactive(int signum)
{
	g_sig = signum;

	// ^C を表示して改行 (readlineがECHOを無効にしているため自分で出力)
	write(STDOUT_FILENO, "^C\n", 3);

	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void set_signal_interactive(void)
{
	// Ctrl-C: handler_interactive (デフォルトだとshellが終了してしまう)
	signal(SIGINT, handler_interactive);

	// Ctrl-\: 無視
	signal(SIGQUIT, SIG_IGN);
}


// B: コマンド実行中 (子プロセス) ---------------------------------------------

// SIGINT: シグナル番号だけ記録 (親プロは死なない)
static void	handler_executing(int signum)
{
	g_sig = signum;
}

// 親プロ
void set_signal_executing(void)
{
	// SIGINT: g_sigに記録 (実行後に改行出力するため)
	signal(SIGINT, handler_executing);

	// SIGQUIT: 無視
	signal(SIGQUIT, SIG_IGN);
}

// 子プロ
void set_signal_child(void)
{
	// SIGINT: デフォルト
	signal(SIGINT, SIG_DFL);

	// SIGQUIT: デフォルト
	signal(SIGQUIT, SIG_DFL); // Quit: 3
}


// C: heredoc ---------------------------------------------
void set_signal_heredoc(void)
{
	// SIGINT: デフォルト
	signal(SIGINT, SIG_DFL);

	// SIGQUIT: 無視
	signal(SIGQUIT, SIG_IGN);
}