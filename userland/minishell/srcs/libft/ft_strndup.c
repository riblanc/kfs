/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 00:58:50 by ereali            #+#    #+#             */
/*   Updated: 2022/03/03 00:59:05 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strndup(size_t n, const char *s1)
{
	size_t	i;
	char	*cpy;

	i = 0;
	cpy = (char *)malloc(sizeof(*cpy) * (n + 1));
	if (!(cpy))
		return (NULL);
	while (s1[i] && i < n)
	{
		cpy[i] = s1[i];
		i++;
	}
	cpy[i] = '\0';
	return (cpy);
}
