/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_util.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:55 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 18:24:30 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void	free_env_node(t_env *node);

// **envp -> t_env linked list
char	**env_list_to_array(t_env *env_list)
{
	char	**envp;
	char	*tmp;
	int		i;

	envp = (char **)malloc(sizeof(char *) * (count_env_nodes(env_list) + 1));
	if (!envp)
		return (NULL);
	i = 0;
	while (env_list)
	{
		if (env_list->value)
		{
			tmp = ft_strjoin(env_list->key, "=");
			envp[i] = ft_strjoin(tmp, env_list->value);
			free(tmp);
		}
		else
			envp[i] = ft_strdup(env_list->key);
		if (!envp[i])
		{
			envp[i] = NULL;
			free_str_array(envp);
			return (NULL);
		}
		env_list = env_list->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

int	count_env_nodes(t_env *env_list)
{
	int	count;

	count = 0;
	while (env_list)
	{
		count++;
		env_list = env_list->next;
	}
	return (count);
}

void	remove_env_node(t_env **env_list, char *key)
{
	t_env	*current;
	t_env	*prev;

	if (!env_list || !*env_list || !key)
		return ;
	current = *env_list;
	if (ft_strcmp(current->key, key) == 0)
	{
		*env_list = current->next;
		free_env_node(current);
		return ;
	}
	prev = current;
	current = current->next;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			prev->next = current->next;
			free_env_node(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

static void	free_env_node(t_env *node)
{
	free(node->key);
	if (node->value)
		free(node->value);
	free(node);
}
