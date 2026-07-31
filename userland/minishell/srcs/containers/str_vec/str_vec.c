/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_vec.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgiron <jgiron@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 11:02:12 by jgiron            #+#    #+#             */
/*   Updated: 2022/03/07 13:13:15 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "str_vec.h"

t_str_vec	str_vec_init(void)
{
	return ((t_str_vec){});
}

int	str_vec_reserve(t_str_vec *v, int new_cap)
{
	t_type_str_vec	*tmp;
	int				i;

	if (new_cap < v->capacity)
		return (1);
	tmp = malloc(sizeof(t_type_str_vec) * new_cap);
	if (!tmp)
		return (0);
	i = 0;
	while (i < v->size)
	{
		tmp[i] = v->data[i];
		++i;
	}
	free(v->data);
	v->data = tmp;
	v->capacity = new_cap;
	return (1);
}

int	str_vec_push(t_str_vec *v, t_type_str_vec value)
{
	if (v->size >= v->capacity)
	{
		if (v->capacity == 0 && str_vec_reserve(v, 1) == 0)
			return (0);
		else if (str_vec_reserve(v, v->capacity * 2) == 0)
			return (0);
	}
	v->data[v->size] = value;
	v->size++;
	return (1);
}

int	str_vec_concat(t_str_vec *v, t_str_vec other)
{
	int	i;

	if (v->size + other.size > v->capacity)
	{
		if (v->capacity == 0 && str_vec_reserve(v, 1) == 0)
			return (0);
		else if (str_vec_reserve(v, (v->size + other.size) * 2) == 0)
			return (0);
	}
	i = 0;
	while (i < other.size)
	{
		v->data[v->size + i] = other.data[i];
		i++;
	}
	v->size += other.size;
	return (1);
}

t_type_str_vec	str_vec_pop(t_str_vec *v)
{
	if (v->size)
	{
		v->size--;
		return (v->data[v->size]);
	}
	return (0);
}
