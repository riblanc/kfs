/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/09 20:06:29 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 13:44:50 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static char	*ft_cpy(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	char	*result;

	i = 0;
	j = 0;
	result = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2)) + 1);
	if (!result)
		return (NULL);
	while (s1[i])
	{
		result[i] = s1[i];
		i++;
	}
	while (s2[j])
	{
		result[i] = s2[j];
		j++;
		i++;
	}
	result[i] = '\0';
	return (result);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*result;

	if ((!(s1)) && (!(s2)))
		return (NULL);
	if (!(s1))
		return (ft_strdup((char *)s2));
	if (!(s2))
		return (ft_strdup((char *)s1));
	result = ft_cpy(s1, s2);
	return (result);
}

char	*ft_strjoinf1(char const *s1, char const *s2)
{
	char	*ret;

	ret = ft_strjoin(s1, s2);
	free((char *)s1);
	return (ret);
}

char	*ft_strjoinf2(char const *s1, char const *s2)
{
	char	*ret;

	ret = ft_strjoin(s1, s2);
	free((char *)s2);
	return (ret);
}

char	*ft_strjoinff(char const *s1, char const *s2)
{
	char	*ret;

	ret = ft_strjoin(s1, s2);
	free((char *)s1);
	free((char *)s2);
	return (ret);
}
