/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokeniser.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 21:09:46 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 17:18:02 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_token_list(t_token_list *ptr)
{
	if (!ptr)
		return ;
	free_token_list(ptr->next);
	free(ptr->arg);
	free(ptr);
}

t_token_type	c_type(t_quote nb, const char *str, size_t len)
{
	int					i;
	const char			*operator[] = {"<<", ">>", "&&", "||",
		">", "<", "|", "(", ")"};
	const t_token_type	type[] = {DLESS, DGREAT, AND_IF, OR_IF, GREAT, LESS,
		PIPE, LPARENTHESIS, RPARENTHESIS, WORD, INVALID};

	i = 0;
	while (i < 9 && ft_strncmp((const char *)str + (len), operator[i],
			ft_strlen(operator[i])))
		i++;
	if (i > 8 && !nb && ft_isspace(str[len]))
		return (INVALID);
	else if (i > 8)
		return (WORD);
	return (type[i]);
}

enum e_quote	ft_nbquote(bool *escaped, enum e_quote nb, char **str, size_t l)
{
	if (*escaped)
		(*escaped) = false;
	else if ((*str)[l] == '\'')
	{
		if (nb == ONE)
			nb = NONE;
		else if (nb == NONE)
			nb = ONE;
	}
	else if ((*str)[l] == '"')
	{
		if (nb == DOUBLE)
			nb = NONE;
		else if (nb == NONE)
			nb = DOUBLE;
	}
	else if ((*str)[l] == '\\' && nb != ONE)
		(*escaped) = true;
	return (nb);
}

size_t	create_t_token_list(char **str, t_token_list **line, t_env *env)
{
	size_t			len;
	t_token_list	*node;
	bool			escaped;

	len = 0;
	node = NULL;
	escaped = false;
	node = ft_lstnew(c_type(NONE, *str, len));
	if (!node)
		return (FATAL);
	while ((*str)[len] && (escaped || node->nb > NONE
		|| WORD == c_type(node->nb, *str, len)))
		node->nb = ft_nbquote(&escaped, node->nb, str, len++);
	len += (node->type != WORD)
		+ !!(node->type & (DLESS | DGREAT | OR_IF | AND_IF));
	if (!(*str)[len] && node->nb)
		return (free(node), env->err = 2, my_perror(env, (char *[2]){
				"Syntax error: missing closing quote",
				NULL}, false, KO), KO);
	if (node->type != INVALID && add_token(&node, len, line, (*str)) == OK)
		return (*str += len, OK);
	else if (node->type != INVALID)
		return (FATAL);
	return (free(node), *str += len, OK);
}

t_status	tokenise(char *str, t_token_list **dst, t_env *env)
{
	size_t			i;
	t_status		ret;
	t_token_list	*tmp;

	i = 0;
	*dst = NULL;
	while (i < ft_strlen(str))
	{
		ret = create_t_token_list(&str, dst, env);
		if (ret != OK)
		{
			free_token_list(*dst);
			return (ret);
		}
	}
	tmp = ft_lstnew(END);
	if (!tmp)
	{
		free_token_list(*dst);
		return (FATAL);
	}
	ft_lstadd_back(dst, tmp);
	return (OK);
}
