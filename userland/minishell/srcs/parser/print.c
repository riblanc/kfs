/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:17:25 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 11:50:30 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_print_pipe(t_pipeline **line);
void	ft_print_grouping(t_grouping **line);
void	ft_print_list(t_list **line);

void	ft_prin(t_token_list	**line)
{
	t_token_list	*tmp;

	tmp = (*line);
	if (!tmp)
	{
		printf("empty\n");
		return ;
	}
	while (tmp)
	{
		printf("La string || %s ||\n type de token : %d\n nb de quote :%d\n",
			(tmp)->arg, tmp->type, tmp->nb);
		tmp = tmp->next;
	}
}

void	ft_prin_redir(t_redir	**line)
{
	t_redir	*tmp;

	tmp = (*line);
	if (!tmp)
	{
		printf("empty redir\n");
		return ;
	}
	while (tmp)
	{
		printf("Le type || %d ||\n ", (tmp)->type);
		printf("le word suivant : %s\n", tmp->word);
		printf("int du fd :%d\n", tmp->newfd);
		tmp = tmp->next;
	}
}

void	ft_print_pipe(t_pipeline **line)
{
	t_pipeline	*tmp;

	tmp = (*line);
	if (!tmp)
	{
		printf("empty\n");
		return ;
	}
	while (tmp)
	{
		if (tmp->command.type == SIMPLE)
			ft_prin(&(tmp->command.command.simple.argv_tokens));
		if (tmp->command.type == GROUPING)
			ft_print_grouping(&(tmp->command.command.grouping));
		tmp = tmp->next;
	}
}

void	ft_print_list(t_list **line)
{
	t_list	*tmp;

	tmp = (*line);
	if (!tmp)
	{
		printf("empty list\n");
		return ;
	}
	while (tmp)
	{
		printf("Mon sparateur est le %d\n", tmp->sep);
		if (tmp->command.type == SIMPLE)
			ft_prin(&(tmp->command.command.simple.argv_tokens));
		if (tmp->command.type == PIPELINE)
			ft_print_pipe(&(tmp->command.command.pipeline));
		if (tmp->command.type == GROUPING)
			ft_print_grouping(&(tmp->command.command.grouping));
		tmp = tmp->next;
	}
}

void	ft_print_grouping(t_grouping **line)
{
	t_grouping	*tmp;

	tmp = (*line);
	if (!tmp)
	{
		printf("empty list\n");
		return ;
	}
	if (tmp->command.type == SIMPLE)
		ft_prin(&(tmp->command.command.simple.argv_tokens));
	if (tmp->command.type == PIPELINE)
		ft_print_pipe(&(tmp->command.command.pipeline));
	if (tmp->command.type == LIST)
		ft_print_list(&(tmp->command.command.list));
	printf("-------redir------------\n");
	ft_prin_redir(&(tmp->redir_list));
	return ;
}
