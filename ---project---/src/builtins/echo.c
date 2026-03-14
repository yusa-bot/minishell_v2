/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:09:44 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/14 15:02:21 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_echo(char **args)
{
	int	i;
	int	n_option;

	i = 1;
	n_option = 0;

	// -n あるか
	while (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
		n_option = 1;
		i++;
	}

	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);

		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);

		i++;
	}

	if (!n_option)
		ft_putchar_fd('\n', STDOUT_FILENO);

	return (0);
}
