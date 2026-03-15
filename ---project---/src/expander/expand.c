/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 21:28:34 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// 展開系util
// str: args[i], redir->filename を expand_stringに渡す
void	expand_node(t_node *node, t_env *env_list);
// 	str: args[i] か redir->filename の $, クォート を適切に展開する
static char *expand_string(char *str, t_env *env_list, int *has_wildcard);
//　$と、それに続く文字(2個目の$まで) を返す
//		* 環境変数は連続出力にも対応する, valueが無い時は空文字扱い
static char	*handle_dollar(char *res, char *str, int *i, t_env *env_list, int in_dquote, int *has_wildcard);
// 文字列の末尾に1文字を追加し、元の文字列を解放
static char	*append_char(char *str, char c);

// 展開系util ----------------------------------------------
// str: args[i], redir->filename を expand_stringに渡す
void	expand_node(t_node *node, t_env *env_list)
{
	int			i;
	int has_wildcard;
	char		*expanded_str;
	t_redirect	*redir;
	char	**sorted_matches;
	int		match_count = 0;

	has_wildcard = 0;

	if (!node)
		return ;

	// argsを展開する
	i = 0;
	if (node->args)
	{
		while (node->args[i])
		{
			// 展開 ex.)node->args[i]: *.c
			expanded_str = expand_string(node->args[i], env_list, &has_wildcard);

			// wildcard.c の処理へ委譲し、引数配列を再構築する -------------------
			if (has_wildcard == 1)
			{
				sorted_matches = expand_wildcard(expanded_str); // expanded_str: *.c
				// sorted_matches: [init.c ,util.c, main.c]

				// 挿入する要素数をカウント
				while (sorted_matches && sorted_matches[match_count])
					match_count++;

				// 古い文字列を解放
				free(node->args[i]);
				free(expanded_str); // expanded_str: *.c

				// 配列全体を再構築して、node->args 自体を上書き
				node->args = insert_matches_to_args(node->args, i, sorted_matches, match_count);

				// 追加した要素の分だけ i を進める (ループの最後で i++ されるため -1)
				if (match_count > 0)
					i += (match_count - 1);
			}

			// wildcardではない場合、上書き -------------------
			else
			{
				free(node->args[i]);
				node->args[i] = expanded_str;
			}

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
			expanded_str = expand_string(redir->filename, env_list, &has_wildcard);

			// wildcard
			if (has_wildcard == 1)
			{
				sorted_matches = expand_wildcard(expanded_str);

				// 要素数をカウント
				match_count = 0;
				while (sorted_matches && sorted_matches[match_count])
					match_count++;

				// 1個にマッチした or 0個マッチで*を含むそのままの文字列が返ってきた
				if (match_count == 1)
				{
					// 上書き
					free(redir->filename);
					redir->filename = ft_strdup(sorted_matches[0]);

					free_str_array(sorted_matches);
					free(expanded_str);
				}
				else // match_count > 1
				{
					// エラー出力
                    ft_putstr_fd("minishell: ", 2);
                    ft_putstr_fd(redir->filename, 2); // 展開前(元の文字列)を出すのがBash準拠
                    ft_putstr_fd(": ambiguous redirect\n", 2);

                    free_str_array(sorted_matches);
                    free(expanded_str);
                    return ;
				}
			}
			// wildcardではない場合、上書き -------------------
			else
			{
				free(redir->filename);
				redir->filename = expanded_str;
			}

		}
		redir = redir->next;
	}
}

// 	str: args[i] か redir->filename の $, クォート を適切に展開する
static char *expand_string(char *str, t_env *env_list, int *has_wildcard)
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

	if (has_wildcard)
        *has_wildcard = 0;

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
			res = handle_dollar(res, str, &i, env_list, in_dquote, has_wildcard);
			continue ; // '$HOME$?'の時など (iは加算済)
		}

		// *
		else if (str[i] == '*' && !in_squote && !in_dquote)
		{
			if (has_wildcard)
                *has_wildcard = 1;
			res = append_char(res, str[i]);
		}

		// 通常の文字　"'$HOME'" の ' も文字
		else
			res = append_char(res, str[i]);

		i++;
	}
	return (res);
}

//　$と、それに続く文字(2個目の$まで) を返す
//		環境変数は連続出力にも対応する, valueが無い時は空文字扱い
static char *handle_dollar(char *res, char *str, int *i, t_env *env_list, int in_dquote, int *has_wildcard)
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
		// wildcard: !in_dquote & 展開された値に '*' が含まれていれば フラグを立てる
        if (!in_dquote && has_wildcard && ft_strchr(val, '*'))
            *has_wildcard = 1;

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
