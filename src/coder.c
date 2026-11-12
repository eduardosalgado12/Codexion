/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 15:05:38 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/12 16:53:24 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	print_status(t_coder *coder, char *msg)
{
	pthread_mutex_lock(&coder->sim->print_mutex);
	if (!is_stopped(coder->sim))
		printf("%ld %d %s\n",
			get_time_ms() - coder->sim->start_time,
			coder->id,
			msg);
	pthread_mutex_unlock(&coder->sim->print_mutex);
}

static void	take_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		pthread_mutex_lock(&coder->right->mutex);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->left->mutex);
		print_status(coder, "has taken a dongle");
	}
	else
	{
		pthread_mutex_lock(&coder->left->mutex);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->right->mutex);
		print_status(coder, "has taken a dongle");
	}
}

static void	release_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left->mutex);
	pthread_mutex_unlock(&coder->right->mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->left == coder->right)
	{
		pthread_mutex_lock(&coder->left->mutex);
		print_status(coder, "has taken a dongle");
		usleep(coder->sim->params.t_burnout * 1000);
		pthread_mutex_unlock(&coder->left->mutex);
		return (NULL);
	}
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
