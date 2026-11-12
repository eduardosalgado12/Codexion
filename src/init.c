/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 12:15:45 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/12 16:47:36 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->params.n_coders);
	if (sim->dongles == NULL)
		return (0);
	i = 0;
	while (i < sim->params.n_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			return (0);
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
			return (0);
		sim->dongles[i].in_use = 0;
		sim->dongles[i].released_at = 0;
		sim->dongles[i].next_ticket = 0;
		sim->dongles[i].queue.size = 0;
		i++;
	}
	return (1);
}

static int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->params.n_coders);
	if (sim->coders == NULL)
		return (0);
	i = 0;
	while (i < sim->params.n_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->params.n_coders];
		sim->coders[i].compile_count = 0;
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
			return (0);
		sim->coders[i].sim = sim;
		i++;
	}
	return (1);
}

int	init_sim(t_sim *sim)
{
	int	i;

	sim->stop = 0;
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
		return (0);
	if (init_dongles(sim) == 0)
		return (0);
	if (init_coders(sim) == 0)
		return (0);
	sim->start_time = get_time_ms();
	i = 0;
	while (i < sim->params.n_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		i++;
	}
	return (1);
}
