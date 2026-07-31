/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_match.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:40:57 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:54:20 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	glob_include(char *glob, char *str);

static char	manage_glob(char **glob, char **str, bool escaped)
{
	if (**glob == '*' && !escaped)
	{
		while (**glob == '*')
			++*glob;
		if (!**glob)
			return (true);
		while (**str && **str != '/')
		{
			if ((**glob == **str || **glob == '?') && glob_include(*glob, *str))
				return (true);
			++*str;
		}
		return (false);
	}
	else if (**glob && ((**glob == '?' && **str != '/' && !escaped)
			|| **glob == **str))
	{
		++*str;
		++*glob;
	}
	else
		return (false);
	return (-1);
}

void	foo(char **glob, t_quote *state, bool *escaped)
{
	if ((*escaped))
		(*escaped) = (false);
	else if (*(*glob) == '\'')
		(*state) = ((*state) != ONE) * ONE;
	else if (*(*glob) == '"')
		(*state) = ((*state) != DOUBLE) * DOUBLE;
	else if (*(*glob) == '\\')
	{
		(*escaped) = true;
		++(*glob);
	}
}

bool	glob_include(char *glob, char *str)
{
	t_quote	state;
	bool	escaped;
	int		ret;

	state = NONE;
	escaped = false;
	if (*glob == '.' && *str != '.')
		return (false);
	if (*glob != '.' && *str == '.')
		return (false);
	while (*glob && (*str || *glob == '*'))
	{
		foo(&glob, &state, &escaped);
		if (state == NONE || escaped)
		{
			ret = manage_glob(&glob, &str, escaped);
			if (ret != -1)
				return (ret);
		}
	}
	return (*str == *glob);
}

t_status	path_match_current(char *glob, t_str_vec *dst)
{
	DIR				*current;
	struct dirent	*entry;
	char			*tmp;

	current = opendir(".");
	if (!current)
		return (KO);
	entry = readdir(current);
	while (entry)
	{
		if ((ft_strcmp(entry->d_name, ".") && ft_strcmp(entry->d_name, "..")
				&& glob_include(glob, entry->d_name)))
		{
			tmp = ft_strdup(entry->d_name);
			if (!tmp || !str_vec_push(dst, tmp))
			{
				free(tmp);
				closedir(current);
				return (FATAL);
			}
		}
		entry = readdir(current);
	}
	closedir(current);
	return (OK);
}
