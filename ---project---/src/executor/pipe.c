/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:33 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/17 23:18:18 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// pipe
// pipe(2つのfdを繋ぐ) -> 左(書), 右(読)の子プロを作成 -> 実行関数へ -> 親プロで子プロを待つ
int	exec_pipeline(t_node *node, t_env **env_list);
// 左（書）の子プロセス で fd[1] に書き込み準備 -> node下層を呼び出し再帰
static void	exec_pipe_left(t_node *node, t_env **env_list, int fd[2]);
// 右（読）の子プロセス で fd[0] から読み取り -> node下層を呼び出し再帰
static void	exec_pipe_right(t_node *node, t_env **env_list, int fd[2]);


// pipe ----------------------------------------------
// pipe(2つのfdを繋ぐ) -> 左(書), 右(読)の子プロを作成 -> 実行関数へ -> 親プロで子プロを待つ
int	exec_pipeline(t_node *node, t_env **env_list)
{
	int		fd[2];
	pid_t	pids[2];
	int		status;

	// pipe (プロセス間の接続口) 作成 -> fd[0]:読み取り(右が使う), fd[1]:書き込み(左が使う)
	if (pipe(fd) < 0)
	{
		perror("minishell: pipe");
		return (1);
	}

	// .1左用の子プロを作成 (fd[2]は引き継ぐ)
	pids[0] = fork();
	if (pids[0] == 0) // 子プロだったら
		exec_pipe_left(node, env_list, fd); // -> 左コマンド実行

	// 2.右用の子プロを作成 (fd[2]は引き継ぐ)
	pids[1] = fork();
	if (pids[1] == 0) // 子プロだったら
		exec_pipe_right(node, env_list, fd); // -> 右コマンド実行

	// 親プロ: 1.左 2.右 のfdを閉める
	close(fd[0]);
	close(fd[1]);

	// 両方の子プロセスの終了を待つ
	waitpid(pids[0], NULL, 0);
	waitpid(pids[1], &status, 0); // 右(読)のstatusを取得

	return (calculate_exit_status_quit(status));

}

// 左（書）の子プロセス で fd[1] に書き込み準備 -> node下層を呼び出し再帰
static void	exec_pipe_left(t_node *node, t_env **env_list, int fd[2])
{
	// パイプ子プロ: シグナルをデフォルトに戻す (シグナルで直接死ねるようにする)
	set_signal_child();
	// 読み込み端は使わないので閉じる
	close(fd[0]);

	// STDOUT_FILENO を fd[1] で上書き -> fd[1]に書き込める
	dup2(fd[1], STDOUT_FILENO);
	close(fd[1]);

	// 実行する環境が整ったので、木の下層を再帰で実行
	//	子プロなので、exitする
	exit(exec_ast(node->left, env_list));
}

// 右（読）の子プロセス で fd[0] から読み取り -> node下層を呼び出し再帰
static void	exec_pipe_right(t_node *node, t_env **env_list, int fd[2])
{
	// パイプ子プロ: シグナルをデフォルトに戻す (シグナルで直接死ねるようにする)
	set_signal_child();
	// 書き込み端は使わないので閉じる
	close(fd[1]);

	// STDIN_FILENO を fd[0] で上書き -> fd[0]から読み取れる
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);

	// 実行する環境が整ったので、木の下層を再帰で実行
	//	子プロなので、exitする
	exit(exec_ast(node->right, env_list));
}
