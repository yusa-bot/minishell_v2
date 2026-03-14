/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:26 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/14 15:52:46 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//				env       export (引数なし)
//フォーマット   KEY=VALUE    declare -x KEY="VALUE", （VALUE）が必ずダブルクォート（"）
//ソート   	  そのまま  	ASCIIコード順
//値なし変数	   表示なし	  	 表示(declare -x ONLY_KEY)
//_			 あり 		  なし


int	builtin_export(char **args, t_env **env_list)
{
	int		i;
	char	*sep;
	char	*key;

	if (!args[1])
	{
		print_exported_env(*env_list);
		return (0);
	}

	// 複数の引数に対応
	i = 1;
	while (args[i])
	{
		sep = ft_strchr(args[i], '=');

		if (sep)eeeeeeee
		{
			key = ft_substr(args[i], 0, sep - args[i]);
			set_env_value(env_list, key, sep + 1);
			free(key);
		}

		else // 値なし登録
			set_env_value(env_list, args[i], NULL);

		i++;
	}
	return (0);
}

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

	// t_envがある間、メモリにけkeyをコピー
	i = 0;
	node = env_list;
	while (node)
	{
		keys[i++] = node->key;
		node = node->next;
	}

	// sort
	sort_str_array(keys, count);

	i = 0;
	while (i < count)
	{
		node = env_list;

		while (node && ft_strcmp(node->key, keys[i]) != 0)
			node = node->next;
		if (node)
			print_single_export(node);
	}

}

static void	print_single_export(t_env *node)
{

}
