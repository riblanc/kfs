/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/09 19:10:03 by ereali            #+#    #+#             */
/*   Updated: 2021/05/30 17:30:40 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	char	*s2;

	i = 0;
	if (!(s))
		return (NULL);
	if (start > ft_strlen(s) || len == 0)
	{
		s2 = (char *)malloc((sizeof(char) * 1));
		s2[0] = '\0';
		return (s2);
	}
	s2 = (char *)malloc((sizeof(char) * (len + 1)));
	if (!(s2))
		return (NULL);
	while (s[start] && i < len)
	{
		s2[i] = s[start];
		i++;
		start++;
	}
	s2[i] = '\0';
	return (s2);
}
