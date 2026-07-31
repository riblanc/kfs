/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 17:04:15 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 17:04:33 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_lstadd_back_redir(t_redir **alst, t_redir *new)
{
	t_redir	*tmp;

	if (!(*alst))
	{
		(*alst) = new;
		new->next = NULL;
	}
	else if (new)
	{
		tmp = (*alst);
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
		new->next = NULL;
	}
}

void	free_redir(t_redir *list)
{
	if (!list)
		return ;
	free_redir(list->next);
	free(list->word);
	free(list);
}

enum e_redir	ft_redir_type(t_token_list **current)
{
	if ((*current)->type == LESS)
		return (INPUT);
	else if ((*current)->type == GREAT)
		return (OUTPUT);
	else if ((*current)->type == DGREAT)
		return (APPEND);
	else
		return (HERE);
}

t_status	new_redir_list(t_token_list **current, t_redir **dst, t_env *env)
{
	*dst = (t_redir *)malloc(sizeof(t_redir));
	if (!*dst)
		return (FATAL);
	**dst = (t_redir){.fd_save = -1};
	(*dst)->type = ft_redir_type(current);
	if ((*current)->arg[0] == '>')
		(*dst)->newfd = 1;
	if ((*current)->next && (*current)->next->type == WORD)
	{
		(*current) = ((*current)->next);
		(*dst)->word = ft_strdup((*current)->arg);
		if (!(*dst)->word)
			return (free(*dst), FATAL);
	}
	else
		return (free((*dst)->word), free(*dst), KO);
	if ((*dst)->type == HERE && ft_heredoc(env, *dst) != OK)
		return (free((*dst)->word), free(*dst), KO);
	return (OK);
}
