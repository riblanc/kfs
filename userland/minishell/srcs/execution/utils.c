/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:04:39 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:04:40 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*sig_message_1(int sig)
{
	char	*a[NSIG];

	ft_memset(a, 0, NSIG * sizeof (char *));
	a[SIGHUP] = "Hangup";
	a[SIGQUIT] = "Quit";
	a[SIGILL] = "Illegal instruction";
	a[SIGTRAP] = "BPT trace/trap";
	a[SIGABRT] = "ABORT instruction";
	a[SIGFPE] = "Floating point exception";
	a[SIGKILL] = "Killed";
	a[SIGBUS] = "Bus error";
	a[SIGSEGV] = "Segmentation fault";
	a[SIGSYS] = "Bad system call";
	a[SIGPIPE] = "Broken pipe";
	a[SIGALRM] = "Alarm clock";
	a[SIGTERM] = "Terminated";
	a[SIGURG] = "Urgent IO condition";
	a[SIGSTOP] = "Stopped (signal)";
	a[SIGTSTP] = "Stopped";
	a[SIGCONT] = "Continue";
	return (a[sig]);
}

char	*sig_message_2(int sig)
{
	char	*a[NSIG];

	ft_memset(a, 0, NSIG * sizeof (char *));
	a[SIGCHLD] = "Child death or stop";
	a[SIGTTIN] = "Stopped (tty input)";
	a[SIGTTOU] = "Stopped (tty output)";
	a[SIGIO] = "I/O ready";
	a[SIGXCPU] = "CPU limit";
	a[SIGXFSZ] = "File limit";
	a[SIGVTALRM] = "Alarm (virtual)";
	a[SIGPROF] = "Alarm (profile)";
	a[SIGWINCH] = "Window changed";
	a[SIGUSR1] = "User signal 1";
	a[SIGUSR2] = "User signal 2";
	a[SIGPWR] = "power failure imminent";
	return (a[sig]);
}

char	*get_sig_message(int sig)
{
	char	*ret;

	ret = sig_message_1(sig);
	if (!ret)
		ret = sig_message_2(sig);
	if (!ret)
		return ("");
	return (ret);
}

t_status	get_err(t_env *env, pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1 && errno == EINTR)
		return (FATAL);
	if (WIFEXITED(status))
		env->err = WEXITSTATUS(status);
	else
	{
		env->err = 128 + WTERMSIG(status);
		if (env->is_interactive)
		{
			ft_putstr_fd((char *) get_sig_message(WTERMSIG(status)), 2);
			write(2, "\n", 1);
		}
	}
	return (OK);
}

bool	is_dir(char *path)
{
	struct stat	buf;

	if (stat(path, &buf) == 0 && S_ISDIR(buf.st_mode))
		return (true);
	return (false);
}
