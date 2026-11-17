/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/17 15:38:19 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/17 17:06:43 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	request(t_coder *coder, t_dongle *dongle)
{
	t_request	req;

	req.coder_id = coder->id;
	req.ticket = dongle->next_ticket++;
	if (coder->sim->params.scheduler == FIFO)
		req.priority = req.ticket;
	else
		req.priority = coder->last_compile_start
			+ coder->sim->params.t_burnout;
	heap_push(&dongle->queue, req);
}

static void	lock_pair(t_coder *coder)
{
	if (coder->left < coder->right)
	{
		pthread_mutex_lock(&coder->left->dongle_mutex);
		pthread_mutex_lock(&coder->right->dongle_mutex);
	}
	else
	{
		pthread_mutex_lock(&coder->right->dongle_mutex);
		pthread_mutex_lock(&coder->left->dongle_mutex);
	}
}

static int	dongle_ready(t_coder *coder, t_dongle *dongle)
{
	if (dongle->in_use)
		return (0);
	if (dongle->queue.items[0].coder_id != coder->id)
		return (0);
	if (get_time_ms() < dongle->released_at + coder->sim->params.cooldown)
		return (0);
	return (1);
}

void	take_dongles(t_coder *coder)
{
	t_request	req;

	lock_pair(coder);
	request(coder, coder->left);
	request(coder, coder->right);
	while (!(dongle_ready(coder, coder->left)
			&& dongle_ready(coder, coder->right)))
	{
		pthread_mutex_unlock(&coder->left->dongle_mutex);
	    pthread_mutex_unlock(&coder->right->dongle_mutex);
		usleep(500);
		lock_pair(coder);
	}
	heap_pop(&coder->left->queue, &req);
	heap_pop(&coder->right->queue, &req);
	coder->left->in_use = 1;
	coder->right->in_use = 1;
	pthread_mutex_unlock(&coder->left->dongle_mutex);
	pthread_mutex_unlock(&coder->right->dongle_mutex);
}

void	release_dongles(t_coder *coder)
{
	long	now;

	now = get_time_ms();
	lock_pair(coder);
	coder->left->in_use = 0;
	coder->left->released_at = now;
	coder->right->in_use = 0;
	coder->right->released_at = now;
	pthread_mutex_unlock(&coder->left->dongle_mutex);
	pthread_mutex_unlock(&coder->right->dongle_mutex);
}