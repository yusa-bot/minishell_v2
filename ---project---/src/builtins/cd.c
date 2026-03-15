/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:09:37 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	builtin_cd(char **args, t_env **env_list)
{
	char	cwd[PATH_MAX];
	char	*path;

	path = args[1];

	// 引数なしは HOME へ
	if (!path)
		path = get_env_value(*env_list, "HOME");

	// 移動前に現在の場所を OLDPWD として保持
	getcwd(cwd, sizeof(cwd));

	// change dir
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}

	// 環境変数の更新
	set_env_value(env_list, "OLDPWD", cwd);
	getcwd(cwd, sizeof(cwd));
	set_env_value(env_list, "PWD", cwd);

	return (0);
}