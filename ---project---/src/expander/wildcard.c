/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 09:40:42 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//コマンドラインの引数に * が含まれているか判定する
//	'*' や "*" のようにクォートされている場合は除外
//* が有効な場合、expand_wildcard()
//opendir(".") と readdir() を使用して、現在のディレクトリ内のファイル一覧を取得
//隠しファイル（. から始まるファイル名）は、明示的に .* と指定されない限り除外
//入力されたパターン（例: *.c）と、取得したファイル名が一致するかを判定
//マッチしたファイル名群をアルファベット順にソートし、元の引数（*）と置き換える形で配列を再構築
//マッチするファイルが1つも存在しなかった場合は、* を展開せず、そのままの文字列として引数に残す

// wildcard展開
// dir内の全ファイル取得 -> 一致判定 -> sort -> sorted_matches配列を返す
char	**expand_wildcard(char *pattern); // pattern:*.c
// パターンとファイル名が一致するか判定
// 		*があったら、patternとstrが不一致の間、match(*に対応するstr部分)に記録し、
//		patternとstrが一致し、双方が\0で終わったら、一致していると言える。
static int	match_pattern(char *pattern, char *str); // pattern:*.c str:d_name
// 見つかったファイル名を配列に追加して拡張
static char	**append_match(char **matches, char *new_str, int *count);
// 文字列配列を辞書順(ASCII順)にバブルソートする
static void	sort_str_array(char **array, int count);
// 配列全体を再構築して、node->args 自体を上書き
// 		args 配列の index 番目の要素を削除し、そこに matches 配列の要素を全て挿入
char	**insert_matches_to_args(char **args, int index, char **matches, int match_count)


// dir内の全ファイル取得 -> 一致判定 -> sort -> sorted_matches配列を返す
char	**expand_wildcard(char *pattern) // pattern:*.c
{
	DIR				*dir;
	struct dirent	*dp;
	char			**matches;
	int				count;

	matches = NULL;
	count = 0;

	dir = opendir(".");
	if (!dir)
		return (NULL);

	// ディレクトリ内の全エントリを1つずつ読み込む
	while ((dp = readdir(dir)) != NULL)
	{
		// パターンが '.' で始まっていない限り、隠しファイルを除外
		if (dp->d_name[0] == '.' && pattern[0] != '.')
			continue ;

		// パターンとファイル名が一致するか判定
		if (match_pattern(pattern, dp->d_name))
			matches = append_match(matches, dp->d_name, &count);
	}

	closedir(dir);

	// 1つもマッチしなかった場合は、元のパターンをそのまま返す
	if (count == 0)
		matches = append_match(matches, pattern, &count);

	// sort
	else
		sort_str_array(matches, count);

	return (matches);

	//opendir(".") でカレントディレクトリを開く
	//readdir() で全ファイルを取得し、"*.c" というパターンに一致するか判定
	//一致したファイル名をリスト化して返す。例：["main.c", "parser.c"]
}

// パターンとファイル名が一致するか判定
// 		*があったら、patternとstrが不一致の間、match(*に対応するstr部分)に記録し、
//		patternとstrが一致し、双方が\0で終わったら、一致していると言える。
static int	match_pattern(char *pattern, char *str) // pattern:*.c str:d_name
{
	char	*star_idx;
	char	*match;

	star_idx = NULL;
	match = NULL;

	while (*s)
	{
		// 文字が完全に一致する場合、両方のポインタを進める
		if (*pattern == *str)
		{
			pattern++;
			str++;
		}

		// パターン側が '*' の場合、位置を記憶してパターン側だけ進める（一旦0文字マッチと仮定）
		else if (*pattern == '*')
		{
			star_idx = pattern;
			match = str;
			pattern++;
		}

		// 文字が一致せず、かつ過去に '*' を通過していた場合
		else if (star_idx != NULL)
		{
			// パターンを '*' の次の文字に戻し、文字列側を1文字進めて再試行する（バックトラッキング）
			pattern = star_idx + 1;
			match++;
			str = match;
		}

		else
			return (0);
	}

	// 文字列の末尾に到達後、パターン側に '*' が残っていれば全て消費する
	while (*pattern == '*')
		pattern++;

	// パターンも末尾('\0')に到達していれば完全一致
	if (*pattern == '\0')
    	return (1); // 真（一致）
	else
		return (0); // 偽（不一致）
}

// 見つかったファイル名を配列に追加して拡張
static char	**append_match(char **matches, char *new_str, int *count)
{
	char	**new_array;
	int		i;

	// 新しいメモリ
	new_array = (char **)malloc(sizeof(char *) * (*count + 2));
	if (!new_array)
		return (NULL);

	// matchしてるものが既にあったら、新しいメモリにコピー
	i = 0;
	if (matches)
	{
		while (i < *count)
		{
			new_array[i] = matches[i];
			i++;
		}
		free(matches);
	}
	new_array[i] = ft_strdup(new_str);
	new_array[i + 1] = NULL;

	(*count)++;
	return (new_array);
}

// 文字列配列を辞書順(ASCII順)にバブルソートする
static void	sort_str_array(char **array, int count)
{
	int		i;
	int		j;
	char	*tmp;

	if (!array || count < 2)
		return ;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			// ft_strcmp が正の値を返した場合、順序が逆なので入れ替える
			if (ft_strcmp(array[j], array[j + 1]) > 0)
			{
				tmp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = tmp;
			}

			j++;
		}
		i++;
	}
}

// 配列全体を再構築して、node->args 自体を上書き
// 		args 配列の index 番目の要素を削除し、そこに matches 配列の要素を全て挿入
char	**insert_matches_to_args(char **args, int index, char **matches, int match_count)
{
	int		old_len = 0;
	int		i = 0, j = 0, k = 0;
	char	**new_args;

	if (!args || !matches)
		return (args);

	// 元のargsの要素数
	while (args[old_len])
		old_len++;

	// 新しいargsの配列
	new_args = (char **)malloc(sizeof(char *) * (old_len + match_count));
	if (!new_args)
		return (NULL);

	// indexより前(元のargs)をコピー
	while (i < index)
		new_args[k++] = args[i++];

	// matchesの要素をすべて挿入
	while (j < match_count)
		new_args[k++] = matches[j++];

	// indexより後(元のargs)をコピー
	i++; // index番目の元の文字列は飛ばすためi++
	while (i < old_len)
		new_args[k++] = args[i++];
	new_args[k] = NULL;

	free(args);
	free(matches);
	return (new_args);
}