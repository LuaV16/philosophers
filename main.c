/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 20:13:15 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/12 16:31:59 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	check_eating_times(t_philo *philo)
{
	t_arg	*rules;
	t_philo	*first_philo;
	int		n;
	int		check;

	rules = philo->rules;
	if (rules->number_of_times_each_philosopher_must_eat == -1)
		return ;
	check = 0;
	n = 0;
	first_philo = rules->philos;
	while (n < rules->number_of_philosophers)
	{
		pthread_mutex_lock(&first_philo[n].eat_times_mutex);
		if (rules->number_of_times_each_philosopher_must_eat <= first_philo[n].times_that_philo_has_eaten)
			check++;
		pthread_mutex_unlock(&first_philo[n].eat_times_mutex);
		n++;
	}
	if (check == rules->number_of_philosophers)
	{
		pthread_mutex_lock(&philo->rules->stop_mutex);
		philo->rules->stop_flag = 1;
		pthread_mutex_unlock(&philo->rules->stop_mutex);
	}
}

void	print_state(t_arg *rules, int id, const char *color, const char *msg)
{
	pthread_mutex_lock(&rules->stop_mutex);
	if (rules->stop_flag)
	{
		pthread_mutex_unlock(&rules->stop_mutex);
		return ;
	}
	pthread_mutex_lock(&rules->print_mutex);
	printf("%s[%lld] Philosopher %i %s\n" RESET, color,
		get_timestamp(rules->start_time), id, msg);
	pthread_mutex_unlock(&rules->print_mutex);
	pthread_mutex_unlock(&rules->stop_mutex);
}

int	check_death(t_philo *philo)
{
	pthread_mutex_lock(&philo->last_meal_mutex);
	if ((get_time_ms() - philo->last_meal) > philo->rules->time_to_die)
	{
		pthread_mutex_lock(&philo->rules->stop_mutex);
		if (philo->rules->stop_flag == 0)
		{
			philo->rules->stop_flag = 1;
			pthread_mutex_lock(&philo->rules->print_mutex);
			printf(DARK_PURPLE "[%lld] Philosopher %i died\n",
				get_timestamp(philo->rules->start_time), philo->id);
			pthread_mutex_unlock(&philo->rules->print_mutex);
		}
		pthread_mutex_unlock(&philo->rules->stop_mutex);
		pthread_mutex_unlock(&philo->last_meal_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->last_meal_mutex);
	return (0);
}

void	one_philo(t_philo *philo)
{
	t_arg	*rules;

	rules = philo->rules;
	pthread_mutex_lock(&rules->forks[0]);
	print_state(rules, philo->id, GREEN, "has taken a fork");
	sleep_ms(rules->time_to_die);
	printf(DARK_PURPLE "[%lld] Philosopher %i died\n",
		get_timestamp(philo->rules->start_time), philo->id);
	pthread_mutex_unlock(&rules->forks[0]);
}

void	*monitor_routine(void *arg)
{
	t_arg	*rules;
	t_philo	*all;
	int		n;

	rules = (t_arg *)arg;
	all = rules->philos;
	if (rules->number_of_philosophers == 1)
		return (NULL);
	while (1)
	{
		n = 0;
		check_eating_times(rules->philos);
		while (n < rules->number_of_philosophers)
		{
			if (check_death(&all[n]) == 1)
				return (NULL);
			n++;
		}
		if (get_stop(rules))
			return (NULL);
		sleep_ms(500);
	}
	return (NULL);
}

void	*philo_routine(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	pthread_mutex_lock(&p->last_meal_mutex);
	p->last_meal = get_time_ms();
	pthread_mutex_unlock(&p->last_meal_mutex);
	p->rules->start_time = get_time_ms();
	if (p->rules->number_of_philosophers == 1)
	{
		one_philo(philo);
		return (NULL);
	}
	if (p->id % 2 == 0)
		sleep_ms(p->rules->time_to_eat);
	while (!get_stop(p->rules) && p->rules->number_of_philosophers > 1)
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
		if (p->rules->number_of_philosophers % 2 != 2)
			sleep_ms(1);
	}
	return (NULL);
}

void	free_and_join(t_philo *philos, t_arg *rules)
{
	int	n;

	if (!philos || !rules)
		return ;
	n = 0;
	while (n < rules->number_of_philosophers)
	{
		pthread_join(philos[n].thread, NULL);
		n++;
	}
	n = 0;
	while (n < rules->number_of_philosophers)
	{
		pthread_mutex_destroy(&philos[n].death_mutex);
		pthread_mutex_destroy(&philos[n].last_meal_mutex);
		pthread_mutex_destroy(&philos[n].eat_times_mutex);
		n++;
	}
	if (rules->forks)
	{
		n = 0;
		while (n < rules->number_of_philosophers)
		{
			pthread_mutex_destroy(&rules->forks[n]);
			n++;
		}
		free(rules->forks);
		rules->forks = NULL;
	}
	pthread_mutex_destroy(&rules->print_mutex);
	pthread_mutex_destroy(&rules->stop_mutex);
	free(philos);
}

int	main(int argc, char *argv[])
{
	t_arg		rules;
	t_philo		*philos;
	pthread_t	monitor;

	if (check_parsing(argc, argv) != 0)
		return (1);
	rules = save_values(argc, argv);
	philos = create_philos(rules.number_of_philosophers, &rules);
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
