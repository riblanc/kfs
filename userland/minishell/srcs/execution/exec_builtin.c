/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:03:47 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:03:48 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	exec_regular_builtin(t_simple s, t_env *env)
{
	env->err = is_built_in(s.argv[0])(s.argv, env);
	return (OK);
}

t_status	exec_special_builtin(t_simple s, t_env *env)
{
	env->err = is_special_built_in(s.argv[0])(s.argv, env);
	if (env->err == 0)
		return (OK);
	if (env->is_interactive)
		return (KO);
	ft_exit(env);
	return (OK);
}
