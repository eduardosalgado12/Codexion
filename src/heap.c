/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/16 15:22:10 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/17 12:47:41 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_before(t_request a, t_request b)
{
	if (a.priority != b.priority)
		return (a.priority < b.priority);
	return (a.coder_id < b.coder_id);
}

int	heap_push(t_heap *heap, t_request req)
{
	int			i;
	int			parent;
	t_request	tmp;

	if (heap->size >= QUEUE_MAX)
		return (0);
	i = heap->size;
	heap->items[i] = req;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (is_before(heap->items[parent], heap->items[i]))
			return (1);
		tmp = heap->items[parent];
		heap->items[parent] = heap->items[i];
		heap->items[i] = tmp;
		i = parent;
	}
	return (1);
}

int	heap_pop(t_heap *heap, t_request *out)
{
	int			i;
	int			child;
	t_request	tmp;

	if (heap->size == 0)
		return (0);
	*out = heap->items[0];
	heap->size--;
	heap->items[0] = heap->items[heap->size];
	i = 0;
	while (2 * i + 1 < heap->size)
	{
		child = (2 * i + 1);
		if (child + 1 < heap->size && is_before(heap->items[child + 1],
				heap->items[child]))
			child = child + 1;
		if (is_before(heap->items[i], heap->items[child]))
			return (1);
		tmp = heap->items[child];
		heap->items[child] = heap->items[i];
		heap->items[i] = tmp;
		i = child;
	}
	return (1);
}
