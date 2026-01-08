/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 21:40:40 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/15 23:30:06 by lvargas-         ###   ########.fr       */
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
	if (rules->n_must_eat == -1)
		return ;
	check = 0;
	n = 0;
	first_philo = rules->philos;
	while (n < rules->n_of_philo)
	{
		pthread_mutex_lock(&first_philo[n].eat_times_mutex);
		if (rules->n_must_eat <= first_philo[n].times_that_philo_has_eaten)
			check++;
		pthread_mutex_unlock(&first_philo[n].eat_times_mutex);
		n++;
	}
	if (check == rules->n_of_philo)
	{
		pthread_mutex_lock(&philo->rules->stop_mutex);
		philo->rules->stop_flag = 1;
		pthread_mutex_unlock(&philo->rules->stop_mutex);
	}
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
