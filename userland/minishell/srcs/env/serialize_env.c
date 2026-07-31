/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serialize_env.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:03:31 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:03:32 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_key_value(char *key, char *value)
{
	char	*ret;
	int		i;

	i = 0;
	ret = malloc(ft_strlen(key) + ft_strlen(value) + 2);
	if (!ret)
		return (NULL);
	while (*key)
	{
		ret[i++] = *key;
		++key;
	}
	ret[i++] = '=';
	while (*value)
	{
		ret[i++] = *value;
		++value;
	}
	ret[i] = 0;
	return (ret);
}

static size_t	get_env_size(t_var_list *env)
{
	size_t	ret;

	ret = 1;
	while (env)
	{
		env = env->next;
		ret++;
	}
	return (ret);
}

char	**serialize_env(t_var_list *env)
{
	ssize_t	i;
	char	**ret;

	ret = malloc((1 + get_env_size(env)) * sizeof(char *));
	i = 0;
	if (!ret)
		return (NULL);
	while (env)
	{
		if (env->is_exported)
			ret[i] = join_key_value(env->key, env->value);
		if (env->is_exported && !ret[i])
		{
			while (i > 0)
				free(ret[--i]);
			free(ret);
			return (NULL);
		}
		(void)(env->is_exported && i++);
		env = env->next;
	}
	ret[i] = NULL;
	return (ret);
}
