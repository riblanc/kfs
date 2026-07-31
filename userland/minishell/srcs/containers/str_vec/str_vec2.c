/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_vec2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:02:12 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/07 13:13:15 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "str_vec.h"

int	str_vec_cpy(t_str_vec src, t_str_vec *dst)
{
	int	i;

	if (src.size == 0)
	{
		*dst = str_vec_init();
		return (1);
	}
	if (str_vec_reserve(dst, src.size) == 0)
		return (0);
	i = 0;
	while (i < src.size)
	{
		dst->data[i] = src.data[i];
		i++;
	}
	dst->size = src.size;
	return (1);
}

int	str_vec_insert_one(t_str_vec *v, int pos, t_type_str_vec value)
{
	int	i;

	if (v->size == v->capacity)
	{
		if (v->capacity == 0 && str_vec_reserve(v, 1) == 0)
			return (0);
		else if (str_vec_reserve(v, v->capacity * 2) == 0)
			return (0);
	}
	i = v->size;
	while (i > pos)
	{
		v->data[i] = v->data[i - 1];
		--i;
	}
	v->data[i] = value;
	v->size++;
	return (1);
}

int	str_vec_resize(t_str_vec *v, int new_size, t_type_str_vec value)
{
	int	i;

	if (new_size < 0)
		return (1);
	else if (new_size <= v->size)
		v->size = new_size;
	else
	{
		if (new_size > v->capacity)
		{
			if (new_size < v->capacity * 2
				&& !str_vec_reserve(v, v->capacity * 2))
				return (0);
			else if (new_size >= v->capacity * 2
				&& !str_vec_reserve(v, new_size))
				return (0);
		}
		i = new_size;
		while (--i >= v->size)
			v->data[i] = value;
		v->size = new_size;
	}
	return (1);
}

t_type_str_vec	str_vec_erase_one(t_str_vec *v, int pos)
{
	t_type_str_vec	ret;

	ret = v->data[pos];
	--v->size;
	while (pos < v->size)
	{
		v->data[pos] = v->data[pos + 1];
		pos++;
	}
	return (ret);
}

void	free_vec(t_str_vec *v)
{
	while (--v->size >= 0)
		free(v->data[v->size]);
	free(v->data);
	v->size = 0;
	v->data = NULL;
	v->capacity = 0;
}
