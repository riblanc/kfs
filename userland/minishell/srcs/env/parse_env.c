/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:03:23 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:03:24 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	parse_env(char **envp, t_env *env)
{
	char	*equal;

	*env = (t_env){};
	while (*envp)
	{
		equal = *envp;
		while (*equal && *equal != '=')
			equal++;
		*equal = 0;
		++equal;
		if (equal)
		{
			if (add_var(env, *envp, equal, true) == FATAL)
				return (FATAL);
		}
		envp++;
	}
	return (OK);
}
