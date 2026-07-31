/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   classic_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 17:10:51 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 17:12:44 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	isvalid_name_letter(char c)
{
	if (c == '_' || (ft_isalnum(c) && c != '\'' && c != '\\' && c != '\"'))
		return (1);
	return (0);
}

t_status	add_token(t_token_list **node, size_t len, t_token_list **line,
		char *str)
{
	if ((*node)->type != INVALID)
	{
		(*node)->arg = ft_strndup(len, str);
		if (!(*node)->arg)
			return (free(*node), FATAL);
		ft_lstadd_back(line, (*node));
	}
	return (OK);
}

t_status	ft_fillargv(t_simple *command)
{
	int				j;
	t_str_vec		dst;

	j = 0;
	if (!command->argv_tokens)
		return (OK);
	if (expand_path(command->argv_tokens, &dst) == FATAL)
		return (free_vec(&dst), FATAL);
	while (j < dst.size - 1)
	{
		if (remove_quotes(dst.data[j]) == FATAL)
			return (free_vec(&dst), FATAL);
		j++;
	}
	dst.data[j] = NULL;
	command->argv = dst.data;
	return (OK);
}
