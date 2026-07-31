/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_find.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:40:51 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:40:52 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	path_find2(char **array, char *name, char **dst)
{
	t_status	ret;

	name = ft_strjoin("/", name);
	ret = KO;
	if (!name)
		return (FATAL);
	while (*array)
	{
		*dst = ft_strjoin(*array, name);
		if (!*dst)
		{
			ret = FATAL;
			break ;
		}
		if (!access(*dst, F_OK) && !is_dir(*dst))
		{
			ret = OK;
			break ;
		}
		free(*dst);
		++array;
	}
	free(name);
	return (ret);
}

t_status	path_find(char *name, t_env *env, char **dst)
{
	char	**directories;
	char	*path;
	int		ret;

	path = get_var_val(env, "PATH");
	*dst = NULL;
	if (!path)
		return (KO);
	directories = ft_split(path, ':');
	if (!directories)
		return (FATAL);
	ret = path_find2(directories, name, dst);
	ft_free_split(directories);
	if (ret != OK)
		*dst = NULL;
	return (ret);
}
