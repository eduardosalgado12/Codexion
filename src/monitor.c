/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 17:04:16 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/10 17:36:31 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;
	long	last;

	sim = (t_sim *)arg;
	while (1)
	{
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
                    get_time_ms() - sim->start_time, sim->coders[i].id);
                pthread_mutex_unlock(&sim->print_mutex);
                return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
