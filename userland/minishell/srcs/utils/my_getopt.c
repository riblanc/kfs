/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_getopt.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:40:40 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:40:41 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// echo -n
// my_getopt(&argv, "n", dst)

void	my_getopt(char ***argv, char *option, char *dest)
{
	int	i;

	ft_bzero(dest, 256);
	while (**argv && ***argv == '-')
	{
		if (!(**argv)[1])
			return ;
		i = 0;
		while ((**argv)[++i])
			if (!ft_strchr(option, (**argv)[i]))
				return ;
		i = 0;
		while ((**argv)[++i])
			dest[(int)*ft_strchr(option, (**argv)[i])] = 1;
		(*argv)++;
	}
}
