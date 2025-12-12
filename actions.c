/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:28:23 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/12 16:29:44 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_fork(t_philo *philo)
{
	t_arg	*rules;
	int		left;
	int		right;

	rules = philo->rules;
	left = philo->id - 1;
	right = philo->id % rules->number_of_philosophers;
	if (philo->id % 2 != 0)
	{
		pthread_mutex_lock(&rules->forks[right]);
		print_state(rules, philo->id, GREEN, "has taken a fork");
		pthread_mutex_lock(&rules->forks[left]);
		print_state(rules, philo->id, GREEN, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(&rules->forks[left]);
		print_state(rules, philo->id, GREEN, "has taken a fork");
		pthread_mutex_lock(&rules->forks[right]);
		print_state(rules, philo->id, GREEN, "has taken a fork");
	}
}

void	eating(t_philo *philo)
{
	t_arg	*rules;
	int		left;
	int		right;

	rules = philo->rules;
	left = philo->id - 1;
	right = philo->id % rules->number_of_philosophers;
	pthread_mutex_lock(&philo->last_meal_mutex);
	philo->last_meal = get_time_ms();
	pthread_mutex_unlock(&philo->last_meal_mutex);
	print_state(rules, philo->id, YELLOW, "is eating");
	pthread_mutex_lock(&philo->eat_times_mutex);
	philo->times_that_philo_has_eaten += 1;
	pthread_mutex_unlock(&philo->eat_times_mutex);
	check_eating_times(philo);
	sleep_ms(rules->time_to_eat);
	pthread_mutex_unlock(&rules->forks[left]);
	pthread_mutex_unlock(&rules->forks[right]);
}

void	sleeping(t_philo *philo)
{
	t_arg	*rules;

	rules = philo->rules;
	print_state(rules, philo->id, BLUE, "is sleeping");
	sleep_ms(rules->time_to_sleep);
}

void	thinking(t_philo *philo)
{
	t_arg	*rules;

	rules = philo->rules;
	print_state(rules, philo->id, RED, "is thinking");
}
