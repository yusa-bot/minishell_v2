/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:43 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/11 23:25:20 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// tokenize
// 1prompt
t_token *tokenize(char *line);
// |, <, >, &, (, ) -> &&, ||, <<, >>
t_token *consume_operator(char **line_ptr);
// 演算子でも空白でもない連続した文字列 -> TK_WORD
t_token *consume_word(char **line_ptr);

// util
// スペースやタブの間ポインタを進める
void skip_spaces(char **line_ptr);
// malloc を使用して新しいトークンノードのメモリを確保し、種類と文字列をセット
t_token *new_token(t_token_type type, char *value);
// 作成したトークンを、トークンリストの末尾に追加します。
void token_add_back(t_token **head, t_token *new_node);
// 対象文字が演算子か
int     is_operator(char c);


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
}

// 文字が |, <, >, &, (, ) -> &&, ||, <<, >>
t_token *consume_operator(char **line_ptr)
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
}

// 演算子でも空白でもない連続した文字列 -> TK_WORD
t_token *consume_word(char **line_ptr)
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
		if (!in_squote && !in_dquote && is_delimiter(**line_ptr))
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
}

// util ----------------------------------------------
// スペースやタブの間ポインタを進める
void skip_spaces(char **line_ptr)
{
	while (**line_ptr == ' ' || **line_ptr == '\t')
		(*line_ptr)++;
}

// malloc を使用して新しいトークンノードのメモリを確保し、種類と文字列をセット
t_token *new_token(t_token_type type, char *value)
{
	t_token	*node;

	node = (t_token *)malloc(sizeof(t_token));
	if (!node)
		return (NULL);

	node->type = type;
	node->value = value;
	node->next = NULL;

	return (node);
}

// 作成したトークンを、トークンリストの末尾に追加します。
void token_add_back(t_token **head, t_token *new_node)
{
	t_token	*current;

	if (!head || !new_node)
		return ;

	// 先頭
	if (*head == NULL)
	{
		*head = new_node;
		return ;
	}

	// 通常の追加
	current = *head;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
}

// 対象文字が演算子か
int     is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>' || c == '&' || c == '(' || c == ')')
		return (1);
	return (0);
}

// 文字が区切り文字（空白または演算子）かどうか
static int	is_delimiter(char c)
{
	if (c == ' ' || c == '\t' || c == '\n')
		return (1);
	if (is_operator(c))
		return (1);
	return (0);
}
