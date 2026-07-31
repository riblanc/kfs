/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 00:51:43 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 17:11:00 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	read_heredoc(t_env *env, int fd, bool quote, const char *word)
{
	char	*rl;

	rl = my_readline(env, "PS2");
	while (rl && !g_int && ft_strcmp(word, rl))
	{
		if (quote == false)
			rl = expand_word_all(rl, env);
		if (rl)
			write(fd, rl, ft_strlen(rl));
		else
			return (FATAL);
		write(fd, "\n", 1);
		free(rl);
		rl = my_readline(env, "PS2");
	}
	free(rl);
	return (OK);
}

t_status	ft_heredoc(t_env *env, t_redir *redir)
{
	int		fd;
	bool	quote;
	char	*word;

	quote = false;
	if (ft_strchr(redir->word, '\'') || ft_strchr(redir->word, '\"'))
	{
		quote = true;
		if (remove_quotes(redir->word) == FATAL)
			return (FATAL);
	}
	word = redir->word;
	redir->word = NULL;
	if (my_tmp_file(&fd, &redir->word) == KO)
		return (free(word), my_perror(env, (char *[2]){
				"can't create here-document", NULL}, true, KO));
	if (read_heredoc(env, fd, quote, word) == FATAL)
		return (close(fd), free(word), unlink(redir->word), KO);
	if (close(fd) != 0)
		return (free(word), KO);
	if (g_int)
		return (free(word), unlink(redir->word), KO);
	return (free(word), redir->oldfd = fd, OK);
}

t_status	remove_quotes(char	*str)
{
	char	*cpy;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	cpy = ft_strdup(str);
	if (!cpy)
		return (FATAL);
	while (cpy[i + j])
	{
		if ((ft_strchr("\\\"", cpy[i + j]) && need_to_expand(cpy, i + j) < 2)
			|| (cpy[i + j] == '\'' && (need_to_expand(cpy, i + j) == 2
					|| need_to_expand(cpy, i + j) == 0)))
			str[i] = cpy[i + ++j];
		else
		{
			(str)[i] = cpy[i + j];
			i++;
		}
	}
	(str)[i] = '\0';
	free(cpy);
	return (OK);
}

char	*ft_fill_with_bslash(char *str, char *new, const char *inhibit)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (str[i])
	{
		if (ft_strchr(inhibit, str[i]))
		{
			new[i + j++] = '\\';
			new[i + j] = str[i];
			i++;
		}
		while (!ft_strchr(inhibit, str[i]))
		{
			new[i + j] = str[i];
			i++;
		}
	}
	return (new);
}

char	*ft_inhibit(char *str, const char *inhibit)
{
	char	*new;

	if (!str)
		return (ft_strdup(""));
	if (!inhibit)
		return (str);
	new = ft_calloc(sizeof(char), ft_strlen(str)
			+ ft_countoccur(str, inhibit) + 1);
	if (!new)
		return (NULL);
	new = ft_fill_with_bslash(str, new, inhibit);
	return (new);
}
