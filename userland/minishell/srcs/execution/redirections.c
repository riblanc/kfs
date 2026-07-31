/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:04:25 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:04:26 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * redirect oldfd to newd and save a copy of newfd in save
 * @return OK or FATAL if a syscall fail
 */
t_status	redir(int oldfd, int newfd, int *save, t_env *env)
{
	if (save)
	{
		*save = dup (newfd);
		if (*save == -1)
			return (FATAL);
		if (env->opened_files.size <= *save
			&& !char_vec_resize(&env->opened_files, *save + 1, FD_CLOSE))
			return (FATAL);
		env->opened_files.data[*save] = FD_TMP;
	}
	if (dup2(oldfd, newfd) == -1)
		return (FATAL);
	if (oldfd < env->opened_files.size)
		env->opened_files.data[oldfd] = FD_CLOSE;
	if (newfd >= env->opened_files.size
		&& !char_vec_resize(&env->opened_files, newfd + 1, FD_CLOSE))
		return (FATAL);
	env->opened_files.data[newfd] = FD_OPEN;
	close(oldfd);
	return (OK);
}

t_status	open_redir(t_env *env, t_redir *r)
{
	int	old_fd;

	if (r->type == OUTPUT || r->type == CLOBBER)
		old_fd = open(r->word, O_CREAT | O_WRONLY | O_TRUNC, 00644);
	else if (r->type == APPEND)
		old_fd = open(r->word, O_CREAT | O_WRONLY | O_APPEND, 00644);
	else if (r->type == INPUT || r->type == HERE)
		old_fd = open(r->word, O_RDONLY);
	else
		old_fd = open(r->word, O_RDWR | O_CREAT | O_TRUNC, 00644);
	if (old_fd == -1)
		return (my_perror(env, (char *[2]){"cannot open ", r->word}, true, KO));
	return (redir(old_fd, r->newfd, &r->fd_save, env));
}

t_status	perform_redirection(t_env *env, t_redir *list)
{
	int	ret;

	ret = OK;
	while (list)
	{
		if ((list->type == DUPIN || list->type == DUPOUT))
			ret = redir(list->oldfd, list->newfd, &list->fd_save, env);
		else
			ret = open_redir(env, list);
		if (ret != OK)
			break ;
		list = list->next;
	}
	return (ret);
}

void	clean_fds(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->opened_files.size)
	{
		if (env->opened_files.data[i] == FD_TMP)
		{
			close(i);
			env->opened_files.data[i] = FD_CLOSE;
		}
		i++;
	}
}

t_status	reset_redirection(t_env *env, t_redir *list)
{
	int	ret;

	if (!list)
		return (OK);
	ret = reset_redirection(env, list->next);
	if (ret == FATAL)
		return (ret);
	if (list->type == DUPIN || list->type == DUPOUT)
		ret |= (redir(list->newfd, list->oldfd, NULL, env) == FATAL) * FATAL;
	else if (list->fd_save != -1)
	{
		ret |= dup2(list->fd_save, list->newfd);
		close(list->fd_save);
		env->opened_files.data[list->fd_save] = FD_CLOSE;
	}
	if (list->type == HERE && unlink(list->word) == -1)
		return (FATAL);
	return (ret);
}
