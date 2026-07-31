/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 02:41:49 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 20:54:08 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned char	sh_unset(char **argv, t_env *env)
{
	int		i;

	i = 0;
	while (++argv && *argv)
	{
		while ((*argv)[i] && !ft_isdigit((*argv)[0])
			&& isvalid_name_letter((*argv)[i]))
			i++;
		if ((*argv)[i] != '\0')
			ft_putstr_fd("unset : bad variable name\n", 2);
		else if (i > 0)
			remove_var(env, (*argv));
		i = 0;
	}
	return (0);
}
