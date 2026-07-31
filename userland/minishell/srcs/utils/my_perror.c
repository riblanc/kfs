/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_perror.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:04:25 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:04:26 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/* strerror was reached through a chain of glibc headers that mlibc does not
 * have, and an implicit declaration would return int. */
#include <string.h>

t_status	my_perror(t_env *env, char *error[2], bool use_errno, t_status ret)
{
	int	errno_save;

	(void)env;
	errno_save = errno;
	ft_putstr_fd(NAME ": ", 2);
	ft_putstr_fd(error[0], 2);
	if (error[1])
		ft_putstr_fd(error[1], 2);
	if (use_errno)
	{
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno_save), 2);
	}
	write(2, "\n", 1);
	errno = errno_save;
	return (ret);
}
