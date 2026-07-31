/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_name.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:03:39 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:13:31 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

t_builtin	is_special_built_in(char *name)
{
	const char		*list[] = {"exit", "export", "unset", NULL};
	const t_builtin	f[] = {&sh_exit, &sh_export, &sh_unset};
	int				i;

	i = -1;
	while (list[++i])
		if (!ft_strcmp(list[i], name))
			return (f[i]);
	return (NULL);
}

t_builtin	is_built_in(char *name)
{
	const char		*list[] = {"env", "echo", "cd", "pwd", NULL};
	const t_builtin	f[] = {&sh_env, &sh_echo, &sh_cd, &sh_pwd};
	int				i;

	i = -1;
	if (!name)
		return (NULL);
	while (list[++i])
		if (!ft_strcmp(list[i], name))
			return (f[i]);
	return (NULL);
}
