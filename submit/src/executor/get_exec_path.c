/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_exec_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 17:39:34 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:43:22 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char	*search_in_paths(char *cmd, char *path_env);

// get full path of executable
char	*get_cmd_path(char *cmd, t_env *env_list)
{
	char	*path_env;
	char	*tmp;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = get_env_value(env_list, "PATH");
	if (!path_env)
	{
		tmp = ft_strjoin("./", cmd);
		if (tmp && access(tmp, X_OK) == 0)
			return (tmp);
		free(tmp);
		return (NULL);
	}
	return (search_in_paths(cmd, path_env));
}

// search cmd in PATH directories
static char	*search_in_paths(char *cmd, char *path_env)
{
	char	**paths;
	char	*tmp;
	char	*full_path;
	int		i;

	paths = ft_split_c(path_env, ':');
	if (!paths)
		return (NULL);
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
}
