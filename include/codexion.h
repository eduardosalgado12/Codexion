/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 11:40:40 by edsalgad          #+#    #+#             */
/*   Updated: 2026/11/19 19:00:55 by edsalgad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define FIFO 0
# define EDF 1
# define QUEUE_MAX 2

typedef struct s_params
{
	int				n_coders;
	long			t_burnout;
	long			t_compile;
	long			t_debug;
	long			t_refactor;
	int				n_compiles;
	long			cooldown;
	int				scheduler;
}					t_params;

typedef struct s_request
{
	int				coder_id;
	long			priority;
	long			ticket;
}					t_request;

typedef struct s_heap
{
	t_request		items[QUEUE_MAX];
	int				size;
}					t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	dongle_mutex;
	int				in_use;
	long			released_at;
	t_heap			queue;
	long			next_ticket;
}					t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_dongle		*left;
	t_dongle		*right;
	long			last_compile_start;
	int				compile_count;
	pthread_mutex_t	state_mutex;
	struct s_sim	*sim;
}					t_coder;

typedef struct s_sim
{
	t_params		params;
	t_coder			*coders;
	t_dongle		*dongles;
	long			start_time;
	int				stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
	pthread_t		monitor;
}					t_sim;

int					parse_args(int argc, char **argv, t_params *params);
void				cleanup(t_sim *sim);
int					init_sim(t_sim *sim);
long				get_time_ms(void);
void				*coder_routine(void *arg);
void				print_status(t_coder *coder, char *msg);
void				*monitor_routine(void *arg);
int					is_stopped(t_sim *sim);
int					heap_push(t_heap *heap, t_request req);
int					heap_pop(t_heap *heap, t_request *out);
void				take_dongles(t_coder *coder);
void				print_start_compile(t_coder *coder);
void				release_dongles(t_coder *coder);

#endif
