/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 13:29:03 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/10 17:15:04 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void start_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.n_coders)
	{
		pthread_create(&sim->coders[i].thread, NULL,
			coder_routine, &sim->coders[i]);
		i++;
	}
	pthread_create(&sim->monitor, NULL, monitor_routine, sim);
}

static void join_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.n_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (parse_args(argc, argv, &sim.params) == 0)
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (init_sim(&sim) == 0)
	{
		fprintf(stderr, "Error: initialization failed\n");
		return (1);
	}
	start_threads(&sim);
	join_threads(&sim);
	cleanup(&sim);
	return (0);
}
