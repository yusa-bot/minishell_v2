/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:36:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 19:54:10 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

t_node	*new_node(t_node_type type, t_node *left, t_node *right)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->left = left;
	node->right = right;
	node->args = NULL;
	node->redirects = NULL;
	return (node);
}

int	is_redirect(t_token_type type)
{
	if (type == TK_REDIR_IN || type == TK_REDIR_OUT
		|| type == TK_REDIR_APPEND || type == TK_HEREDOC
		|| type == TK_REDIR_RDWR)
		return (1);
	return (0);
}

//　Reallocate memory for `args` and add it
void	append_arg(t_node *node, char *value)
{
	int		i;
	char	**new_args;

	i = 0;
	if (node->args)
	{
		while (node->args[i])
			i++;
	}
	new_args = (char **)malloc(sizeof(char *) * (i + 2));
	if (!new_args)
		return ;
	i = 0;
	if (node->args)
	{
		while (node->args[i])
		{
			new_args[i] = node->args[i];
			i++;
		}
		free(node->args);
	}
	new_args[i] = ft_strdup(value);
	new_args[i + 1] = NULL;
	node->args = new_args;
}
