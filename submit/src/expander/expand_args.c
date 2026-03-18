/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:54:59 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:15:05 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	expand_arg_wildcard(t_node *node, int i, char *expanded);
static int	remove_empty_arg(t_node *node, int i, char *expanded);
static int	try_word_split(t_node *node, int *i, char *expanded);
static char	**insert_matches_to_args(char **args, int index, char **matches, int match_count);

void	expand_args(t_node *node, t_env *env_list)
{
	int		i;
	int		has_wildcard;
	char	*expanded;

	i = 0;
	while (node->args[i])
	{
		expanded = expand_string(node->args[i], env_list, &has_wildcard);
		if (has_wildcard == 1)
		{
			i = expand_arg_wildcard(node, i, expanded);
			i++;
			continue ;
		}
		if (expanded[0] == '\0'
			&& !ft_strchr(node->args[i], '\'')
			&& !ft_strchr(node->args[i], '"'))
		{
			if (remove_empty_arg(node, i, expanded))
				continue ;
		}
		if (try_word_split(node, &i, expanded))
			continue ;
		free(node->args[i]);
		node->args[i] = expanded;
		i++;
	}
}

// wildcard expand -> insert args
static int	expand_arg_wildcard(t_node *node, int i, char *expanded)
{
	char	**matches;
	int		count;

	matches = expand_wildcard(expanded);
	count = 0;
	while (matches && matches[count])
		count++;
	free(node->args[i]);
	free(expanded);
	node->args = insert_matches_to_args(node->args, i, matches, count);
	if (count > 0)
		return (i + count - 1);
	return (i);
}

static int	remove_empty_arg(t_node *node, int i, char *expanded)
{
	int	j;

	free(node->args[i]);
	free(expanded);
	j = i;
	while (node->args[j + 1])
	{
		node->args[j] = node->args[j + 1];
		j++;
	}
	node->args[j] = NULL;
	return (1);
}

static int	try_word_split(t_node *node, int *i, char *expanded)
{
	char	**splits;
	int		count;

	if (!has_unquoted_dollar(node->args[*i])
		|| !ft_strchr(expanded, ' '))
		return (0);
	splits = ft_split_c(expanded, ' ');
	count = 0;
	while (splits && splits[count])
		count++;
	if (count <= 1)
	{
		free_str_array(splits);
		return (0);
	}
	free(node->args[*i]);
	free(expanded);
	node->args = insert_matches_to_args(node->args, *i, splits, count);
	*i += count;
	return (1);
}

// Insert all elements of the `matches` array into the original `args`
static char	**insert_matches_to_args(char **args, int index, char **matches, int match_count)
{
	int		old_len = 0;
	int		i = 0, j = 0, k = 0;
	char	**new_args;

	if (!args || !matches)
		return (args);
	while (args[old_len])
		old_len++;
	new_args = (char **)malloc(sizeof(char *) * (old_len + match_count));
	if (!new_args)
		return (NULL);
	while (i < index)
		new_args[k++] = args[i++];
	while (j < match_count)
		new_args[k++] = matches[j++];
	i++;
	while (i < old_len)
		new_args[k++] = args[i++];
	new_args[k] = NULL;
	free(args);
	free(matches);
	return (new_args);
}