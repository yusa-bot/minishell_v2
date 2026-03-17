/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:26 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 17:46:48 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//				env       export (引数なし)
//フォーマット   KEY=VALUE    declare -x KEY="VALUE", （VALUE）が必ずダブルクォート（"）
//ソート   	  そのまま  	ASCIIコード順
//値なし変数	   表示なし	  	 表示(declare -x ONLY_KEY)
//(_			 あり 		  なし）

// 引数あり 引数なし に対応
int	builtin_export(char **args, t_env **env_list);
//　引数なしを処理
static void	print_exported_env(t_env *env_list);
// 値の中の ", \, $, ` をエスケープして出力
static void	print_escaped_value(char *value);
// 1つの環境変数ノードを表示
static void	print_single_export(t_env *node);
// 有効な識別子か確認 (先頭: 英字or_, 以降: 英数字or_)
static int	is_valid_identifier(char *name);


// 引数あり 引数なし に対応
int	builtin_export(char **args, t_env **env_list)
{
	int		i;
	char	*sep;
	char	*key;
	int		ret;

	// 引数なしだったら環境変数一覧表示
	if (!args[1])
	{
		print_exported_env(*env_list);
		return (0);
	}

	ret = 0;
	// 複数の引数に対応
	i = 1;
	while (args[i])
	{
		// 値があるか
		sep = ft_strchr(args[i], '=');

		// 値あり登録
		if (sep)
		{
			key = ft_substr(args[i], 0, sep - args[i]);
			if (!is_valid_identifier(key))
			{
				ft_putstr_fd("minishell: export: `", STDERR_FILENO);
				ft_putstr_fd(args[i], STDERR_FILENO);
				ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
				free(key);
				ret = 1;
				i++;
				continue ;
			}
			set_env_value(env_list, key, sep + 1);
			free(key);
		}

		// 値なし登録
		else
		{
			if (!is_valid_identifier(args[i]))
			{
				ft_putstr_fd("minishell: export: `", STDERR_FILENO);
				ft_putstr_fd(args[i], STDERR_FILENO);
				ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
				ret = 1;
				i++;
				continue ;
			}
			set_env_value(env_list, args[i], NULL);
		}

		i++;
	}
	return (ret);
}

//　引数なしを処理
static void	print_exported_env(t_env *env_list)
{
	int		count;
	int		i;
	char	**keys;
	t_env	*node;

	// t_envnの数をカウント
	count = count_env_nodes(env_list);

	// 環境変数分の文字列配列を確保
	keys = (char **)malloc(sizeof(char *) * (count + 1));
	if (!keys)
		return ;

	// 全ての環境変数をメモリにコピー
	i = 0;
	node = env_list;
	while (node)
	{
		keys[i++] = node->key;
		node = node->next;
	}

	// 環境変数をsort
	sort_str_array(keys, count);

	// sortされたkey順に元のリストから該当nodeを探して表示
	i = 0;
	while (i < count)
	{
		node = env_list;

		// sort済みkeyとnode->keyが一致するまでnextへ
		while (node && ft_strcmp(node->key, keys[i]) != 0)
			node = node->next;

		// 1つの環境変数をprint (内部変数 ? はスキップ)
		if (node && ft_strcmp(node->key, "?") != 0)
			print_single_export(node);
		i++;
	}
	free(keys);
}

// 値の中の ", \, $, ` をエスケープして出力
static void	print_escaped_value(char *value)
{
	while (*value)
	{
		if (*value == '"' || *value == '\\' || *value == '$' || *value == '`')
			ft_putchar_fd('\\', STDOUT_FILENO);
		ft_putchar_fd(*value, STDOUT_FILENO);
		value++;
	}
}

// 1つの環境変数ノードを表示
static void	print_single_export(t_env *node)
{
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd(node->key, STDOUT_FILENO);
	if (node->value)
	{
		ft_putstr_fd("=\"", STDOUT_FILENO);
		print_escaped_value(node->value);
		ft_putchar_fd('\"', STDOUT_FILENO);
	}
	ft_putchar_fd('\n', STDOUT_FILENO);
}

// 有効な識別子か確認 (先頭: 英字or_, 以降: 英数字or_)
static int	is_valid_identifier(char *name)
{
	int	i;

	if (!name || !name[0])
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}
