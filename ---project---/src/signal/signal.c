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

	// 画面上に改行を入れて、プロンプトが被らないようにする
	write(STDOUT_FILENO, "\n", 1);

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

// 親プロ
void set_signal_executing(void)
{
	// SIGINT: 無視
	signal(SIGINT, SIG_IGN);

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