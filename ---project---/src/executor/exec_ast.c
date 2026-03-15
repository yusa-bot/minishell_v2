/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:00 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// parser.cで格納されたt_nodeから、t_node_typeを判定し適切な実行処理へと振り分けるルーター
//		-> exec_ast.c(&&, ||, ()), pipe.c, exec_cmd.c

// AST -> &&, ||, () に振り分け
// AST評価 -> type に応じて処理を分岐 -> $?を返す
int exec_ast(t_node *node, t_env **env_list);
// NODE_AND (&&)を処理.
//		左ノードを実行し、その終了ステータスが0であった場合のみ、右ノードを実行
static int exec_and(t_node *node, t_env **env_list);
// NODE_OR (||) を処理.
//		左ノードを実行し、その終了ステータスが0以外であった場合のみ、右ノードを実行
static int exec_or(t_node *node, t_env **env_list);
// NODE_SUBSHELL (()) を処理.
//		forkで子プロ生成 -> 子プロでexec_ast() -> 親プロが子プロのステータスを返す
static int exec_subshell(t_node *node, t_env **env_list);


// AST -> &&, ||, () に振り分け ----------------------------------------------
// AST評価 -> type に応じて処理を分岐 (分解より下層は分岐先で再帰) -> $?を返す
int exec_ast(t_node *node, t_env **env_list)
{
	int	status;

	if (node == NULL)
		return (0);

	if (node->type == NODE_CMD) // NODE_CMD
		status = exec_cmd(node, env_list);

	else if (node->type == NODE_PIPE) // NODE_PIPE
		status = exec_pipeline(node, env_list);

	else if (node->type == NODE_AND) // NODE_AND
		status = exec_and(node, env_list);

	else if (node->type == NODE_OR) // NODE_OR
		status = exec_or(node, env_list);

	else if (node->type == NODE_SUBSHELL)
		status = exec_subshell(node, env_list);

	// 環境変数の $? を更新
	update_exit_status(env_list, status);

	return (status);
}

// NODE_AND (&&)を処理.
//		左ノードを実行し、その終了ステータスが0であった場合のみ、右ノードを実行
static int exec_and(t_node *node, t_env **env_list)
{
	int	status;

	if (!node)
		return (0);

	// '&&'を跨いだ左項
	status = exec_ast(node->left, env_list); // 再帰
	update_exit_status(env_list, status); // $? を更新

	// '&&'を跨いだ左項が成功
	if (status == 0)
	{
		status = exec_ast(node->right, env_list); // 再帰
		update_exit_status(env_list, status); // 右項の結果で $? を上書き
	}

	return (status);
}

// NODE_OR (||) を処理.
//		左ノードを実行し、その終了ステータスが0以外であった場合のみ、右ノードを実行
static int exec_or(t_node *node, t_env **env_list)
{
	int	status;

	if (!node)
		return (0);

	// '||'を跨いだ左項
	status = exec_ast(node->left, env_list); // 再帰
	update_exit_status(env_list, status); // $? を更新

	// '||'を跨いだ左項が成功
	if (status != 0)
	{
		status = exec_ast(node->right, env_list); // 再帰
		update_exit_status(env_list, status); // 右項の結果で $? を上書き
	}

	return (status);
}

// NODE_SUBSHELL (()) を処理.
//		forkで子プロ生成 -> 子プロでexec_ast() -> 親プロが子プロのステータスを返す
static int exec_subshell(t_node *node, t_env **env_list)
{
	pid_t	pid;
	int		status;

	pid = fork();

	if (pid < 0)
		return (1); // fork失敗

	// 子プロ
	if (pid == 0)
	{
		// サブシェルノード自体にリダイレクト((ls && pwd) > out.txt)の場合、dup2

		status = exec_ast(node->left, env_list);

		// 子プロセスはexitで終わらせる
		exit(status);
	}

	// 親プロ
	waitpid(pid, &status, 0);

	// waitpid のステータスマクロ(WIFEXITED等)を使って正確な終了コードを計算して返す
	return (calculate_exit_status(status));
}
