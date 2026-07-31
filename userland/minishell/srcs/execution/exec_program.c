/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_program.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:03:57 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:03:59 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	check_file(char *name, t_simple s, t_env *env)
{
	char	*msg;

	if (!name || (access(name, X_OK) && (errno == ENOENT || errno == ENOTDIR)))
	{
		msg = ": Not found";
		env->err = 127;
	}
	else if (is_dir(name))
	{
		msg = ": Is a directory";
		env->err = 126;
	}
	else if (access(name, X_OK))
	{
		msg = ": Permission denied";
		env->err = 126;
	}
	else
		return (true);
	my_perror(env, (char *[2]){s.argv[0], msg}, false, OK);
	return (false);
}

t_status	exec_program(char *name, t_simple s, t_env *env)
{
	char		**envp;
	pid_t		pid;

	if (!check_file(name, s, env))
		return (OK);
	pid = fork();
	if (pid == -1)
		return (FATAL);
	else if (!pid)
	{
		reset_signals(env);
		env->is_interactive = false;
		clean_fds(env);
		envp = serialize_env(env->vars);
		if (!envp)
			return (FATAL);
		execve(name, s.argv, envp);
		my_perror(env, (char *[2]){name, NULL}, true, 0);
		env->err = 126;
		ft_free_split(s.argv);
		(void)((ft_free_split(envp), true) && (free(name), true));
		ft_exit (env);
	}
	return (get_err(env, pid), OK);
}
