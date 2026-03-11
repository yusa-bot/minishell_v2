/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:55 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/11 22:10:56 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


//char **envpは文字の配列で、export や unset によって変数は動的に増減するため、
// 固定長の配列では管理が破綻する -> 連結リストにする


// **envp -> t_envの連結リスト
t_env *init_env(char **envp)
{
	t_env   *env_list;
	char    *key;
    char    *value;
    char    *equal_pos;
    int     i;

	env_list = NULL;
	if (!envp)
        return (NULL);

    i = 0;
	while (envp[i] != NULL)
	{
		// '=' の位置を探す
		equal_pos = ft_strchr(envp[i], '=');

		if (equal_pos != NULL)
		{
			// '=' の前までを key として切り出し
            key = ft_substr(envp[i], 0, equal_pos - envp[i]);

			// '=' の後から終端までを value として切り出し
            value = ft_strdup(equal_pos + 1);
		}
		else // '=' が存在しない場合 (例: "VAR")
		{
            key = ft_strdup(envp[i]);
            value = NULL; // 明示的にNULLを設定し、値を持たないことを表現
		}

		// ノードを作成してリストの末尾に追加
		env_add_back(&env_list, env_new(key, value));
		i++;
	}
	return (env_list);
}


