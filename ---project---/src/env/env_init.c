/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:28:32 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/11 22:42:33 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//char **envpは文字の配列で、export や unset によって変数は動的に増減するため、
// 固定長の配列では管理が破綻する -> 連結リストにする

// リストの基本操作
// **envp -> t_envの連結リスト
t_env *init_env(char **envp);
// malloc を使用して新しいノードを作成
t_env *env_new(char *key, char *value);
// リストの末尾にノードを追加
void env_add_back(t_env **env_list, t_env *new_node);
// シェル終了時にリスト全体のメモリ（key, value, ノード自体）を解放
void free_env_list(t_env *env_list);


// リストの基本操作 ----------------------------------------------
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

// malloc を使用して新しいノードを作成
t_env *env_new(char *key, char *value)
{
	t_env	*new_node;

	new_node = (t_env *)malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);

	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;

	return (new_node);
}

// リストの末尾にノードを追加
void env_add_back(t_env **env_list, t_env *new_node)
{
	t_env	*current;

	if (!env_list || !new_node)
		return ;

	// 先頭
	if (*env_list == NULL)
	{
		*env_list = new_node;
		return ;
	}

	// 通常追加
	current = *env_list;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
}

// シェル終了時にリスト全体のメモリ（key, value, ノード自体）を解放
void free_env_list(t_env *env_list)
{
	t_env	*current;
	t_env	*next_node;

	// 現在nodeを保存
	current = env_list;

	while (current != NULL)
	{
		// 次nodeを保存
		next_node = current->next;

		if (current->key != NULL)
			free(current->key);
		if (current->value != NULL)
			free(current->value);

		free(current);

		// free対象をnext_nodeに
		current = next_node;
	}
}
