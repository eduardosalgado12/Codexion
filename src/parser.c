/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:58:36 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/03 15:49:07 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "codexion.h"

int	is_valid_number(char *str)
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

int		parse_args(int argc, char **argv, t_params *params)
{
    int     i;
    
    if (argc != 9)
        return (0);
    i = 1;
    while (i < 8)
    {
        if (is_valid_number(argv[i]) == 0)
            return (0);
        i++;
    }
}