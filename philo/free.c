/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 21:57:31 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/15 23:50:29 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	free_each_philo(t_philo *philos, t_arg *rules)
{
	int	n;

	n = 0;
	while (n < rules->n_of_philo)
	{
		pthread_join(philos[n].thread, NULL);
		n++;
	}
	n = 0;
	while (n < rules->n_of_philo)
	{
		pthread_mutex_destroy(&philos[n].death_mutex);
		pthread_mutex_destroy(&philos[n].last_meal_mutex);
		pthread_mutex_destroy(&philos[n].eat_times_mutex);
		n++;
	}
}

void	free_and_join(t_philo *philos, t_arg *rules)
{
	int	n;

	if (!philos || !rules)
		return ;
	free_each_philo(philos, rules);
	if (rules->forks)
	{
		n = 0;
		while (n < rules->n_of_philo)
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
