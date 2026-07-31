/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/11 19:49:49 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 10:18:15 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_count_word(char const *s, char c)
{
	size_t	i;
	size_t	word;

	i = 0;
	word = 0;
	while (s[i])
	{
		if (s[i] == c)
			word++;
		i++;
	}
	return (word + 1);
}

static char	*ft_get_next_word(char const *s, char c, size_t *j)
{
	char	*word;
	size_t	jcopy;
	size_t	i;

	i = 0;
	jcopy = *j;
	while (s[jcopy] && s[jcopy] != c)
		jcopy++;
	word = (char *)malloc(sizeof(char) * (jcopy - *j + 1));
	if (!(word))
		return (NULL);
	while (*j < jcopy)
	{
		word[i] = s[*j];
		i++;
		(*j)++;
	}
	(*j)++;
	word[i] = '\0';
	return (word);
}

static void	ft_freetablib(char **tab, int i)
{
	while (i >= 0)
	{
		free(tab[i]);
		i--;
	}
	free(tab);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;
	size_t	i;
	size_t	j;
	size_t	word;

	i = 0;
	j = 0;
	if (!(s))
		return (NULL);
	word = ft_count_word(s, c);
	tab = (char **)malloc(sizeof(char *) * (word + 1));
	if (!(tab))
		return (NULL);
	while (i < word)
	{
		tab[i] = ft_get_next_word(s, c, &j);
		if (!(tab[i]))
		{
			ft_freetablib(tab, i);
			return (NULL);
		}
		i++;
	}
	tab[word] = NULL;
	return (tab);
}

void	ft_free_split(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
		free(array[i++]);
	free(array);
}
