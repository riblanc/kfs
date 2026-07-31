/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 21:09:46 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 17:04:41 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	parse_error(t_command to_destroy[2], t_command ret)
{
	destroy_tree(to_destroy);
	destroy_tree(to_destroy + 1);
	return (ret);
}

t_command	parsing(t_token_list **current, t_token_type expected, t_env *env)
{
	t_command	tree;
	bool		operator;

	tree = (t_command){.type = PARSE_ERROR};
	operator = false;
	if (!(*current))
		return (tree);
	while (!(expected & (*current)->type))
	{
		if (!operator && ((*current)->type >= WORD && (*current)->type <= 16))
			tree = parse_simple(current, env);
		else if (!operator && (*current)->type == LPARENTHESIS)
			tree = parse_grouping(current, env);
		else if (operator && (*current)->type == PIPE)
			tree = parse_pipe(current, tree, env);
		else if (operator && ((*current)->type == OR_IF
				|| (*current)->type == AND_IF))
			tree = parse_list(current, tree, env);
		else
			return (destroy_tree(&tree), (t_command){.type = PARSE_ERROR});
		if (tree.type == PARSE_ERROR || tree.type == PARSE_FATAL)
			return (tree);
		operator = true;
	}
	return (tree);
}

const char	*get_token_str(t_token_type token)
{
	int			i;
	const char	*token_list[] = {
		"word",
		">",
		"<",
		"<<",
		">>",
		"|",
		"&&",
		"||",
		"(",
		")",
		";",
		"end of line",
	};

	i = 0;
	while (token != 1)
	{
		token >>= 1;
		i++;
	}
	return (token_list[i]);
}

t_status	parse_tree(t_token_list *current, t_command *tree, t_env *env)
{
	*tree = parsing(&current, END, env);
	env->current_tree_root = tree;
	if (tree->type == PARSE_FATAL)
		return (FATAL);
	else if (g_int)
		return (KO);
	else if (tree->type == PARSE_ERROR)
	{
		env->err = 2;
		return (my_perror(env, (char *[2]){
				"Syntax error near unexpected token: ",
				(char *)get_token_str(current->type)}, false, KO));
	}
	return (OK);
}
