/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:03:04 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/07 14:21:54 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	add_var(t_env *env, char *key, char *value, bool is_exported)
{
	t_var_list	*new_node;
	t_var_list	*tmp;

	new_node = malloc(sizeof(t_var_list));
	if (!new_node)
		return (FATAL);
	new_node->key = ft_strdup(key);
	new_node->value = ft_strdup(value);
	new_node->is_exported = is_exported;
	if (!new_node->key || !new_node->value)
	{
		free(new_node->key);
		return (free(new_node->value), free(new_node), FATAL);
	}
	new_node->next = NULL;
	if (!env->vars)
		env->vars = new_node;
	else
	{
		tmp = env->vars;
		tmp = ft_lstlast(tmp);
		tmp->next = new_node;
	}
	return (OK);
}

char	*get_var_val(t_env *env, char *key)
{
	t_var_list	*tmp;

	tmp = env->vars;
	while (tmp)
	{
		if (!ft_strcmp(key, tmp->key))
			return (tmp->value);
		tmp = tmp->next;
	}
	return (NULL);
}

t_var_list	*get_var_ptr(t_env *env, char *key)
{
	t_var_list	*tmp;

	tmp = env->vars;
	while (tmp)
	{
		if (!ft_strcmp(key, tmp->key))
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

t_status	set_var(t_env *env, char *key, char *value, bool is_exported)
{
	t_var_list	*tmp;

	tmp = env->vars;
	while (tmp)
	{
		if (!ft_strcmp(key, tmp->key))
		{
			if (value == tmp->value)
				return (OK);
			free(tmp->value);
			tmp->value = ft_strdup(value);
			if (!tmp->value)
				return (FATAL);
			return (OK);
		}
		tmp = tmp->next;
	}
	return (add_var(env, key, value, is_exported));
}

t_status	remove_var(t_env *env, char *key)
{
	t_var_list	*tmp;
	t_var_list	*old;

	tmp = env->vars;
	old = NULL;
	while (tmp)
	{
		if (!ft_strcmp(key, tmp->key))
		{
			free(tmp->key);
			free(tmp->value);
			if (!old)
				env->vars = tmp->next;
			else
				old->next = tmp->next;
			free(tmp);
			return (OK);
		}
		old = tmp;
		tmp = tmp->next;
	}
	return (KO);
}
