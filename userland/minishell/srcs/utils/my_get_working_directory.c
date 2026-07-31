/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_get_working_directory.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:06:25 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:06:26 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	my_get_working_directory(const char *for_whom, char **dst)
{
	char	*ret;
	char	buf[PATH_MAX];

	ret = NULL;
	ret = getcwd(buf, PATH_MAX);
	if (!ret)
	{
		*dst = NULL;
		ft_putstr_fd((char *)for_whom, 2);
		write(2, ": ", 2);
		perror("get_cwd()");
		return (KO);
	}
	*dst = ft_strdup(ret);
	if (!*dst)
		return (FATAL);
	return (OK);
}
