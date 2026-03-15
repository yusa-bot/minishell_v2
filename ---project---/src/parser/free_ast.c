/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:38:37 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// AST 全体のメモリを再帰的に解放する
void	free_ast(t_node *node)
// 文字列配列 (char **) の全要素と枠組みを解放する
static void	free_str_array(char **array)
// リダイレクトの連結リストを解放する
static void	free_redirects(t_redirect *redir)


// AST 全体のメモリを再帰的に解放する
void	free_ast(t_node *node)
{
	if (!node)
		return ;

	// 1. 左右の子ノードを再帰的に先に解放する
	free_ast(node->left);
	free_ast(node->right);

	// 2. ノードが保持しているデータ（引数配列）を解放
	if (node->args)
	{
		free_str_array(node->args);
		node->args = NULL; // 二重解放防止の安全策
	}

	// 3. ノードが保持しているデータ（リダイレクトリスト）を解放
	if (node->redirects)
	{
		free_redirects(node->redirects);
		node->redirects = NULL;
	}

	// 4. 全ての子とデータが解放された後、最後に自分自身を解放
	free(node);
}

// 文字列配列 (char **) の全要素と枠組みを解放する
static void	free_str_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

// リダイレクトの連結リストを解放する
static void	free_redirects(t_redirect *redir)
{
	t_redirect	*tmp;

	while (redir)
	{
		tmp = redir->next;
		// filename は malloc で確保された文字列なので解放が必要
		if (redir->filename)
			free(redir->filename);
		// ノード自体を解放
		free(redir);
		redir = tmp;
	}
}