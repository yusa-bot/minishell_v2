/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:49 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 16:13:52 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

t_node *parse(t_token **tokens);
static t_node *parse_list(t_token **tokens);
static t_node *parse_pipeline(t_token **tokens);

// Parse starting with the lowest-priority tokens

// Entrance -> parse_list
t_node *parse(t_token **tokens)
{
	t_node	*root;

	if (!tokens || !*tokens)
		return (NULL);
	root = parse_list(tokens);
	if (root == NULL)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		if (*tokens)
			ft_putstr_fd((*tokens)->value, 2);
		else
			ft_putstr_fd("newline", 2);
		ft_putstr_fd("'\n", 2);
		return (NULL);
	}
	if (*tokens != NULL)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd((*tokens)->value, 2);
		ft_putstr_fd("'\n", 2);

		free_ast(root);
		return (NULL);
	}
	return (root);
}

// &&, || is Lowest priority -> parse_pipeline
static t_node *parse_list(t_token **tokens)
{
	t_node *node;
	t_node *right;
	t_node_type type;
	node = parse_pipeline(tokens);
	if (!node)
		return (NULL);
	while (*tokens && ((*tokens)->type == TK_AND || (*tokens)->type == TK_OR))
	{
		type = ((*tokens)->type == TK_AND) ? NODE_AND : NODE_OR;
		*tokens = (*tokens)->next;
		right = parse_pipeline(tokens);
		if (!right)
			return (NULL);
		node = new_node(type, node, right);
	}
	return (node);
}

// Next, process the lower-priority pipe -> parse_command
static t_node *parse_pipeline(t_token **tokens)
{
	t_node *node;
    t_node *right;

	node = parse_command(tokens);
	if (!node)
		return (NULL);
	while (*tokens && (*tokens)->type == TK_PIPE)
	{
		*tokens = (*tokens)->next;
		right = parse_command(tokens);
		if (!right)
			return (NULL);
		node = new_node(NODE_PIPE, node, right);
	}
	return (node);
}
