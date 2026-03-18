/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 09:40:42 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:04:33 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char	**collect_matches(char *dir_path, char *fpat,
				char **matches, int *count);
static char	**append_match(char **matches, char *prefix,
				char *new_str, int *count);
static char	*make_match_entry(char *prefix, char *new_str);

// Branch based on the presence or absence of / -> sorted_matches -> sort
//		pattern:*.c or src/*.c
char	**expand_wildcard(char *pattern)
{
	char			**matches;
	int				count;
	char			*slash;

	matches = NULL;
	count = 0;
	slash = ft_strrchr(pattern, '/');
	if (slash)
	{
		*slash = '\0';
		matches = collect_matches(pattern, slash + 1, matches, &count);
		*slash = '/';
	}
	else
		matches = collect_matches(NULL, pattern, matches, &count);
	if (count == 0)
		matches = append_match(matches, NULL, pattern, &count);
	else
		sort_str_array(matches, count);
	return (matches);
}

// '/' ex.) dir_path:src, fpat:*.c
static char	**collect_matches(char *dir_path, char *fpat, char **matches, int *count)
{
	DIR				*dir;
	struct dirent	*dp;

	if (dir_path)
		dir = opendir(dir_path);
	else
		dir = opendir(".");
	if (!dir)
		return (matches);
	while ((dp = readdir(dir)) != NULL)
	{
		if (dp->d_name[0] == '.' && fpat[0] != '.')
			continue ;
		if (match_pattern(fpat, dp->d_name))
			matches = append_match(matches, dir_path, dp->d_name, count);
	}
	closedir(dir);
	return (matches);
}

// Add the found filenames to the array and expand ----------------------------------------------
//		prefix: src
static char	**append_match(char **matches, char *prefix,
		char *new_str, int *count)
{
	char	**new_array;
	int		i;

	new_array = (char **)malloc(sizeof(char *) * (*count + 2));
	if (!new_array)
		return (NULL);
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
	new_array[i] = make_match_entry(prefix, new_str);
	if (!new_array[i])
		return (NULL);
	(*count)++;
	new_array[*count] = NULL;
	return (new_array);
}

static char	*make_match_entry(char *prefix, char *new_str)
{
	char	*tmp;
	char	*full;

	if (prefix)
	{
		tmp = ft_strjoin(prefix, "/");
		if (!tmp)
			return (NULL);
		full = ft_strjoin(tmp, new_str);
		free(tmp);
		return (full);
	}
	return (ft_strdup(new_str));
}

// ----------------------------------------------

void	sort_str_array(char **array, int count)
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
