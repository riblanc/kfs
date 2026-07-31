/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:04:05 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:04:07 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_status	my_fork(pid_t *dst, t_env *env)
{
	*dst = fork();
	if (*dst == -1)
		return (FATAL);
	if (!*dst)
	{
		reset_signals(env);
		env->is_interactive = false;
	}
	return (OK);
}

t_status	set_pipe(const t_pipeline *p, const int *next_pipe,
					int prev_pipe_read, t_env *env) {
	int	ret;

	ret = OK;
	if (p->next)
	{
		if (redir(next_pipe[1], 1, NULL, env) == FATAL
			|| close(next_pipe[0]) == -1)
			ret = FATAL;
	}
	if (prev_pipe_read != -1)
	{
		if (redir(prev_pipe_read, 0, NULL, env) == FATAL)
			ret = FATAL;
	}
	if (ret != OK)
		return (my_perror(env, (char *[2]){"Can't set pipe", NULL}, true, FATAL)
				, FATAL);
	return (OK);
}

pid_t	exec_pipe_node(t_env *env, t_pipeline *p, int *next_pipe,
						int prev_pipe_read)
{
	pid_t	pid;

	next_pipe[0] = -1;
	next_pipe[1] = -1;
	if ((p->next && pipe(next_pipe) < -1))
		return (-1);
	if (my_fork(&pid, env) == FATAL)
	{
		close(next_pipe[0]);
		close(next_pipe[1]);
		return (-1);
	}
	else if (!pid)
	{
		if (set_pipe(p, next_pipe, prev_pipe_read, env) == OK)
			exec_command(p->command, env);
		ft_exit(env);
	}
	if (prev_pipe_read != -1)
		close(prev_pipe_read);
	if (next_pipe[0] != -1)
		close(next_pipe[1]);
	return (pid);
}

t_status	exec_pipeline(union u_command cmd, t_env *env)
{
	t_pipeline	*p;
	int			next_pipe[2];
	int			prev_pipe_read;
	pid_t		pid;
	int			to_wait;

	to_wait = -1;
	p = cmd.pipeline;
	prev_pipe_read = -1;
	while (p)
	{
		pid = exec_pipe_node(env, p, next_pipe, prev_pipe_read);
		if (pid == -1)
			return (FATAL);
		prev_pipe_read = next_pipe[0];
		to_wait++;
		p = p->next;
	}
	if (to_wait >= 0)
		get_err(env, pid);
	while (--to_wait >= 0)
		if (wait(NULL) == -1 && errno == EINTR)
			return (FATAL);
	return (OK);
}
