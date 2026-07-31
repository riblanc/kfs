/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 02:40:29 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 18:36:37 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned char	sh_exit(char **argv, t_env *env)
{
	int	i;

	i = 0;
	if (!(argv[1]))
	{
		ft_free_split(argv);
		ft_exit(env);
	}
	if ((argv[1])[i] == '-' && ft_isdigit((argv[1])[i + 1]))
		i++;
	while (ft_isdigit((argv[1])[i]))
		i++;
	if ((argv[1])[i] == '\0'
		&& !(nb_is_superior(argv[1], "9223372036854775807")))
		env->err = ft_atoi(argv[1]);
	else if (!ft_isdigit((argv[1])[i]) || ((i >= 20 || nb_is_superior(argv[1],
		"9223372036854775807")) && (argv[1])[i] == '\0'))
		return (ft_putstr_fd("exit : numeric argument required\n", 2), 2);
	ft_free_split(argv);
	ft_exit(env);
	return (0);
}
