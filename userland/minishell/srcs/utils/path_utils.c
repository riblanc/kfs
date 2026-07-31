/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:41:03 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:41:09 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	path_push(char *path, char *component)
{
	int	len;

	len = ft_strlen(path);
	if (len + ft_strlen(component) + 1 >= PATH_MAX
		|| ft_strlen(component) > NAME_MAX)
	{
		errno = ENAMETOOLONG;
		return (KO);
	}
	path += len;
	if (len > 0 && (path[-1] != '/' || !*component))
	{
		*path = '/';
		++path;
	}
	while (*component)
	{
		*path = *component;
		++path;
		++component;
	}
	*path = 0;
	return (OK);
}

void	path_pop(char *path)
{
	char	*tmp;

	if (!path)
		return ;
	tmp = path + ft_strlen(path);
	while (tmp != path && *tmp != '/')
		--tmp;
	if (*tmp == '/' && (tmp == path || tmp[-1] == '/'))
		tmp[1] = 0;
	else
		*tmp = 0;
}
