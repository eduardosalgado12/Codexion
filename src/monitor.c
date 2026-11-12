/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 17:04:16 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/12 16:47:37 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	all_compiled(t_sim *sim)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (i < sim->params.n_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (count < sim->params.n_compiles)
			return (0);
		i++;
	}
	return (1);
}

static int	check_burnouts(t_sim *sim)
{
	int		i;
	long	last;

	i = 0;
	while (i < sim->params.n_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		last = sim->coders[i].last_compile_start;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (get_time_ms() - last > sim->params.t_burnout)
		{
			pthread_mutex_lock(&sim->print_mutex);
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop = 1;
			pthread_mutex_unlock(&sim->stop_mutex);
			printf("%ld %d burned out\n",
					get_time_ms() - sim->start_time,
					sim->coders[i].id);
			pthread_mutex_unlock(&sim->print_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (1)
	{
		if (check_burnouts(sim))
			return (NULL);
		if (all_compiled(sim))
		{
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop = 1;
			pthread_mutex_unlock(&sim->stop_mutex);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
