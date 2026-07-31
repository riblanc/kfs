/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 08:22:15 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 10:23:41 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*ft_new_list(t_command command, t_token_list *current, int prev)
{
	t_list	*new;

	new = (t_list *)malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	if (current && (prev == OR_IF || prev == AND_IF))
		new->sep = prev;
	else
		new->sep = SEMI;
	new->command = command;
	new->next = NULL;
	return (new);
}

t_command	parse_list(t_token_list **current, t_command prev_command,
	t_env *env)
{
	t_command	tree;
	t_command	tmp_com;
	t_list		*tmp;
	int			prev;

	tree.type = LIST;
	tree.command.list = ft_new_list(prev_command, *current, SEMI);
	if (!tree.command.list)
		return (parse_error((t_command[2]){prev_command},
			(t_command){.type = PARSE_FATAL}));
	tmp = tree.command.list;
	while (*current && ((*current)->type == AND_IF || (*current)->type == 128))
	{
		prev = (*current)->type;
		(*current) = (*current)->next;
		tmp_com = parsing(current, AND_IF | 128 | 512 | END, env);
		if (tmp_com.type == PARSE_FATAL || tmp_com.type == PARSE_ERROR)
			return (parse_error((t_command[2]){tree}, tmp_com));
		tmp->next = ft_new_list(tmp_com, *current, prev);
		if (!tmp->next)
			return (parse_error((t_command[2]){tree, tmp_com},
				(t_command){.type = PARSE_FATAL}));
		tmp = tmp->next;
	}
	return (tree);
}
