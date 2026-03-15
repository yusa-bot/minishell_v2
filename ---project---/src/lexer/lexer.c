/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:43 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// tokenize
// 1prompt
t_token *tokenize(char *line);
// |, <, >, &, (, ) -> &&, ||, <<, >>
static t_token *consume_operator(char **line_ptr);
// 演算子でも空白でもない連続した文字列 -> TK_WORD
static t_token *consume_word(char **line_ptr);

// free tokens
// トークンの連結リストを全て解放する
void	free_tokens(t_token *tokens)


// tokenize ----------------------------------------------
// consume_operator(),consume_word()で**line_ptrを進めることで、
//	tokenizeで管理不要に(tokenize()に返ってきたらptrは進んでいる)

// 1 prompt
t_token *tokenize(char *line)
{
	t_token	*head;
	t_token	*new_node; //1token

	head = NULL;

	while (line)
	{
		//空白があれば読み飛ばす
		skip_spaces(&line);
		if (*line == '\0')
			break ;

		// tokenize
		if (is_operator(*line)) //対象文字が演算子（|, <, >, &, (, )）
			new_node = consume_operator(&line);
		else // 文字列
			new_node = consume_word(&line);

		// 切り出し失敗（クォートが閉じていない等）の処理
		if (new_node == NULL)
		{
			free_tokens(head);
			return (NULL);
		}

		// 切り出したトークンをリストの末尾に繋ぐ
		token_add_back(&head, new_node);
	}
	return (head);

	//空白があれば読み飛ばす。
	//対象文字が演算子（|, <, >, &, (, )）であれば consume_operator を呼ぶ。
	//それ以外であれば consume_word を呼ぶ。
	//切り出したトークンをリストの末尾に繋ぐ。
}

// 文字が |, <, >, &, (, ) -> &&, ||, <<, >>
static t_token *consume_operator(char **line_ptr)
{
	t_token_type	type;
	char			*value;
	int				len;
	char			*s;

	// 先頭を保持
	s = *line_ptr;

	len = 1;

	if (s[0] == '<' && s[1] == '<')
		{ type = TK_HEREDOC; len = 2; }
	else if (s[0] == '<')
		{ type = TK_REDIR_IN; len = 1; }

	else if (s[0] == '>' && s[1] == '>')
		{ type = TK_REDIR_APPEND; len = 2; }
	else if (s[0] == '>')
		{ type = TK_REDIR_OUT; len = 1; }

	else if (s[0] == '|' && s[1] == '|')
		{ type = TK_OR; len = 2; }
	else if (s[0] == '|')
		{ type = TK_PIPE; len = 1; }

	else if (s[0] == '&' && s[1] == '&')
		{ type = TK_AND; len = 2; }
	else if (s[0] == '&')
	{
		// エラー
		ft_putendl_fd("minishell: syntax error near unexpected token `&'", 2);
		return (NULL);
	}

	else if (s[0] == '(')
		{ type = TK_LPAREN; len = 1; }
	else if (s[0] == ')')
		{ type = TK_RPAREN; len = 1; }

	// ヒープ領域にする
	value = ft_substr(s, 0, len);

	*line_ptr += len;

	return (new_token(type, value));

	//「次の1文字」を先読みするロジックが必要
	//現在の文字が < の場合、次が < なら TK_HEREDOC として2文字進め、
	// 違えば TK_REDIR_IN として1文字進める
	//現在の文字が & の場合、次が & なら TK_AND。次が & でなければ、
	// 未定義の構文としてエラーまたは単なる文字として処理する
}

// 演算子でも空白でもない連続した文字列 -> TK_WORD
static t_token *consume_word(char **line_ptr)
{
	char	*start;
	char	*value;
	int		in_squote; // ''
	int		in_dquote; // ""

	start = *line_ptr;
	in_squote = 0;
	in_dquote = 0;

	while (**line_ptr != '\0')
	{
		// quoteの外にdelimiterがあったら、単語修了
		if (!in_squote && !in_dquote && is_spaces_operator(**line_ptr))
			break ;

		if (**line_ptr == '\'' && !in_dquote)
			n_squote = !in_squote; // ''のなかに入る
		else if (**line_ptr == '"' && !in_squote)
			in_dquote = !in_dquote; // ""のなかに入る

		(*line_ptr)++;
	}

	// ループを抜けた段階でクォートが閉じられていなければエラー
	if (in_squote || in_dquote)
	{
		ft_putendl_fd("minishell: unclosed quote error", 2);
		return (NULL);
	}

	// 始点から現在地までの部分文字列を取得
	value = ft_substr(start, 0, *line_ptr - start);

	return (new_token(TK_WORD, value));

	//クォートの処理: ' または " が出現した場合、対応する閉じクォートが見つかるまで、
	// その中にある空白や演算子は「すべて単なる文字列の一部」として扱い、切り出しを続行
	// クォートの除去はこの段階ではしない
}

// free tokens ----------------------------------------------
// トークンの連結リストを全て解放する
void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		// 1. 次のノードのアドレスを安全な場所に退避
		tmp = tokens->next;

		// 2. トークンが保持している文字列（動的確保されている場合）を解放
		// ※ 構造体のメンバ名が 'str', 'word', 'value' など実装によって異なるため、
		// ご自身の minishell.h の定義に合わせて変更してほしい
		if (tokens->value)
		{
			free(tokens->value);
			tokens->value = NULL;
		}

		// 3. トークンのノード自体を解放
		free(tokens);

		// 4. 退避しておいた次のノードにポインタを進める
		tokens = tmp;
	}
}