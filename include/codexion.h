/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 11:40:40 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/09 15:24:12 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>	// threads, mutexes, condition variables
# include <stdio.h>		// printf, fprintf
# include <stdlib.h>	// malloc, free, atoi
# include <string.h>	// strcmp, strlen, memset
# include <unistd.h>	// usleep, write
# include <sys/time.h>	// gettimeofday (falta-te este)

# define FIFO 0
# define EDF 1
# define QUEUE_MAX 2

typedef struct s_params
{
	int			n_coders;
	long		t_burnout;
	long		t_compile;
	long		t_debug;
	long		t_refactor;
	int			n_compiles;
	long		cooldown;
	int			scheduler;
}				t_params;

typedef struct s_request
{
	int			coder_id;
	long		priority; // fifo: nº de chegada | edf: deadline
}				t_request;

typedef struct s_heap
{
	t_request	items[QUEUE_MAX];
	int			size;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t		mutex; // protege todos os campos abaixo
	int					in_use; // 1 se alguém o tem na mão
	long				released_at; // quando foi largado (para o cooldown)
	pthread_cond_t		cond; // para acordar quem está à espera
	t_heap				queue; // pedidos à espera
	long				next_ticket; // contador para fifo (0, 1, 2, 3…)
}				t_dongle;

typedef struct s_coder
{
	int				id; // 1 a n_coders
	pthread_t		thread;
	t_dongle		*left;
	t_dongle		*right;
	long			last_compile_start; // lido pelo monitor!
	int				compile_count; // lido pelo monitor!
	pthread_mutex_t	state_mutex; // protege os dois campos acima
	struct s_sim	*sim; // ponteiro para o estado global
}				t_coder;

typedef struct s_sim
{
	t_params		params;
	t_coder			*coders; // array de n_coders
	t_dongle		*dongles; // array de n_coders
	long			start_time; // em ms
	int				stop; // 1 quando a simulação acaba
	pthread_mutex_t	stop_mutex; // protege stop
	pthread_mutex_t	print_mutex; // protege os printf
	pthread_t		monitor;
}				t_sim;

int		parse_args(int argc, char **argv, t_params *params);
void	cleanup(t_sim *sim);
int		init_sim(t_sim *sim);

#endif