/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:55 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/11 22:31:56 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


// 実行時の配列変換（Executor 用：最重要）
// リスト構造の環境変数を、再び char ** の配列（KEY=VALUE の形式）に逆変換
char **env_list_to_array(t_env *env_list);

// 変数の検索と更新（Expander / Builtins 用）
// 指定されたキーを持つ変数を探し、その value のポインタを返す
char *get_env_value(t_env *env_list, char *key);
// 指定されたキーが存在すれば value を上書きし、存在しなければ新規ノードとして追加
void set_env_value(t_env **env_list, char *key, char *value);
// 指定されたキーが存在すれば value を上書きし、存在しなければ新規ノードとして追加
void set_env_value(t_env **env_list, char *key, char *value);


// 実行時の配列変換（Executor 用：最重要）----------------------------------------------
// リスト構造の環境変数を、再び char ** の配列（KEY=VALUE の形式）に逆変換
char **env_list_to_array(t_env *env_list)
{

}

// 変数の検索と更新（Expander / Builtins 用）----------------------------------------------
// 指定されたキーを持つ変数を探し、その value のポインタを返す
char *get_env_value(t_env *env_list, char *key)
{

}

// 指定されたキーが存在すれば value を上書きし、存在しなければ新規ノードとして追加
void set_env_value(t_env **env_list, char *key, char *value)
{

}

// 指定されたキーを持つノードをリストから切り離して free する
void remove_env_node(t_env **env_list, char *key)
{

}


