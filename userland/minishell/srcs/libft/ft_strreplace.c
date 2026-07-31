/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strreplace.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 10:18:46 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 10:18:47 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strreplace(char *s, char *to_add, size_t start_add, size_t to_pass)
{
	char	*ret;
	size_t	j;

	j = 0;
	ret = (char *)malloc(sizeof(*ret) * (ft_strlen(to_add) + ft_strlen(s)
				- (to_pass - start_add) + 1));
	if (!(ret))
		return (NULL);
	ret = ft_strncpy(ret, s, start_add);
	while (to_add && to_add[j])
	{
		ret[start_add + j] = to_add[j];
		j++;
	}
	j = start_add + j;
	while (s[to_pass])
	{
		ret[j] = s[to_pass];
		to_pass++;
		j++;
	}
	free(s);
	ret[j] = '\0';
	return (ret);
}
