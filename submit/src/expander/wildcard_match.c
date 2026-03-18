/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_match.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 16:58:05 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 17:05:05 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	match_step(char **pat, char **str, char **star, char **match);

// Check if the pattern matches the filename
int	match_pattern(char *pattern, char *str)
{
	char	*star_idx;
	char	*match;

	star_idx = NULL;
	match = NULL;
	while (*str)
	{
		if (match_step(&pattern, &str, &star_idx, &match) == -1)
			return (0);
	}
	while (*pattern == '*')
		pattern++;
	if (*pattern == '\0')
		return (1);
	return (0);
}

static int	match_step(char **pat, char **str, char **star, char **match)
{
	if (**pat == **str)
	{
		(*pat)++;
		(*str)++;
	}
	else if (**pat == '*')
	{
		*star = *pat;
		*match = *str;
		(*pat)++;
	}
	else if (*star != NULL)
	{
		*pat = *star + 1;
		(*match)++;
		*str = *match;
	}
	else
		return (-1);
	return (0);
}