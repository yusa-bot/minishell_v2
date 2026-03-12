/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 21:28:34 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/12 21:29:02 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 展開系util
// str: args[i], redir->filename を expand_stringに渡す
void	expand_node(t_node *node, t_env *env_list);
// 	str: args[i] か redir->filename の $, クォート を適切に展開する
char *expand_string(char *str, t_env *env_list);
//　$と、それに続く文字(2個目の$まで) を返す
//		* 環境変数は連続出力にも対応する, valueが無い時は空文字扱い
static char	*handle_dollar(char *res, char *str, int *i, t_env *env_list);
// 文字列の末尾に1文字を追加し、元の文字列を解放
static char	*append_char(char *str, char c);


// 展開系util ----------------------------------------------
// str: args[i], redir->filename を expand_stringに渡す
void	expand_node(t_node *node, t_env *env_list)
{
	int			i;
	char		*expanded_str;
	t_redirect	*redir;

	if (!node)
		return ;

	// argsを展開する
	i = 0;
	if (node->args)
	{
		while (node->args[i])
		{
			// 展開
			expanded_str = expand_string(node->args[i], env_list);
			free(node->args[i]);

			// 展開後の文字列で上書き
			node->args[i] = expanded_str;

			i++;
		}
	}

	// リダイレクトのファイル名の展開
	redir = node->redirects;
	while (redir)
	{
		if (redir->type != TK_HEREDOC) // heredoc以外
		{
			// 展開
			expanded_str = expand_string(redir->filename, env_list);
			free(redir->filename);

			// 展開後の文字列で上書き
			redir->filename = expanded_str;
		}
		redir = redir->next;
	}
}

// 	str: args[i] か redir->filename の $, クォート を適切に展開する
char *expand_string(char *str, t_env *env_list);
{
	char	*res;
	int		i;
	int		in_squote;
	int		in_dquote;

	// 空文字列からスタート
	res = ft_strdup("");

	in_squote = 0;
	in_dquote = 0;
	i = 0;

	while (str[i] != '\0')
	{
		// ' & クオート外 なら
		if (str[i] == '\'' && !in_dquote)
			in_squote = !in_squote; // 反転
		// " & クオート外 なら
		else if (str[i] == '\"' && !in_squote)
			in_dquote = !in_dquote; // 反転

		// $ & '' の中ではない場合、展開
		else if (str[i] == '$' && !in_squote)
		{
			res = handle_dollar(res, str, &i, env_list);
			continue ; // '$HOME$?'の時など (iは加算済)
		}

		// 通常の文字　"'$HOME'" の ' も文字
		else
			res = append_char(res, str[i]);

		i++;
	}
	return (res);
}

//　$と、それに続く文字(2個目の$まで) を返す
//		* 環境変数は連続出力にも対応する, valueが無い時は空文字扱い
static char	*handle_dollar(char *res, char *str, int *i, t_env *env_list)
{
	int		start;
	char	*var_name;
	char	*val;
	char	*new_res;

	// '$' 自体をスキップ
	(*i)++;

	// ?
	if (str[*i] == '?')
	{
		val = get_env_value(env_list, "?");
		(*i)++;
	}

	// 通常の環境変数 (英数字とアンダースコア)
	else if (ft_isalnum(str[*i]) || str[*i] == '_')
	{
		start = *i;

		// 英数字とアンダースコア
		while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
			(*i)++;

		// keyを取得
		var_name = ft_substr(str, start, *i - start);
		// valueを取得
		val = get_env_value(env_list, var_name);

		free(var_name);
	}

	// 単独の '$'
	else
		return (append_char(res, '$'));

	// 取得したvalueを結果の文字列に結合する
	if (val)
	{
		new_res = ft_strjoin(res, val);
		free(res);
		return (new_res);
	}

	// 変数が存在しない場合、何も足さず空文字として扱う
	return (res);
}

// 文字列の末尾に1文字を追加し、元の文字列を解放
static char	*append_char(char *str, char c)
{
	char	*new_str;
	int		len;
	int		i;

	if (!str)
		return (NULL);

	len = ft_strlen(str);

	// 新しいメモリ
	new_str = (char *)malloc(sizeof(char) * (len + 2));
	if (!new_str)
		return (NULL);

	// 新しいメモリに、元+c+'\0'
	i = 0;
	while (i < len)
	{
		new_str[i] = str[i];
		i++;
	}
	new_str[i] = c;
	new_str[i + 1] = '\0';

	free(str);
	return (new_str);
}
