/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_simple.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 08:22:51 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 14:30:42 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_lstadd_back(t_token_list **alst, t_token_list *new)
{
	t_token_list	*tmp;

	if (!(*alst))
	{
		(*alst) = new;
		new->next = NULL;
	}
	else if (new)
	{
		tmp = (*alst);
		while (tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = new;
		new->next = NULL;
	}
}

t_status	parse_simple_token(t_token_list **current, t_env *env,
							t_command *tree, t_redir **tmp)
{
	int				ret;
	t_token_list	*cpy;

	if ((*current)->type == WORD)
	{
		cpy = ft_lstcpy(*current);
		if (!cpy)
			return (FATAL);
		ft_lstadd_back(&((*tree).command.simple.argv_tokens), cpy);
	}
	else
	{
		ret = new_redir_list(current, tmp, env);
		if (ret != OK || !(*tmp))
			return (ret);
		ft_lstadd_back_redir(&(*tree).command.simple.redir_list, (*tmp));
	}
	return (OK);
}

t_command	parse_simple(t_token_list **current, t_env *env)
{
	t_command	tree;
	t_redir		*tmp;
	t_status	ret;

	tree = (t_command){.type = SIMPLE};
	while ((*current) && (*current)->type >= WORD && (*current)->type <= DGREAT)
	{
		ret = parse_simple_token(current, env, &tree, &tmp);
		if (ret == KO)
			return (parse_error((t_command[2]){(tree)},
				(t_command){.type = PARSE_ERROR}));
		else if (ret == FATAL)
			return (parse_error((t_command[2]){(tree)},
				(t_command){.type = PARSE_ERROR}));
		(*current) = (*current)->next;
	}
	return (tree);
}
