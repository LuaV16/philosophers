/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 12:26:21 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/12 12:01:03 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_mutex(t_philo *philos, t_arg *rules)
{
	int	n;

	rules->forks = malloc(rules->number_of_philosophers * sizeof *rules->forks);
	if (!rules->forks)
		return (1);
	if (pthread_mutex_init(&rules->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&rules->stop_mutex, NULL) != 0)
		return (1);
	n = 0;
	while (n < rules->number_of_philosophers)
	{
		if (pthread_mutex_init(&rules->forks[n], NULL) != 0)
			return (1);
		if (pthread_mutex_init(&philos[n].last_meal_mutex, NULL) != 0)
			return (1);
		if (pthread_mutex_init(&philos[n].death_mutex, NULL) != 0)
			return (1);
		if (pthread_mutex_init(&philos[n].eat_times_mutex, NULL) != 0)
			return (1);
		n++;
	}
	return (0);
}

void	fill_philos(t_philo *philos, t_arg *rules, int n)
{
	philos[n].id = n + 1;
	philos[n].rules = rules;
	philos[n].last_meal = rules->start_time;
	philos[n].is_dead = 0;
	philos[n].times_that_philo_has_eaten = 0;
}

t_philo	*create_philos(int n_philos, t_arg *rules)
{
	t_philo	*philos;
	int		n;
	int		i;

	philos = malloc(rules->number_of_philosophers * sizeof(*philos));
	if (!philos)
		return (NULL);
	rules->philos = philos;
	if (init_mutex(philos, rules) == 1)
		return (NULL);
	n = 0;
	while (n < n_philos)
	{
		fill_philos(philos, rules, n);
		if (pthread_create(&philos[n].thread, NULL, philo_routine,
				&philos[n]) != 0)
		{
			i = 0;
			while (i < n)
				pthread_join(philos[i++].thread, NULL);
			return (free(philos), NULL);
		}
		usleep(100);
		n++;
	}
	return (philos);
}

t_arg	save_values(int argc, char *argv[])
{
	t_arg	args;

	ft_atoi_parse(argv[1], &args.number_of_philosophers);
	ft_atoi_parse(argv[2], &args.time_to_die);
	ft_atoi_parse(argv[3], &args.time_to_eat);
	ft_atoi_parse(argv[4], &args.time_to_sleep);
	if (argc > 5)
		ft_atoi_parse(argv[5], &args.number_of_times_each_philosopher_must_eat);
	else
		args.number_of_times_each_philosopher_must_eat = -1;
	//args.start_time = get_time_ms();
	args.stop_flag = 0;
	return (args);
}
