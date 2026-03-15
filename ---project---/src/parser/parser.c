/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:49 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 17:02:08 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// parse
// 構文解析の入り口
t_node *parse(t_token **tokens);
// 最も優先順位の低い && と || を処理
static t_node *parse_list(t_token **tokens);
// 次に優先順位の低い |（パイプ）を処理
static t_node *parse_pipeline(t_token **tokens);
// 最も優先順位が高い「単一のコマンド」または「サブシェル ()」を処理
static t_node *parse_command(t_token **tokens);
// t_redirectを作成してnodeに追加
static void append_redirect(t_node *node, t_token_type type, char *filename);


// parse ----------------------------------------------

// 構文解析の入り口
t_node *parse(t_token **tokens)
{
	t_node	*root;

	if (!tokens || !*tokens)
		return (NULL);

	root = parse_list(tokens);
	if (root == NULL)
		return (NULL);

	// トークンが余っていればエラー
	if (*tokens != NULL)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd((*tokens)->value, 2);
		ft_putstr_fd("'\n", 2);

		free_ast(root);
		return (NULL);
	}

	return (root);
}

// 最も優先順位の低い && と || を処理
static t_node *parse_list(t_token **tokens)
{
	t_node *node;
	t_node *right;
	t_node_type type;

	// パイプラインをパースして左辺に
	node = parse_pipeline(tokens);
    if (!node)
        return (NULL);

	// 次のトークンが && か || なら、ツリーを成長させる
	while (*tokens && ((*tokens)->type == TK_AND || (*tokens)->type == TK_OR))
	{
		type = ((*tokens)->type == TK_AND) ? NODE_AND : NODE_OR;
		tokens = (*tokens)->next; //次のtokenに進む

		// パイプラインをパースして右辺に
		right = parse_pipeline(tokens);
		if (!right)
            return (NULL); // 構文エラー (例: "ls &&" で終わっている)

		// 新しい親ノードを作って、これまでのノードを左下にぶら下げる
		node = new_node(type, node, right);
	}
	return (node);


	//parse_pipeline を呼び出し、左側のノード（left）を取得
	//次のトークンが && または || である限りループ

	//&& と || を読み飛ばし、再び parse_pipeline を呼んで右側のノード（right）を取得
	//NODE_AND または NODE_OR を作成し、left と right を繋
	//作成したノードを新たな left としてループを続
}

// 次に優先順位の低い |（パイプ）を処理
static t_node *parse_pipeline(t_token **tokens)
{
	t_node *node;
    t_node *right;

	// parse_commandでパースして左辺に
	node = parse_command(tokens);
    if (!node)
        return (NULL);

	while (*tokens && (*tokens)->type == TK_PIPE)
	{
		*tokens = (*tokens)->next; //次のtokenに進む

		// parse_commandでパースして右辺に
		right = parse_command(tokens);
		if (!right)
            return (NULL);

		// 新しい親ノードを作って、これまでのノードを左下にぶら下げる
		node = new_node(NODE_PIPE, node, right);
	}
	return (node);

	//parse_command を呼び出して左側のノードを取得
	//次のトークンが | なら、それを読み飛ばして右側の parse_command を取得
	//NODE_PIPE で繋
}

// 最も優先順位が高い「単一のコマンド」または「サブシェル ()」を処理
// 		サブシェル内はlistから始まっているが、元promptからすればcmdと同じ扱い
static t_node *parse_command(t_token **tokens)
{
	t_node	*node;
	t_node	*subshell_node;
	t_token_type redir_type;

	if (!tokens || !*tokens)
		return (NULL);

	// サブシェル
	if ((*tokens)->type == TK_LPAREN)
	{
		*tokens = (*tokens)->next; // '(' を進める

		// 再帰的にパース
		node = parse_list(tokens);
		if (!node)
			return (NULL); // 再帰した葉からの結果が無い場合

		// ')'がない場合
		if (!*tokens || (*tokens)->type != TK_RPAREN)
		{
			free_ast(node);
			return (NULL);
		}

		*tokens = (*tokens)->next; // ')' を進める

		// 取得した木を NODE_SUBSHELL でラップして返す
		subshell_node = new_node(NODE_SUBSHELL, node, NULL);
		return (subshell_node);
	}

	// 通常のコマンド (NODE_CMD) の処理 ----------

	// 今までは最後にnodeを作成していたが、
	// 		今回は葉のため、最初に葉を作成し、内容を埋めていく
	node = new_node(NODE_CMD, NULL, NULL);
	if (!node)
		return (NULL);

	// パース段階で args と redirects を明確に分ける
	// 'ls > out -l', '> out ls -l' どちらでも正しくパースできる
	// トークンが単語(TK_WORD)またはリダイレクトである限りループ
	while (*tokens && ((*tokens)->type == TK_WORD || is_redirect((*tokens)->type)))
	{
		// TK_WORD
		if ((*tokens)->type == TK_WORD)
		{
			// t_node->args(コマンドと引数の配列)に追加
			append_arg(node, (*tokens)->value);
			*tokens = (*tokens)->next; // 次に進める
		}
		else if (is_redirect((*tokens)->type)) // リダイレクト
		{
			redir_type = (*tokens)->type;
			*tokens = (*tokens)->next; // リダイレクト記号を進める

			// リダイレクトの次がTK_WORDではなければ
			if (!*tokens || (*tokens)->type != TK_WORD)
			{
				free_ast(node);
				return (NULL);
			}

			// t_node->redirectsに追加
			// redir_type, (*tokens)->value:ファイル名
			append_redirect(node, redir_type, (*tokens)->value);
			tokens = (*tokens)->next; // ファイル名を進める

		}
	}

	// TK_WORDもリダイレクトも無い場合
	if (node->args == NULL && node->redirects == NULL)
	{
		free_ast(node);
		return (NULL);
	}

	return (node);


	//現在のトークンが ( の場合:
	//	( を読み飛ばす
	//	再帰的に parse_list を呼び出す
	//	戻ってきたノードを NODE_SUBSHELL でラップし、) を読み飛ばす
	//それ以外の場合:
	//	NODE_CMD を作成
	//	トークンが TK_WORD（コマンド名や引数）またはリダイレクト（<, >, <<, >>）である限りループ
	//	引数配列（args）やリダイレクトリスト（redirects）にデータを詰め込む
}


// t_redirectを作成してnodeに追加
static void append_redirect(t_node *node, t_token_type type, char *filename)
{
	t_redirect	*new_redir;
	t_redirect	*current;

	new_redir = (t_redirect *)malloc(sizeof(t_redirect));
	if (!new_redir)
		return ;

	new_redir->type = type;
	new_redir->filename = ft_strdup(filename);
	new_redir->next = NULL;

	// t_nodeでt_redirectが初めての場合
	if (node->redirects == NULL)
	{
		node->redirects = new_redir;
			return ;
	}

	// t_nodeにt_redirectが既にある場合 -> リストの末尾を探して追加
	current = node->redirects;
	while (current->next != NULL)
		current = current->next;
	current->next = new_redir;
}

//　リダイレクトが複数ある場合、最後のリダイレクトが適応されるため、
//		t_redirect同士をnextで繋げる
