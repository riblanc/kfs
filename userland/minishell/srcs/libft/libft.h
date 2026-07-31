/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/07 14:07:44 by ereali            #+#    #+#             */
/*   Updated: 2022/03/04 12:46:30 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <unistd.h>
# include <stdlib.h>

void			*ft_memset(void *b, int c, size_t len);
void			ft_bzero(void *s, size_t n);
void			*ft_memcpy(void *dst, const void *src, size_t n);
void			*ft_memccpy(void *dst, const void *src, int c, size_t n);
void			*ft_memchr(const void *s, int c, size_t n);
int				ft_memcmp(const void *s1, const void *s2, size_t n);
void			*ft_calloc(size_t count, size_t size);
size_t			ft_strlen(const char *s);
char			*ft_strdup(const char *s1);
char			*ft_strndup(size_t n, const char *s1);
size_t			ft_strlcpy(char *dst, const char *src, size_t dstsize);
char			*ft_strcpy(char *dest, const char *src);
char			*ft_strncpy(char *dest, const char *src, size_t n);
size_t			ft_strlcat(char *dst, const char *src, size_t dstsize);
char			*ft_strchr(const char *s, int c);
char			*ft_strrchr(const char *s, int c);
char			*ft_strnstr(const char *haystack,
					const char *needle, size_t len);
int				ft_strncmp(const char *s1, const char *s2, size_t n);
int				ft_strcmp(const char *s1, const char *s2);
int				ft_atoi(char *str);
int				ft_isspace(char c);
int				ft_isalpha(int c);
int				ft_isalnum(int c);
int				ft_isascii(int c);
int				ft_isprint(int c);
int				ft_toupper(int c);
int				ft_tolower(int c);
int				ft_isdigit(int c);
size_t			ft_countoccur(char *str, const char *to_count);
char			*ft_substr(char const *s, unsigned start, size_t len);
char			*ft_strjoin(char const *s1, char const *s2);
char			*ft_strjoinff(char const *s1, char const *s2);
char			*ft_strjoinf2(char const *s1, char const *s2);
char			*ft_strjoinf1(char const *s1, char const *s2);
char			*ft_strreplace(char *s, char *to_add,
					size_t start_add, size_t to_pass);
char			*ft_strtrim(char const *s1, char const *set);
char			**ft_split(char const *s, char c);
void			ft_free_split(char **array);
char			*ft_itoa(int n);
char			*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void			ft_putchar_fd(char c, int fd);
void			ft_putstr_fd(char *s, int fd);
void			ft_putendl_fd(char *s, int fd);
void			ft_putnbr_fd(int n, int fd);
int				nb_is_superior(char *nb, char *to_compare);

#endif
