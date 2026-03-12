/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:55 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/12 22:11:06 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


// 実行時の配列変換（Executor）
// リスト構造の環境変数を、再び char ** の配列（KEY=VALUE の形式）に逆変換
char **env_list_to_array(t_env *env_list);
// t_envのノード数をカウントする
static int	count_env_nodes(t_env *env_list);

// 変数の検索と更新（Expander / Builtins 用）
// 指定されたキーを持つ変数を探し、その value のポインタを返す
char *get_env_value(t_env *env_list, char *key)
// 指定されたキーが存在すれば value を上書きし、存在しなければ新規ノードとして追加
void set_env_value(t_env **env_list, char *key, char *value)
// リストの末尾に新しいノードを追加する
void	env_add_back(t_env **env_list, t_env *new_node)
// 指定されたキーを持つノードをリストから切り離して free する
void remove_env_node(t_env **env_list, char *key)



// 実行時の配列変換（Executor）----------------------------------------------
// リスト構造の環境変数を、再び char ** の配列（KEY=VALUE の形式）に逆変換
char **env_list_to_array(t_env *env_list)
{
	char	**envp;
	char	*tmp;
	int		i;

	envp = (char **)malloc(sizeof(char *) * (count_env_nodes(env_list) + 1));
	if (!envp)
		return (NULL);

	i = 0;
	while (env_list)
	{
		// KEY=VALUEの形式に戻す
		if (env_list->value)
		{
			tmp = ft_strjoin(env_list->key, "=");
			envp[i] = ft_strjoin(tmp, env_list->value);
			free(tmp);
		}
		else // valueがNULLの場合、keyのみを複製
			envp[i] = ft_strdup(env_list->key);

		env_list = env_list->next;
		i++;
	}

	envp[i] = NULL;
	return (envp);
}

// t_envのノード数をカウントする
static int	count_env_nodes(t_env *env_list)
{
	int	count;

	count = 0;
	while (env_list)
	{
		count++;
		env_list = env_list->next;
	}
	return (count);
}


// 変数の検索と更新（Expander / Builtins 用）----------------------------------------------
// 指定されたキーを持つ変数を探し、その value のポインタを返す
char *get_env_value(t_env *env_list, char *key)
{
	if (!env_list || !key)
		return (NULL);

	while (env_list)
	{
		// keyと一致
		if (ft_strcmp(env_list->key, key) == 0)
			return (env_list->value);
		env_list = env_list->next;
	}
	return (NULL);
}

// 指定されたキーが存在すれば value を上書きし、存在しなければ新規ノードとして追加
void set_env_value(t_env **env_list, char *key, char *value)
{
	t_env	*current;
	char	*new_value;

	if (!env_list || !key)
		return ;

	current = *env_list;

	// 1. 既存のkeyを探して上書き
	while (current)
	{
		// 上書き
		if (ft_strcmp(current->key, key) == 0)
		{
			if (current->value)
				free(current->value);
			current->value = value ? ft_strdup(value) : NULL;
			return ;
		}

		current = current->next;
	}

	// 2. 既存のkeyが無ければ、新規作成
	new_value = value ? ft_strdup(value) : NULL;
	// 末尾に追加
	env_add_back(env_list, env_new(ft_strdup(key), new_value));
}

// リストの末尾に新しいノードを追加する
void	env_add_back(t_env **env_list, t_env *new_node)
{
	t_env	*current;

	if (!env_list || !new_node)
		return ;

	// リストが空の場合、新しいノードを先頭に
	if (*env_list == NULL)
	{
		*env_list = new_node;
		return ;
	}

	// リストがある場合、末尾に追加
	current = *env_list;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
}

// 指定されたキーを持つノードをリストから切り離して free する
void remove_env_node(t_env **env_list, char *key)
{
	t_env	*current;
	t_env	*prev;

	if (!env_list || !*env_list || !key)
		return ;

	current = *env_list;

	// 1. 先頭ノードが削除対象の場合
	if (ft_strcmp(current->key, key) == 0)
	{
		// 先頭をnextへ。
		*env_list = current->next;

		// keyとvalueをfree -> nodeをfree
		free(current->key);
		if (current->value)
			free(current->value);
		free(current);

		return ;
	}

	// 2. 2番目以降のノードが削除対象の場合
	// nodeを1つ進める
	prev = current;
	current = current->next;

	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			// currentを抜かしてnodeを連結
			prev->next = current->next;

			free(current->key);
			if (current->value)
				free(current->value);
			free(current);

			return ;
		}

		prev = current;
		current = current->next;
	}
}


