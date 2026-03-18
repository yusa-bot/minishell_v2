/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:36:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 17:02:33 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// ASTの新しいノードを作成し、初期化
t_node *new_node(t_node_type type, t_node *left, t_node *right);
// tokenがリダイレクト演算子かどうかを判定
int  is_redirect(t_token_type type);
//　char **args をメモリ再確保して追加
void append_arg(t_node *node, char *value);


// ASTの新しいノードを作成し、初期化
// node->args, node->redirectsは、葉(NODE_CMD)以外は空
t_node *new_node(t_node_type type, t_node *left, t_node *right)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	if (!node)
		return (NULL);

	node->type = type;
	node->left = left;
	node->right = right;

	node->args = NULL;       // NODE_CMD用初期化
	node->redirects = NULL;  // NODE_CMD用初期化

	return (node);
}

// tokenがリダイレクト演算子かどうかを判定
int  is_redirect(t_token_type type)
{
	if (type == TK_REDIR_IN || type == TK_REDIR_OUT ||
		type == TK_REDIR_APPEND || type == TK_HEREDOC ||
		type == TK_REDIR_RDWR)
		return (1);
	return (0);
}

//　char **args をメモリ再確保して追加
void append_arg(t_node *node, char *value)
{
	int		i;
	char	**new_args;

	i = 0;

	// t_nodeに既にあるargsの数をカウント
	//		parse_commandではtokenごとにparseするため、既にargs tokenがある場合
	if (node->args)
	{
		while (node->args[i])
			i++;
	}

	// 追加するargsも含めて再度メモリを確保
	new_args = (char **)malloc(sizeof(char *) * (i + 2));
	if (!new_args)
		return ;

	// 既存のポインタを新しい配列にコピーし、古い配列を解放
	i = 0;
	if (node->args)
	{
		while (node->args[i])
		{
			new_args[i] = node->args[i];
			i++;
		}
		free(node->args);
	}

	// Lexer側のtokenと分離するため、複製して追加
	new_args[i] = ft_strdup(value);
	new_args[i + 1] = NULL;
	node->args = new_args;
}
