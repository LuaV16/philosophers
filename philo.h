/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 20:47:01 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/12 11:15:28 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# ifndef MAX_PHILOS
#  define MAX_PHILOS 200
# endif

# define RESET "\x1b[0m"
# define RED "\x1b[31m"
# define GREEN "\x1b[32m"
# define YELLOW "\x1b[33m"
# define BLUE "\x1b[34m"
# define DARK_PURPLE "\x1b[38;5;90m"

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_philo	t_philo;

typedef struct s_arg
{
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	int				stop_flag;
	long long		start_time;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	*forks;
	pthread_mutex_t	stop_mutex;
	t_philo *philos;
}					t_arg;

typedef struct s_philo
{
	t_arg			*rules;
	pthread_t		thread;
	long long		last_meal;
	pthread_mutex_t	last_meal_mutex;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	eat_times_mutex;
	int				id;
	int				is_dead;
	int				times_that_philo_has_eaten;
}					t_philo;

int					ft_isdigit(int c);
int					ft_atoi_parse(const char *nptr, int *out);
int					check_parsing(int argc, char *argv[]);
int get_stop(t_arg *rules);
long long get_time_ms(void);
long long get_timestamp(long long start_time_ms);
void *philo_routine(void *philo);
t_philo	*create_philos(int n_philos, t_arg *rules);
t_arg	save_values(int argc, char *argv[]);

#endif