/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 20:13:15 by lvargas-          #+#    #+#             */
/*   Updated: 2026/01/08 11:46:26 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor_routine(void *arg)
{
	t_arg	*rules;
	t_philo	*all;
	int		n;

	rules = (t_arg *)arg;
	all = rules->philos;
	if (rules->n_of_philo == 1)
		return (NULL);
	while (1)
	{
		n = 0;
		check_eating_times(rules->philos);
		while (n < rules->n_of_philo)
		{
			if (check_death(&all[n]) == 1)
				return (NULL);
			n++;
		}
		if (get_stop(rules))
			return (NULL);
		sleep_ms(1);
	}
	return (NULL);
}

void	routine_loop(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	while (!get_stop(p->rules) && p->rules->n_of_philo > 1)
	{
		eating(philo);
		if (get_stop(p->rules))
			break ;
		sleeping(philo);
		if (get_stop(p->rules))
			break ;
		thinking(philo);
		if (get_stop(p->rules))
			break ;
		if (p->rules->n_of_philo % 2 != 2)
			sleep_ms(1);
	}
}

void	*philo_routine(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	pthread_mutex_lock(&p->last_meal_mutex);
	p->last_meal = get_time_ms();
	pthread_mutex_unlock(&p->last_meal_mutex);
	if (p->rules->n_of_philo == 1)
	{
		one_philo(philo);
		return (NULL);
	}
	if (p->id % 2 == 0)
		sleep_ms(p->rules->time_to_eat);
	routine_loop(philo);
	return (NULL);
}

int	main(int argc, char *argv[])
{
	t_arg		rules;
	t_philo		*philos;
	pthread_t	monitor;

	if (check_parsing(argc, argv) != 0)
		return (1);
	rules = save_values(argc, argv);
	philos = create_philos(rules.n_of_philo, &rules);
	if (pthread_create(&monitor, NULL, monitor_routine, &rules) != 0)
	{
		pthread_mutex_lock(&rules.stop_mutex);
		rules.stop_flag = 1;
		pthread_mutex_unlock(&rules.stop_mutex);
	}
	pthread_join(monitor, NULL);
	free_and_join(philos, &rules);
	return (0);
}
