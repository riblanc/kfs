/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   char_vec.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:01:29 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/04 11:01:31 by jgiron           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHAR_VEC_H
# define CHAR_VEC_H

# include <stdlib.h>
# include <stdbool.h>

typedef char	t_type_char_vec;

typedef struct s_char_vec {
	t_type_char_vec	*data;
	int				size;
	int				capacity;
}				t_char_vec;

int				char_vec_push(t_char_vec *v, t_type_char_vec value);
t_type_char_vec	char_vec_pop(t_char_vec *v);
int				char_vec_concat(t_char_vec *v, t_char_vec other);
int				char_vec_reserve(t_char_vec *v, int new_cap);
int				char_vec_insert_one(t_char_vec *v, int pos,
					t_type_char_vec value);
int				char_vec_resize(t_char_vec *v, int new_size,
					t_type_char_vec value);
t_type_char_vec	char_vec_erase_one(t_char_vec *v, int pos);
int				char_vec_cpy(t_char_vec src, t_char_vec *dst);
t_char_vec		char_vec_init(void);

#endif
