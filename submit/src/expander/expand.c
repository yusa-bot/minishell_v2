/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 21:28:34 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:05:25 by ayusa            ###   ########.fr       */
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

// Free the original string and append one character to the end of the string
char	*append_char(char *str, char c)
{
	char	*new_str;
	int		len;
	int		i;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	new_str = (char *)malloc(sizeof(char) * (len + 2));
	if (!new_str)
	{
		free(str);
		return (NULL);
	}
	i = 0;
	while (i < len)
	{
		new_str[i] = str[i];
		i++;
	}
	new_str[i] = c;
	new_str[i + 1] = '\0';
	free(str);
	return (new_str);
}

char	*expand_heredoc_line(char *line, t_env *env_list)
{
	int		has_wildcard;
	char	*result;

	result = expand_string(line, env_list, &has_wildcard);
	free(line);
	return (result);
}
