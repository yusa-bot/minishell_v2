/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:35 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/24 22:34:42 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

int	apply_redirects(t_redirect *redirects)
{
	t_redirect	*current;

	current = redirects;
	while (current)
	{
		if (handle_redir_node(current) != 0)
			return (1);
		current = current->next;
	}
	return (0);
}

void	restore_fds(int saved_stdin, int saved_stdout)
{
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);
}
