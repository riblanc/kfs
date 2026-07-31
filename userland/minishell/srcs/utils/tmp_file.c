/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tmp_file.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:41:25 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:41:27 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	tobase64(unsigned char c)
{
	c %= 64;
	if (c < 10)
		c += '0';
	else if (c < 36)
		c = c - 10 + 'A';
	else if (c < 62)
		c = c - 36 + 'a';
	else if (c == 62)
		c = '+';
	else if (c == 63)
		c = '-';
	return (c);
}

char	*rand_str(void)
{
	int		fd;
	int		ret;
	char	buf[21];
	int		i;

	fd = open("/dev/random", O_RDONLY);
	ret = read(fd, &buf, 10);
	if (ret != 10)
	{
		close(fd);
		return (NULL);
	}
	i = 10;
	while (--i >= 0)
	{
		buf[i * 2 + 1] = tobase64(buf[i]);
		buf[i * 2] = tobase64(buf[i] >> 4);
	}
	buf[20] = 0;
	close(fd);
	return (ft_strdup(buf));
}

t_status	find_tmp_dir(char **dst)
{
	*dst = P_tmpdir "/.";
	if (access(*dst, X_OK | W_OK))
		*dst = "/tmp/.";
	if (access(*dst, X_OK | W_OK))
		return (KO);
	return (OK);
}

t_status	my_tmp_file(int *fd, char **dst)
{
	char	*dir;

	if (find_tmp_dir(&dir) == KO)
		return (KO);
	*dst = NULL;
	*fd = -1;
	errno = EEXIST;
	while (*fd == -1 && errno == EEXIST)
	{
		free(*dst);
		*dst = rand_str();
		if (!*dst)
			return (KO);
		*dst = ft_strjoinf2(dir, *dst);
		if (!*dst)
			return (KO);
		*fd = open(*dst, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (*fd == -1 && errno != EEXIST)
			break ;
	}
	return (OK);
}
