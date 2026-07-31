/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dupenv.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:02:32 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:02:38 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_var_list	*dup_var_list(t_var_list *src)
{
	t_env	ret;

	ret.vars = NULL;
	while (src)
	{
		if (src->is_exported)
		{
			if (add_var(&ret, src->key, src->value, true) == FATAL)
				return (NULL);
		}
		src = src->next;
	}
	return (ret.vars);
}
