/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_tmp_filename.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 17:50:58 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:59:06 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

char	*generate_tmp_filename(void)
{
	char	*suffix;
	char	*filename;

	while (1)
	{
		suffix = get_random_suffix();
		if (!suffix)
			return (NULL);
		filename = ft_strjoin("/tmp/.minishell_heredoc_", suffix);
		free(suffix);
		if (!filename)
			return (NULL);
		if (access(filename, F_OK) == -1)
			break ;
		free(filename);
	}
	return (filename);
}

// Read random numbers from /dev/urandom and generate an 8-character hexadecimal string
static char	*get_random_suffix(void)
{
	int				fd;
	char			*hex = "0123456789abcdef";
	char			*suffix;
	unsigned char	buf[4];
	int				i;

	suffix = (char *)malloc(sizeof(char) * 9);
	if (!suffix)
		return (NULL);
	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
	{
		free(suffix);
		return (NULL);
	}
	read(fd, buf, 4);
	close(fd);
	i = 0;
	while (i++ < 4)
	{
		suffix[i * 2] = hex[buf[i] / 16];
		suffix[i * 2 + 1] = hex[buf[i] % 16];
	}
	suffix[8] = '\0';
	return (suffix);
}
