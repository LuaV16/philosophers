/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 00:25:21 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/01 00:44:09 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_all_args(char *argv[])
{
	int	n;
	int	number;
	int	error;

	n = 1;
	while (argv[n])
	{
		error = ft_atoi_parse(argv[n], &number);
		if (error != 0)
			return (error);
		n++;
	}
	return (0);
}

void	print_error(int n)
{
	if (n == 1)
		printf(RED "Oops! You need 4 or 5 arguments!\n");
	else if (n == 2)
		printf(RED "Oops! One of the numbers is too big!\n");
	else if (n == 3)
		printf(RED "Oops! Argument format not valid!\n");
	else if (n == 4)
		printf(RED "Oops! All numbers must be greater than 0!\n");
	else if (n == 5)
		printf(RED "Oops! Number of philosophers must be between 1 and %d!\n",
			MAX_PHILOS);
}

int	check_parsing(int argc, char *argv[])
{
	int	error;
	int	number;

	if (argc < 5 || argc > 6)
		return (print_error(1), 1);
	error = check_all_args(argv);
	if (error == 1)
		return (print_error(2), 1);
	if (error == 2)
		return (print_error(3), 1);
	else if (error == 3)
		return (print_error(4), 1);
	ft_atoi_parse(argv[1], &number);
	if (number > MAX_PHILOS)
		return (print_error(5), 1);
	return (0);
}
