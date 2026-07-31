/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:06:37 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:06:38 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	ft_shell(t_env *env, char *line)
{
	t_token_list	*tokens;
	t_command		tree;
	int				ret;

	ret = tokenise(line, &tokens, env);
	free(line);
	if (ret != OK)
	{
		if (env->is_interactive)
			return (ret);
		else
			ft_exit(env);
	}
	if (tokens->type == END)
		return (free_token_list(tokens), OK);
	ret = parse_tree(tokens, &tree, env);
	free_token_list(tokens);
	if (ret == OK)
		ret = exec_command(tree, env);
	else if (!env->is_interactive)
		ft_exit(env);
	destroy_tree(&tree);
	env->current_tree_root = NULL;
	return (ret);
}

t_status	loop(t_env *env)
{
	char	*line;
	int		ret;

	while (1)
	{
		ret = readnline(&line, env);
		if (ret == FATAL)
			return (FATAL);
		if (ret == OK && ft_shell(env, line) == FATAL)
			return (FATAL);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_env		env;

	(void)argc;
	(void)argv;
	if (parse_env(envp, &env) != FATAL && init_env(&env) != FATAL)
		loop(&env);
	if (errno)
		perror(NAME);
	free_env(&env);
	return (1);
}
