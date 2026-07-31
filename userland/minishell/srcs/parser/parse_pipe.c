/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipe.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 08:22:43 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 10:25:22 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_pipeline	*ft_new_pipe(t_command command)
{
	t_pipeline	*new;

	new = (t_pipeline *)malloc(sizeof(t_pipeline));
	if (!new)
		return (NULL);
	new->command = command;
	new->next = NULL;
	return (new);
}

t_command	parse_pipe(t_token_list **current,
	t_command prev_command, t_env *env)
{
	t_command	tree;
	t_command	tmp_c;
	t_pipeline	*first;
	t_pipeline	*tmp;

	tree.type = PIPELINE;
	tree.command.pipeline = ft_new_pipe(prev_command);
	if (!tree.command.pipeline)
		return (parse_error((t_command[2]){prev_command},
			(t_command){.type = PARSE_FATAL}));
	first = tree.command.pipeline;
	tmp = first;
	while ((*current) && (*current)->type == PIPE)
	{
		(*current) = (*current)->next;
		tmp_c = parsing(current, 64 | 128 | PIPE | 512 | END, env);
		if (tmp_c.type == PARSE_ERROR || tmp_c.type == PARSE_FATAL)
			return (parse_error((t_command[2]){tree}, tmp_c));
		tmp->next = ft_new_pipe(tmp_c);
		if (!tmp->next)
			return (parse_error((t_command[2]){tree, tmp_c},
				(t_command){.type = PARSE_FATAL}));
		tmp = tmp->next;
	}
	return (tree);
}
