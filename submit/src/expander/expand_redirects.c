/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redirects.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 16:07:51 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:15:18 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	expand_redir_wildcard(t_redirect *redir, char *expanded);
static void	handle_empty_redir(t_redirect *redir, char *expanded);

void	expand_redirects(t_node *node, t_env *env_list)
{
	t_redirect	*redir;
	char		*expanded;
	int			has_wildcard;

	redir = node->redirects;
	while (redir)
	{
		if (redir->type != TK_HEREDOC)
		{
			expanded = expand_string(
					redir->filename, env_list, &has_wildcard);
			if (has_wildcard
				&& expand_redir_wildcard(redir, expanded))
				return ;
			if (!has_wildcard)
				handle_empty_redir(redir, expanded);
		}
		redir = redir->next;
	}
}

// expand redir wildcard. if ambiguous return 1
static int	expand_redir_wildcard(t_redirect *redir, char *expanded)
{
	char	**matches;
	int		count;

	matches = expand_wildcard(expanded);
	count = 0;
	while (matches && matches[count])
		count++;
	if (count == 1)
	{
		free(redir->filename);
		redir->filename = ft_strdup(matches[0]);
		free_str_array(matches);
		free(expanded);
		return (0);
	}
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(redir->filename, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
	free_str_array(matches);
	free(expanded);
	return (1);
}

// ambiguous redirect
static void	handle_empty_redir(t_redirect *redir, char *expanded)
{
	if (!expanded[0] && has_unquoted_dollar(redir->filename))
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(redir->filename, 2);
		ft_putstr_fd(": ambiguous redirect\n", 2);
		free(expanded);
		redir->filename[0] = '\0';
		return ;
	}
	free(redir->filename);
	redir->filename = expanded;
}

int	has_unquoted_dollar(char *str)
{
	int	in_sq;
	int	in_dq;
	int	i;

	in_sq = 0;
	in_dq = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !in_dq)
			in_sq = !in_sq;
		else if (str[i] == '"' && !in_sq)
			in_dq = !in_dq;
		else if (str[i] == '$' && !in_sq && !in_dq)
			return (1);
		i++;
	}
	return (0);
}
