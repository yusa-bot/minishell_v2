/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:29:58 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 18:21:48 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	ft_isnumeric(char *str);

int	builtin_exit(char **args, t_env **env_list)
{
	int	exit_code;

	ft_putendl_fd("exit", STDERR_FILENO);

	if (!args[1])
	{
		exit_code = ft_atoi(get_env_value(*env_list, "?"));
		exit(exit_code);
	}

	// 引数が非数値の場合
	if (!ft_isnumeric(args[1]))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		exit(2);
	}

	// 引数が exit '' '' のように多い
	if (args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}

	exit_code = ft_atoi(args[1]);

	// 受け取った32bit -> status格納領域8bit へ切り出す
	exit(exit_code & 0xFF);
}

static int	ft_isnumeric(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	// 先頭の符号をスキップ
	if (str[i] == '+' || str[i] == '-')
		i++;
	// 符号だけで数字が続いていない場合（例: "exit +"）は非数値
	if (str[i] == '\0')
		return (0);
	// 残りの文字が全て数字かチェック
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}