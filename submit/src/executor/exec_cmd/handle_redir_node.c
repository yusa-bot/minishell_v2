/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redir_node.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 22:34:01 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/24 22:34:49 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

static int	handle_input(t_redirect *redir);
static int	handle_output(t_redirect *redir);
static int	handle_rdwr(t_redirect *redir);

int	handle_redir_node(t_redirect *current)
{
	if (current->type != TK_HEREDOC
		&& (!current->filename || !current->filename[0]))
		return (1);
	if (current->type == TK_REDIR_IN
		|| current->type == TK_HEREDOC)
	{
		if (handle_input(current) < 0)
			return (1);
	}
	else if (current->type == TK_REDIR_OUT
		|| current->type == TK_REDIR_APPEND)
	{
		if (handle_output(current) < 0)
			return (1);
	}
	else if (current->type == TK_REDIR_RDWR)
	{
		if (handle_rdwr(current) < 0)
			return (1);
	}
	return (0);
}

static int	handle_input(t_redirect *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(redir->filename);
		return (-1);
	}
	if (redir->type == TK_HEREDOC)
		unlink(redir->filename);
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	handle_output(t_redirect *redir)
{
	int	fd;

	if (redir->type == TK_REDIR_OUT)
		fd = open(redir->filename,
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(redir->filename,
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(redir->filename);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int	handle_rdwr(t_redirect *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDWR | O_CREAT, 0644);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(redir->filename);
		return (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}
