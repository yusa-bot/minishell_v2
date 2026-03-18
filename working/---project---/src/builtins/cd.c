/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:09:37 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 18:31:56 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// cd - の処理: OLDPWD を path に設定し、移動先を表示する
static int	cd_minus(t_env **env_list, char **path);

int	builtin_cd(char **args, t_env **env_list)
{
	char	cwd[PATH_MAX];
	char	*path;

	path = args[1];

	// 引数が多すぎる
	if (args[1] && args[2])
	{
		ft_putendl_fd("minishell: cd: too many arguments", STDERR_FILENO);
		return (1);
	}

	// 引数なしは HOME へ
	if (!path)
	{
		path = get_env_value(*env_list, "HOME");
		if (!path)
		{
			ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
			return (1);
		}
	}

	// cd - は OLDPWD へ
	else if (ft_strcmp(path, "-") == 0)
	{
		if (cd_minus(env_list, &path))
			return (1);
	}

	// 移動前に現在の場所を OLDPWD として保持
	getcwd(cwd, sizeof(cwd));

	// change dir
	if (chdir(path) != 0)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		return (1);
	}

	// 環境変数の更新
	set_env_value(env_list, "OLDPWD", cwd);
	getcwd(cwd, sizeof(cwd));
	set_env_value(env_list, "PWD", cwd);

	return (0);
}

// cd - の処理: OLDPWD を path に設定し、移動先を表示する
static int	cd_minus(t_env **env_list, char **path)
{
	//
	*path = get_env_value(*env_list, "OLDPWD");
	if (!(*path))
	{
		ft_putendl_fd("cd: OLDPWD not set", STDERR_FILENO);
		return (1);
	}

	ft_putendl_fd(*path, STDOUT_FILENO);

	return (0);
}