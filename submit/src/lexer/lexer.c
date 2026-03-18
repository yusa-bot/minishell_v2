/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:43 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 15:16:29 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

t_token *tokenize(char *line);
static int	classify_operator(char *s, t_token_type *type, int *len);
static t_token *consume_operator(char **line_ptr);
static t_token *consume_word(char **line_ptr);
void	free_tokens(t_token *tokens);


// tokenize ----------------------------------------------

// skip_spaces -> consume_operator || consume_word -> token_add_back
t_token *tokenize(char *line)
{
	t_token	*head;
	t_token	*new_node;

	head = NULL;
	while (line)
	{
		skip_spaces(&line);
		if (*line == '\0')
			break ;
		if (is_operator(*line))
			new_node = consume_operator(&line);
		else
			new_node = consume_word(&line);
		if (new_node == NULL)
		{
			free_tokens(head);
			return (NULL);
		}
		token_add_back(&head, new_node);
	}
	return (head);
}

static int	classify_operator(char *s, t_token_type *type, int *len)
{
	if (s[0] == '<' && s[1] == '<')
		{ *type = TK_HEREDOC; *len = 2; }
	else if (s[0] == '<' && s[1] == '>')
		{ *type = TK_REDIR_RDWR; *len = 2; }
	else if (s[0] == '<')
		{ *type = TK_REDIR_IN; *len = 1; }
	else if (s[0] == '>' && s[1] == '>')
		{ *type = TK_REDIR_APPEND; *len = 2; }
	else if (s[0] == '>')
		{ *type = TK_REDIR_OUT; *len = 1; }
	else if (s[0] == '|' && s[1] == '|')
		{ *type = TK_OR; *len = 2; }
	else if (s[0] == '|')
		{ *type = TK_PIPE; *len = 1; }
	else if (s[0] == '&' && s[1] == '&')
		{ *type = TK_AND; *len = 2; }
	else if (s[0] == '&')
		return (-1);
	else if (s[0] == '(')
		{ *type = TK_LPAREN; *len = 1; }
	else
		{ *type = TK_RPAREN; *len = 1; }
	return (0);
}

// |, <, >, &, (, ) -> &&, ||, <<, >>
static t_token	*consume_operator(char **line_ptr)
{
	t_token_type	type;
	int				len;

	len = 1;
	if (classify_operator(*line_ptr, &type, &len) == -1)
	{
		ft_putendl_fd(
			"minishell: syntax error near unexpected token `&'", 2);
		return (NULL);
	}
	*line_ptr += len;
	return (new_token(type, ft_substr(*line_ptr - len, 0, len)));
}

// nomal word
static t_token *consume_word(char **line_ptr)
{
	char	*start;
	char	*value;
	int		in_squote;
	int		in_dquote;

	start = *line_ptr;
	in_squote = 0;
	in_dquote = 0;
	while (**line_ptr != '\0')
	{
		if (!in_squote && !in_dquote && is_spaces_operator(**line_ptr))
			break ;
		if (**line_ptr == '\'' && !in_dquote)
			in_squote = !in_squote;
		else if (**line_ptr == '"' && !in_squote)
			in_dquote = !in_dquote;
		(*line_ptr)++;
	}
	if (in_squote || in_dquote)
	{
		ft_putendl_fd("minishell: unclosed quote error", 2);
		return (NULL);
	}
	value = ft_substr(start, 0, *line_ptr - start);
	return (new_token(TK_WORD, value));
}

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		if (tokens->value)
		{
			free(tokens->value);
			tokens->value = NULL;
		}
		free(tokens);
		tokens = tmp;
	}
}