/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:12:29 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 22:33:11 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_word_all(char *str, t_env *env)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (str[i])
	{
		while (str[i] && str[i] != '$')
			i++;
		j = i + 1;
		if (str[i] && str[j] && !ft_isdigit(str[j])
			&& (isvalid_name_letter(str[j]) == true || str[j] == '?'))
		{
			while (str[j] && isvalid_name_letter(str[j]) == true)
				j++;
			if (str[j] == '?' && j == i + 1)
				j++;
			str = ft_replacekey(i, j, str, env);
			if (!str)
				return (NULL);
		}
		else if (str[i])
		i++;
	}
	return (str);
}

char	*expand_word(char *str, t_env *env)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (str[i])
	{
		while (str[i] && str[i] != '$')
			i++;
		j = i + 1;
		if (str[i] && str[j] && !ft_isdigit(str[j])
			&& need_to_expand(str, i) < 2
			&& (isvalid_name_letter(str[j]) == true || str[j] == '?'))
		{
			while (str[j] && isvalid_name_letter(str[j]) == true)
				j++;
			if (str[j] == '?' && j == i + 1)
				j++;
			str = ft_replacekey(i, j, str, env);
			if (!str)
				return (NULL);
		}
		else if (str[i])
			i++;
	}
	return (str);
}

t_status	expand_redir(t_redir **first, t_env *env)
{
	t_redir	*command;

	command = (*first);
	if (!command)
		return (KO);
	while (command && command->word)
	{
		if (ft_strchr(command->word, '$'))
		{
			command->word = expand_word(command->word, env);
			if (!command->word)
				return (FATAL);
		}
		if (remove_quotes(command->word) == FATAL)
			return (FATAL);
		command = command->next;
	}
	if (!command)
		return (OK);
	return (KO);
}

t_status	expand_path(t_token_list *lst, t_str_vec *dst)
{
	int		tmp;
	char	*tmp_s;

	*dst = str_vec_init();
	while (lst)
	{
		tmp = dst->size;
		if (lst->arg[0] && path_match_current(lst->arg, dst) == FATAL)
			return (free_vec(dst), FATAL);
		if (tmp == dst->size && lst->arg[0])
		{
			tmp_s = ft_strdup(lst->arg);
			if (!tmp_s || str_vec_push(dst, tmp_s) == 0)
				return (free(tmp_s), free_vec(dst), FATAL);
		}
		lst = lst->next;
	}
	if (str_vec_push(dst, NULL) == FATAL)
		return (free_vec(dst), FATAL);
	return (OK);
}

t_status	expand_simple(t_simple *command, t_env *env)
{
	t_status		ret;
	t_token_list	*tmp;

	if (!command || !command->argv_tokens)
		return (KO);
	tmp = command->argv_tokens;
	while (tmp && tmp->arg)
	{
		tmp->arg = expand_word(tmp->arg, env);
		if (!tmp->arg || ft_field_split(&tmp))
			return (FATAL);
		tmp = tmp->next;
	}
	if (ft_fillargv(command) != OK)
		return (FATAL);
	ret = OK;
	if (command->redir_list)
	{
		ret = expand_redir(&command->redir_list, env);
		if (ret == FATAL)
			ft_free_split(command->argv);
	}
	return (ret);
}
