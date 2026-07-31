/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/27 17:06:15 by ereali            #+#    #+#             */
/*   Updated: 2019/12/21 20:37:00 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	gnl_ft_check_str(char **str, char **line)
{
	char	*tmp;
	int		k;

	k = gnl_ft_strchr(*str, '\n');
	if (k >= 0)
	{
		(*line) = gnl_ft_swap(0, k, *str);
		if (!*line)
			return (-1);
		tmp = (*str);
		(*str) = gnl_ft_swap(k + 1, -1, *str);
		free(tmp);
		if (!*str)
		{
			free(*line);
			*line = NULL;
			return (-1);
		}
		return (1);
	}
	return (0);
}

char	*gnl_ft_use_read(char *str, char *str1, int fd, int *i)
{
	gnl_ft_bzero(str1, BUFFER_SIZE + 1);
	(*i) = (int)(read(fd, str1, BUFFER_SIZE));
	while (*i > 0)
	{
		str = gnl_ft_strjoin(str, str1);
		if (gnl_ft_strchr(str1, '\n') >= 0)
			break ;
		gnl_ft_bzero(str1, BUFFER_SIZE + 1);
		(*i) = (int)(read(fd, str1, BUFFER_SIZE));
	}
	free(str1);
	return (str);
}

int	gnl_ft_line(char **line, char **str, int i)
{
	int		k;
	char	*str1;

	k = gnl_ft_strchr((*str), '\n');
	if (k >= 0)
	{
		(*line) = gnl_ft_swap(0, k, (*str));
		str1 = (*str);
		(*str) = gnl_ft_swap(k + 1, -1, (*str));
		free(str1);
		return (1);
	}
	k = gnl_ft_strchr((*str), '\0');
	if (i == 0)
	{
		if (k > 0)
			(*line) = gnl_ft_swap(0, k, (*str));
		else
			(*line) = NULL;
		free(*str);
		(*str) = NULL;
		return (0);
	}
	return (1);
}

int	gnl_free(char **str)
{
	free(*str);
	*str = NULL;
	return (-1);
}

int	get_next_line(int fd, char **line)
{
	static char	*str;
	char		*str1;
	int			i;
	int			n;

	i = 0;
	if (!BUFFER_SIZE || fd < 0 || !line)
		return (gnl_free(&str));
	n = gnl_ft_check_str(&str, line);
	if (n != 0)
		return (n);
	str1 = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!str1)
		return (gnl_free(&str));
	str = gnl_ft_use_read(str, str1, fd, &i);
	if (i == 0 && !str)
	{
		return (0);
	}
	if (i == -1)
		return (gnl_free(&str));
	return (gnl_ft_line(line, &str, i));
}
