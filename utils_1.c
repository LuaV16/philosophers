/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 21:16:15 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/12 16:31:40 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	return (0);
}

int	ft_atoi_parse(const char *nptr, int *out)
{
	int	n;
	int	num;

	n = 0;
	while (*nptr)
	{
		if (!ft_isdigit(*nptr))
			return (2);
		num = *nptr - '0';
		if (n > (INT_MAX - num) / 10)
			return (1);
		n = n * 10 + num;
		nptr++;
	}
	if (n <= 0)
		return (3);
	*out = n;
	return (0);
}

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long long	get_timestamp(long long start_time_ms)
{
	return (get_time_ms() - start_time_ms);
}

int	get_stop(t_arg *rules)
{
	int	s;

	pthread_mutex_lock(&rules->stop_mutex);
	s = rules->stop_flag;
	pthread_mutex_unlock(&rules->stop_mutex);
	return (s);
}
