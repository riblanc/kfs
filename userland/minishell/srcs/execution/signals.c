/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:04:33 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:04:34 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler(int sig)
{
	(void)sig;
	rl_done = 1;
	g_int = true;
}

t_status	set_signal(int sig, t_sighandler action, t_env *env)
{
	t_sighandler	ret;

	ret = signal(sig, action);
	if (ret == (t_sighandler)-1)
		return (KO);
	if (env->default_signals[sig] == (t_sighandler)-1)
		env->default_signals[sig] = ret;
	return (OK);
}

void	reset_signals(t_env *env)
{
	if (env->default_signals[SIGINT] != (t_sighandler)-1)
		signal(SIGINT, env->default_signals[SIGINT]);
	if (env->default_signals[SIGQUIT] != (t_sighandler)-1)
		signal(SIGQUIT, env->default_signals[SIGQUIT]);
}
