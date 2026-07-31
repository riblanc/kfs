/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 02:40:48 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 19:01:46 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned char	sh_pwd(char **argv, t_env *env)
{
	char	*buf;

	(void)argv;
	(void)env;
	buf = getcwd(NULL, 0);
	if (buf && ft_strlen(buf) < PATH_MAX)
	{
		ft_putstr_fd(buf, 1);
		ft_putstr_fd("\n", 1);
		free(buf);
		return (OK);
	}
	else
		return (2);
}
