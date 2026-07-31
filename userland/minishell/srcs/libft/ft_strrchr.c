/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/08 16:17:38 by ereali            #+#    #+#             */
/*   Updated: 2019/10/16 21:31:20 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int	i;

	i = ft_strlen(s) - 1;
	if (c == '\0')
		return ((char *)s + ft_strlen(s));
	while (i >= 0 && s[i] != c)
		i--;
	if (i < 0)
		return (NULL);
	return ((char *)(&s[i]));
}
