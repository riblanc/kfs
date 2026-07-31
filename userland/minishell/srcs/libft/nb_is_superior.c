/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nb_is_inferior.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 12:32:50 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 19:08:25 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

int	nb_is_superior(char *nb, char *to_compare)
{
	int	i;

	i = 0;
	if (nb[i] == '-' && to_compare[i] != '-')
		return (0);
	else if (nb[i] != '-' && to_compare[i] == '-')
		return (1);
	else if (nb[i] == '-')
		i++;
	if (ft_strlen(nb) > ft_strlen(to_compare))
		return (1);
	else if (ft_strlen(nb) < ft_strlen(to_compare))
		return (0);
	while (nb[i])
	{
		if (nb[i] > to_compare[i])
		{
			return (1);
		}
		if (nb[i] < to_compare[i])
			return (0);
		i++;
	}
	return (0);
}
