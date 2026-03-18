/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:29:58 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/17 21:35:34 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	ft_isnumeric(char *str);
static int	ft_isoverflow(char *str);
static void	exit_numeric_error(char *arg, t_node *root, t_env *env_list);

int	builtin_exit(char **args, t_env **env_list, t_node *root)
{
	int	exit_code;

	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!args[1])
	{
		exit_code = ft_atoi(get_env_value(*env_list, "?"));
		cleanup_and_exit(exit_code, root, *env_list);
	}
	if (!ft_isnumeric(args[1]) || ft_isoverflow(args[1]))
		exit_numeric_error(args[1], root, *env_list);
	if (args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments",
			STDERR_FILENO);
		return (1);
	}
	exit_code = ft_atoi(args[1]);
	cleanup_and_exit(exit_code & 0xFF, root, *env_list);
	return (0);
}

static void	exit_numeric_error(char *arg, t_node *root, t_env *env_list)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	cleanup_and_exit(2, root, env_list);
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

// long longの範囲を超えるか判定 (ft_isnumericを通過済みの前提)
static int	ft_isoverflow(char *str)
{
	int			sign;
	long long	res;
	int			digit;

	sign = 1;
	res = 0;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str)
	{
		digit = *str - '0';
		if (sign == 1 && (res > (LLONG_MAX - digit) / 10))
			return (1);
		if (sign == -1 && (res > (-(LLONG_MIN + digit)) / 10))
			return (1);
		res = res * 10 + digit;
		str++;
	}
	return (0);
}
