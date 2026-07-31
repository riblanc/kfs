/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 04:00:47 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 18:55:37 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned char	sh_cd(char **argv, t_env *env)
{
	char	*pathname;

	argv++;
	if (!(*argv))
	{
		pathname = get_var_val(env, "HOME");
		if (!pathname)
			return (1);
		pathname = ft_strdup(pathname);
		if (!pathname)
			return (128);
	}
	else
		pathname = ft_strdup(*argv);
	if (!pathname)
		return (128);
	if (chdir(pathname) < 0)
		return (my_perror(env, (char *[2]){"cd: can't cd to ", pathname},
			true, KO), free(pathname), 1);
	if (set_var(env, "PWD", pathname, 1))
		return (128);
	return (free(pathname), 0);
}
// free path name ?
