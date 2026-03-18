/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:00:30 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:57:01 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// waitpid のステータスマクロ(WIFEXITED等)を使って正確な終了コードを計算して返す
int	calculate_exit_status(int status);
// calculate_exit_status + Quit print
int calculate_exit_status_quit(int status);
// 環境変数の $? を更新
void	update_exit_status(t_env **env_list, int status);

// AST・env・historyを解放してexit (子プロセス・builtin exit用)
void	cleanup_and_exit(int status, t_node *ast, t_env *env);

// waitpid のステータスマクロ(WIFEXITED等)を使って正確な終了コードを計算して返す
int	calculate_exit_status(int status)
{
	// 正常終了した場合
	if (WIFEXITED(status))
		return (WEXITSTATUS(status)); // 番号を取得

	// シグナルによって終了させられた場合
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status)); // 128+終了させたシグナル番号

	// 停止状態など、その他の場合はそのまま返す
	return (status);
}

// calculate_exit_status + Quit print
int calculate_exit_status_quit(int status)
{
    int term_sig;

    // 正常終了した場合
    if (WIFEXITED(status))
        return (WEXITSTATUS(status)); // 番号を取得

    // シグナルによって終了させられた場合
    if (WIFSIGNALED(status))
    {
        term_sig = WTERMSIG(status); // 終了させたシグナル番号を取得

        // SIGQUIT: "Quit" を出力 (コアダンプ時は "(core dumped)" も付加)
        if (term_sig == SIGQUIT)
        {
            if (WCOREDUMP(status))
                ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
            else
                ft_putstr_fd("Quit\n", STDERR_FILENO);
        }

        return (128 + term_sig); // 128 + 終了させたシグナル番号
    }

    // 停止状態など、その他の場合はそのまま返す
    return (status);
}

// 環境変数の $? を更新
void	update_exit_status(t_env **env_list, int status)
{
	char	*status_str;

	status_str = ft_itoa(status); /// malloc
	if (!status_str)
		return ;

	set_env_value(env_list, "?", status_str); // 内部で値をft_strdup

	free(status_str);
}

// AST・env・historyを解放してexit (子プロセス・builtin exit用)
void	cleanup_and_exit(int status, t_node *ast, t_env *env)
{
	if (ast)
		free_ast(ast);
	if (env)
		free_env(env);
	rl_clear_history();
	exit(status);
}