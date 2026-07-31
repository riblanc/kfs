/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:06:25 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:06:26 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*my_readline(t_env *env, char *prompt)
{
	char	*line;
	int		ret;

	g_int = false;
	if (env->is_interactive)
	{
		set_signal(SIGINT, sigint_handler, env);
		line = readline(get_var_val(env, prompt));
		set_signal(SIGINT, SIG_IGN, env);
	}
	else
	{
		ret = get_next_line(0, &line);
		if (ret <= 0)
			line = NULL;
	}
	if (g_int)
		env->err = 128 + SIGINT;
	return (line);
}

int	count_trailing_backslashes(char *str)
{
	size_t	i;
	int		ret;

	i = ft_strlen(str);
	ret = 0;
	while (i > 0)
	{
		if (str[i - 1] != '\\')
			break ;
		++ret;
		--i;
	}
	return (ret);
}

t_status	readnline(char **line, t_env *env)
{
	char	*tmp;

	*line = my_readline(env, "PS1");
	if (!*line)
		return (FATAL);
	if (!**line || g_int)
		return (free(*line), KO);
	while (count_trailing_backslashes(*line) % 2)
	{
		(*line)[ft_strlen(*line) - 1] = '\0';
		tmp = my_readline(env, "PS2");
		if (!g_int)
			return (free(*line), KO);
		if (!tmp)
			break ;
		if (*tmp)
			*line = ft_strjoinf1(*line, tmp);
		free(tmp);
		if (!*line)
			return (FATAL);
	}
	if (**line)
		add_history(*line);
	return (OK);
}
