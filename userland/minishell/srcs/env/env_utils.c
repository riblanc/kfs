/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:02:51 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 21:00:40 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_env(t_env *env)
{
	t_var_list	*tmp;

	while (env->vars)
	{
		free(env->vars->key);
		free(env->vars->value);
		tmp = env->vars->next;
		free(env->vars);
		env->vars = tmp;
	}
	get_next_line(-1, NULL);
	free(env->opened_files.data);
	destroy_tree(env->current_tree_root);
	env->current_tree_root = NULL;
	clear_history();
}

t_status	export_var(t_env *env, char *key)
{
	t_var_list	*tmp;

	tmp = env->vars;
	while (tmp)
	{
		if (!ft_strcmp(key, tmp->key))
		{
			tmp->is_exported = true;
			return (OK);
		}
		tmp = tmp->next;
	}
	return (KO);
}

t_var_list	*ft_lstlast(t_var_list *lst)
{
	while (lst && lst->next)
		lst = lst->next;
	return (lst);
}
