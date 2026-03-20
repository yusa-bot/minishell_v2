/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:04 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 18:45:35 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	is_builtin(char *cmd);
static int	exec_builtin(char **args, t_env **env_list, t_node *root);

// Expand -> Apply Redirect -> Execute Branch
int	exec_cmd(t_node *node, t_env **env_list, t_node *root)
{
	int	status;
	int	saved_stdin;
	int	saved_stdout;

	expand_node(node, *env_list);
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (apply_redirects(node->redirects) != 0)
	{
		restore_fds(saved_stdin, saved_stdout);
		return (1);
	}
	if (!node->args || !node->args[0])
	{
		restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	if (is_builtin(node->args[0]))
	{
		if (ft_strcmp(node->args[0], "exit") == 0)
		{
			restore_fds(saved_stdin, saved_stdout);
			return (exec_builtin(node->args, env_list, root));
		}
		status = exec_builtin(node->args, env_list, root);
	}
	else
		status = exec_external(node->args, env_list, root);
	restore_fds(saved_stdin, saved_stdout);
	return (status);
}

// Builtin command ----------------------------------------------

static int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

static int	exec_builtin(char **args, t_env **env_list, t_node *root)
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
		return (builtin_exit(args, env_list, root));
	return (1);
}
