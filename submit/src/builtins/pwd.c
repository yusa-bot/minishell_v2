/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:29 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	builtin_pwd(t_env **env_list)
{
	char	cwd[PATH_MAX];
	char	*pwd_val;

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		ft_putendl_fd(cwd, STDOUT_FILENO);
		return (0);
	}
	pwd_val = get_env_value(*env_list, "PWD");
	if (pwd_val)
	{
		ft_putendl_fd(pwd_val, STDOUT_FILENO);
		return (0);
	}
	perror("pwd");
	return (1);
}
