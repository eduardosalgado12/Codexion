/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:58:36 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/10 14:40:30 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_number(char *str)
{
	int		i;
	long	value;

	if (str[0] == '\0')
		return (0);
	i = 0;
	value = 0;
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		value = value * 10 + (str[i] - '0');
		if (value > 2147483647)
			return (0);
		i++;
	}
	return (1);
}

static int	check_numbers(char **argv)
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if (is_valid_number(argv[i]) == 0)
			return (0);
		i++;
	}
	return (1);
}

static int	parse_scheduler(char *str, t_params *params)
{
	if (strcmp(str, "fifo") == 0)
	{
		params->scheduler = FIFO;
		return (1);
	}
	else if (strcmp(str, "edf") == 0)
	{
		params->scheduler = EDF;
		return (1);
	}
	return (0);
}

int	parse_args(int argc, char **argv, t_params *params)
{
	if (argc != 9)
		return (0);
	if (check_numbers(argv) == 0)
		return (0);
	params->n_coders = atoi(argv[1]);
	params->t_burnout = atoi(argv[2]);
	params->t_compile = atoi(argv[3]);
	params->t_debug = atoi(argv[4]);
	params->t_refactor = atoi(argv[5]);
	params->n_compiles = atoi(argv[6]);
	params->cooldown = atoi(argv[7]);
	if (parse_scheduler(argv[8], params) == 0)
		return (0);
	if (params->n_coders == 0)
		return (0);
	return (1);
}

void	cleanup(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.n_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
		i++;
	}
	free(sim->dongles);
	free(sim->coders);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->print_mutex);
}
