/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 21:09:46 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 23:31:09 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	need_to_expand(char *str, size_t dollars)
{
	size_t	i;
	int		tab[3];

	i = 0;
	ft_bzero(tab, 3 * sizeof(int));
	while (i < dollars)
	{
		if (tab[2] == 4)
			tab[2] = 0;
		else if (str[i] == '\"' && !tab[1] && !tab[2] && tab[0] == 0)
			tab[0] = 1;
		else if (str[i] == '\"' && !tab[1] && !tab[2] && tab[0])
			tab[0] = 0;
		else if (str[i] == '\'' && !tab[0] && !tab[2] && tab[1] == 0)
			tab[1] = 2;
		else if (str[i] == '\'' && !tab[0] && !tab[2] && tab[1])
			tab[1] = 0;
		else if (str[i] == '\\' && !tab[1])
			tab[2] = 4;
		i++;
	}
	return (tab[0] + tab[1] + tab[2]);
}

t_status	found_val(char **new, char *key, t_env *env)
{
	(*new) = get_var_val(env, key);
	(*new) = ft_inhibit((*new), "\\\"\'*");
	if (!(*new))
		return (FATAL);
	return (OK);
}

char	*ft_replacekey(size_t i, size_t j, char *str, t_env *env)
{
	char	*new;
	char	*key;

	key = ft_strndup(j - i - 1, (const char *)(str + i + 1));
	if (!key)
		return (NULL);
	if (!ft_strcmp(key, "?"))
	{
		new = ft_itoa(env->err);
		if (!new)
			return (free(key), NULL);
	}
	else if (found_val(&new, key, env) != OK)
		return (free(key), NULL);
	str = ft_strreplace(str, new, i, i + ft_strlen(key) + 1);
	free(key);
	free(new);
	if (!str)
		return (NULL);
	return (str);
}

t_token_list	*token_list_pop(t_token_list *node, t_token_list **begin)
{
	t_token_list	*tmp;

	if (!*begin)
		return (NULL);
	tmp = *begin;
	while (tmp->next && tmp->next != node)
		tmp = tmp->next;
	if (tmp->next)
		tmp->next = tmp->next->next;
	if (node)
		free(node->arg);
	free(node);
	return (tmp->next);
}

t_status	ft_field_split(t_token_list **lst)
{
	char	*arg;
	int		i;

	i = ft_strlen((*lst)->arg);
	if (i > 0)
		i -= 1;
	while (i >= 0)
	{
		while (i >= 0 && !(ft_isspace((*lst)->arg[i])
				&& need_to_expand((*lst)->arg, i) == 0))
			i--;
		if (i >= 0)
		{
			arg = ft_strdup((*lst)->arg + i + 1);
			if (!arg)
				return (FATAL);
			if (!*arg)
				free(arg);
			else if (ft_lstinsertword(lst, arg) == FATAL)
				return (free(arg), FATAL);
			(*lst)->arg[i] = '\0';
		}
	}
	return (OK);
}
