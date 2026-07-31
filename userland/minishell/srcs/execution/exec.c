/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:04:05 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:04:07 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	g_int;

int	find_and_exec(t_env *env, t_simple *s, int ret, char **name)
{
	if (!(*s).argv || !(*s).argv[0])
		ret = OK;
	else if (ft_strchr((*s).argv[0], '/'))
	{
		*name = ft_strdup((*s).argv[0]);
		if (*name)
			ret = exec_program(*name, (*s), env);
		free((*name));
	}
	else if (is_special_built_in((*s).argv[0]))
		ret = exec_special_builtin((*s), env);
	else if (is_built_in((*s).argv[0]))
		ret = exec_regular_builtin((*s), env);
	else
	{
		ret = path_find((*s).argv[0], env, name);
		if (ret != FATAL)
			ret = exec_program((*name), (*s), env);
		free((*name));
	}
	return (ret);
}

t_status	exec_simple(union u_command cmd, t_env *env)
{
	t_simple	s;
	int			ret;
	char		*name;

	s = cmd.simple;
	if (expand_simple(&s, env) == FATAL)
		return (FATAL);
	ret = perform_redirection(env, s.redir_list);
	if (ret == KO && !env->is_interactive && (s.argv
			&& is_special_built_in(s.argv[0])))
		ret = FATAL;
	if (ret == OK)
		ret = find_and_exec(env, &s, ret, &name);
	ft_free_split(s.argv);
	if (reset_redirection(env, s.redir_list) == FATAL)
		return (FATAL);
	return (ret);
}

t_status	exec_list(union u_command cmd, t_env *env)
{
	t_list		*l;
	t_status	ret;

	l = cmd.list;
	ret = 0;
	while (l)
	{
		if ((l->sep == AND_IF && env->err == 0)
			|| (l->sep == OR_IF && env->err != 0)
			|| l->sep == SEMI)
			ret = exec_command(l->command, env);
		if (ret != OK)
			break ;
		l = l->next;
	}
	return (ret);
}

t_status	exec_grouping(union u_command cmd, t_env *env)
{
	t_grouping	*g;
	int			ret;
	pid_t		pid;

	g = cmd.grouping;
	if (expand_redir(&g->redir_list, env) == FATAL
		|| perform_redirection(env, g->redir_list) == FATAL)
		return (FATAL);
	if (g->is_in_subshell)
	{
		if (my_fork(&pid, env) == FATAL)
			ret = FATAL;
		else if (!pid)
		{
			exec_command(g->command, env);
			ft_exit(env);
		}
		else
			ret = (get_err(env, pid), OK);
	}
	else
		ret = exec_command(g->command, env);
	if (reset_redirection(env, g->redir_list) == FATAL)
		return (FATAL);
	return (ret);
}

t_status	exec_command(t_command cmd, t_env *env)
{
	const t_command_handler	a[] = {
		&exec_simple,
		&exec_pipeline,
		&exec_list,
		&exec_grouping
	};

	return (a[cmd.type](cmd.command, env));
}
