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

// search cmd in PATH directories (handles empty elements as ".")
static char	*search_in_paths(char *cmd, char *path_env)
{
	char	*colon;
	char	*dir;
	char	*tmp;
	char	*full_path;
	size_t	len;

	while (1)
	{
		colon = ft_strchr(path_env, ':');
		if (colon)
			len = colon - path_env;
		else
			len = ft_strlen(path_env);
		if (len == 0)
			dir = ft_strdup(".");
		else
			dir = ft_substr(path_env, 0, len);
		if (!dir)
			return (NULL);
		tmp = ft_strjoin(dir, "/");
		free(dir);
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		if (!colon)
			break ;
		path_env = colon + 1;
	}
	return (NULL);
}
