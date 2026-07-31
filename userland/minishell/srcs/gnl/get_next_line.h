/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/30 20:29:15 by ereali            #+#    #+#             */
/*   Updated: 2019/11/30 21:01:51 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 100
# endif

int		gnl_ft_check_str(char **str, char **line);
char	*gnl_ft_use_read(char *str, char *str1, int fd, int *i);
int		gnl_ft_line(char **line, char **str, int i);
int		get_next_line(int fd, char **line);
char	*gnl_ft_swap(int start, int end, char *str);
int		gnl_ft_strlen(char *str);
char	*gnl_ft_bzero(char *str, int n);
int		gnl_ft_strchr(const char *s, int c);
char	*gnl_ft_strjoin(char *s1, char *s2);

#endif
