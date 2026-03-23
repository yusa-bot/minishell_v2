/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 21:28:34 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 21:59:25 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//  expand args[i] or redir->filename
void	expand_node(t_node *node, t_env *env_list)
{
	if (!node)
		return ;
	if (node->args)
		expand_args(node, env_list);
	expand_redirects(node, env_list);
}

char	*expand_heredoc_line(char *line, t_env *env_list)
{
	int		has_wildcard;
	char	*result;

	result = expand_string(line, env_list, &has_wildcard, 1);
	free(line);
	return (result);
}
