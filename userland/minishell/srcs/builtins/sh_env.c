/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 02:42:04 by ereali            #+#    #+#             */
/*   Updated: 2022/03/03 02:42:05 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned char	sh_env(char **argv, t_env *env)
{
	t_var_list	*first;

	(void)argv;
	first = env->vars;
	while (env->vars && env->vars->key)
	{
		if (env->vars->is_exported == true)
		{
			ft_putstr_fd(env->vars->key, 1);
			ft_putstr_fd("=", 1);
			ft_putstr_fd(env->vars->value, 1);
			ft_putstr_fd("\n", 1);
		}
		env->vars = env->vars->next;
	}
	env->vars = first;
	return (0);
}
