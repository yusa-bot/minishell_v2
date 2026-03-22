/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 22:41:00 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/22 22:41:00 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

char	*read_heredoc_line(void)
{
	char	c;
	char	*line;
	ssize_t	n;

	line = ft_strdup("");
	if (!line)
		return (NULL);
	while (1)
	{
		n = read(STDIN_FILENO, &c, 1);
		if (n <= 0)
		{
			free(line);
			return (NULL);
		}
		if (c == '\n')
			return (line);
		line = append_char(line, c);
		if (!line)
			return (NULL);
	}
}
