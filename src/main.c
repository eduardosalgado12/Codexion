/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 13:29:03 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/09 15:24:20 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
	cleanup(&sim);
	return (0);
}
