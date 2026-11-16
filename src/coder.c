/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 15:05:38 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/16 14:17:02 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	take_one(t_coder *coder, t_dongle *dongle)
{
	long	wait;

	pthread_mutex_lock(&dongle->mutex);
	wait = dongle->released_at + coder->sim->params.cooldown;
	wait = wait - get_time_ms();
	if (wait > 0)
		usleep(wait * 1000);
	print_status(coder, "has taken a dongle");
}

static void	take_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		take_one(coder, coder->right);
		take_one(coder, coder->left);
	}
	else
	{
		take_one(coder, coder->left);
		take_one(coder, coder->right);
	}
}

static void	release_dongles(t_coder *coder)
{
	long	now;

	now = get_time_ms();
	coder->left->released_at = now;
	coder->right->released_at = now;
	pthread_mutex_unlock(&coder->left->mutex);
	pthread_mutex_unlock(&coder->right->mutex);
}

static void	*single_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->mutex);
	print_status(coder, "has taken a dongle");
	usleep(coder->sim->params.t_burnout * 1000);
	pthread_mutex_unlock(&coder->left->mutex);
	return (NULL);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->left == coder->right)
		return (single_coder(coder));
	while (!is_stopped(coder->sim))
	{
		take_dongles(coder);
		pthread_mutex_lock(&coder->state_mutex);
		coder->last_compile_start = get_time_ms();
		pthread_mutex_unlock(&coder->state_mutex);
		print_status(coder, "is compiling");
		usleep(coder->sim->params.t_compile * 1000);
		pthread_mutex_lock(&coder->state_mutex);
		coder->compile_count++;
		pthread_mutex_unlock(&coder->state_mutex);
		release_dongles(coder);
		print_status(coder, "is debugging");
		usleep(coder->sim->params.t_debug * 1000);
		print_status(coder, "is refactoring");
		usleep(coder->sim->params.t_refactor * 1000);
	}
	return (NULL);
}
