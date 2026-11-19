/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 14:10:04 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/17 17:13:29 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
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

int	is_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	print_start_compile(t_coder *coder)
{
	long	t;

	pthread_mutex_lock(&coder->sim->print_mutex);
	if (!is_stopped(coder->sim))
	{
		t = get_time_ms() - coder->sim->start_time;
		printf("%ld %d has taken a dongle\n", t, coder->id);
		printf("%ld %d has taken a dongle\n", t, coder->id);
		printf("%ld %d is compiling\n", t, coder->id);
	}
	pthread_mutex_unlock(&coder->sim->print_mutex);
}