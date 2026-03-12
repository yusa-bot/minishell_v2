/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:04 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/12 21:29:00 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// コマンド実行
// 展開 -> リダイレクト適用 -> 実行分岐
int exec_cmd(t_node *node, t_env **env_list);

// ビルトインコマンド
// ビルトインコマンドか判定
int is_builtin(char *cmd);
// ビルトインコマンドの実装関数を呼び出す
int exec_builtin(char **args, t_env **env_list);

// 外部コマンド
// 子プロでexecve()
int exec_external(char **args, t_env **env_list);
// 実行可能ファイルのフルパスを取得
char *get_cmd_path(ch	r *cmd, t_env *env_list);


// コマンド実行 ----------------------------------------------
// 展開 -> リダイレクト適用 -> 実行分岐
int exec_cmd(t_node *node, t_env **env_list);
{
	int	status;
	int	saved_stdin;
	int	saved_stdout;

	// 変数展開とクォート除去
	expand_node(node, *env_list);
	if (!node->args || !node->args[0]) // args(実行コマンド)が無い
		return (0);

	// 親プロセスの標準入出力をバックアップ
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);

	// リダイレクトの適用
	if (apply_redirects(node->redirects) != 0)
	{ // 失敗
		restore_fds(saved_stdin, saved_stdout); // 親プロセスの元FDに復元
		return (1);
	}

	// コマンドの実行分岐
	if (is_builtin(node->args[0]))
		status = exec_builtin(node->args, env_list); // ビルトイン
	else
		status = exec_external(node->args, env_list); // 外部

	restore_fds(saved_stdin, saved_stdout); // 親プロセスの元FDに復元

	return (status);

	//引数（args）の変数展開やクォート除去（Expander）を行う
	//リダイレクトを適用する
	//コマンド名（args[0]）を見て、ビルトインか外部コマンドかを判定し、
	//	それぞれの実行関数に振り分ける
	//ビルトインを親プロセスで実行した場合は、切り替えた標準入出力を元に戻す
}

// ビルトインコマンド ----------------------------------------------
// echo, cd, pwd, export, unset, env, exit か -> 親プロ
int is_builtin(char *cmd)
{
	if (!cmd)
		return (0);

	if (ft_strcmp(cmd, "echo") == 0) return (1);
	if (ft_strcmp(cmd, "cd") == 0) return (1);
	if (ft_strcmp(cmd, "pwd") == 0) return (1);
	if (ft_strcmp(cmd, "export") == 0) return (1);
	if (ft_strcmp(cmd, "unset") == 0) return (1);
	if (ft_strcmp(cmd, "env") == 0) return (1);
	if (ft_strcmp(cmd, "exit") == 0) return (1);

	return (0);
}

// ビルトインコマンドの実装関数を呼び出す -> 終了ステータスを返す
int exec_builtin(char **args, t_env **env_list)
{
	if (ft_strcmp(args[0], "echo") == 0)
		return (builtin_echo(args));
	if (ft_strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, env_list));
	if (ft_strcmp(args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(args[0], "export") == 0)
		return (builtin_export(args, env_list));
	if (ft_strcmp(args[0], "unset") == 0)
		return (builtin_unset(args, env_list));
	if (ft_strcmp(args[0], "env") == 0)
		return (builtin_env(*env_list));
	if (ft_strcmp(args[0], "exit") == 0)
		return (builtin_exit(args, env_list));

	return (1);
}

// 外部コマンド ----------------------------------------------
// 子プロでexecve()
int exec_external(char **args, t_env **env_list)
{
	pid_t	pid;
	int		status;
	char	*path;
	char	**envp;

	pid = fork();

	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}

	if (pid == 0) // 子プロセス
	{
		// シグナルハンドラをデフォルトに戻す -> 子プロは、Ctrl-Cで終了
		set_default_signals();

		// コマンドのフルパスを取得
		path = get_cmd_path(args[0], *env_list);
		if (!path)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(args[0], 2);
			ft_putstr_fd(": command not found\n", 2);
			exit(127);
		}

		// 環境変数リストを char ** の配列に逆変換
		envp = env_list_to_array(*env_list);

		// execve でプロセスを上書き
		execve(path, args, envp);

		// execve が戻ってきた場合（実行権限がない等）はエラー処理
		perror("minishell");
		free(path);
		free_str_array(envp);
		exit(126);
	}

	waitpid(pid, &status, 0);

	return (calculate_exit_status(status));

	//fork() で子プロセスを生成
	//get_cmd_path() を呼び出し、実行可能ファイルのフルパスを取得
	//env_list_to_array() を使って環境変数リストを char ** の配列に逆変換
	//execve() を呼び出してプロセスを上書き
	//親プロセスは waitpid で子の終了を待ち、calculate_exit_status() でステータスを返す
}

// 実行可能ファイルのフルパスを取得
char *get_cmd_path(char *cmd, t_env *env_list)
{
	char	**paths;
	char	*path_env;
	char	*tmp;
	char	*full_path;
	int		i;

	if (!cmd || !*cmd)
		return (NULL);

	// '/'を含んでいる場合、既にフルパスなのでreturn
	if (ft_strchr(cmd, '/'))
	{
		// 実行できる
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));

		// 実行できない
		return (NULL);
	}

	// env_listからkeyが"PATH"のvalueを取得
	path_env = get_env_value(env_list, "PATH");
	if (!path_env)
		return (NULL);

	// PATH を ':' で分割
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);

	// 各ディレクトリにコマンド名を結合して実行可能かテスト
	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);

		if (access(full_path, X_OK) == 0)
		{
			free_str_array(paths);
			return (full_path);
		}

		free(full_path);
		i++;
	}

	free_str_array(paths);
	return (NULL);

	//環境変数 $PATH を : で分割
	//各ディレクトリにコマンド名を結合
	//access(path, X_OK) で実行可能かチェック
}
