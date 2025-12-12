/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:31:46 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/12 16:32:14 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	sleep_ms(int time_ms)
{
	long	start;
	long	time_diff;

	start = get_time_ms();
	while (1)
	{
		time_diff = get_time_ms() - start;
		if (time_diff >= (long)time_ms)
			break ;
		if ((get_time_ms() - time_diff) > 5)
			usleep(2000);
		else
			usleep(500);
	}
}
