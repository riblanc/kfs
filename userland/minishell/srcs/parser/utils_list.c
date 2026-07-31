/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 00:50:40 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 13:42:34 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token_list	*ft_lstnew(t_token_type token)
{
	t_token_list	*new;

	new = (t_token_list *)malloc(sizeof(t_token_list));
	if (!new)
		return (NULL);
	new->arg = NULL;
	new->type = token;
	new->nb = NONE;
	new->next = NULL;
	return (new);
}

t_token_list	*ft_lstcpy(t_token_list *current)
{
	t_token_list	*cpy;

	cpy = ft_lstnew(current->type);
	if (!cpy)
		return (NULL);
	cpy->arg = ft_strdup((current)->arg);
	if (!cpy->arg)
	{
		free(cpy);
		return (NULL);
	}
	return (cpy);
}

size_t	ft_lstsize(t_token_list *lst)
{
	size_t	i;

	i = 0;
	if (!lst)
		return (0);
	while (lst)
	{
		lst = lst->next;
		i++;
	}
	return (i);
}

t_status	ft_lstinsertword(t_token_list **alst, char *str)
{
	t_token_list	*new;

	new = ft_lstnew(WORD);
	if (!new)
		return (FATAL);
	new->arg = str;
	new->next = (*alst)->next;
	(*alst)->next = new;
	return (OK);
}
