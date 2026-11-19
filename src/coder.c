/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 15:05:38 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/19 19:01:01 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	*single_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->dongle_mutex);
	print_status(coder, "has taken a dongle");
	usleep(coder->sim->params.t_burnout * 1000);
	pthread_mutex_unlock(&coder->left->dongle_mutex);
	return (NULL);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->left == coder->right)
		return (single_coder(coder));
	if (coder->id % 2 == 0)
		usleep(1000);
	while (!is_stopped(coder->sim))
	{
		take_dongles(coder);
		pthread_mutex_lock(&coder->state_mutex);
		coder->last_compile_start = get_time_ms();
		pthread_mutex_unlock(&coder->state_mutex);
		print_start_compile(coder);
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
