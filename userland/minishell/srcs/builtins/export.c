/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 02:40:37 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 20:16:17 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned char	sh_export(char **argv, t_env *env)
{
	int	ret;
	int	i;

	ret = 0;
	while (++argv && *argv)
	{
		i = 0;
		while ((*argv)[i] && !ft_isdigit((*argv)[0])
			&& isvalid_name_letter((*argv)[i]))
			i++;
		if (i > 0 && (*argv)[i] == '=')
		{
			(*argv)[i] = '\0';
			if (set_var(env, *argv,*argv + i + 1, 1) == FATAL)
				return (128);
			(*argv)[i] = '=';
		}
		else if (!((*argv)[i] == '\0'))
		{
			ret = 1;
			ft_putstr_fd("export :bad variable name\n", 2);
		}
	}
	return (ret);
}
