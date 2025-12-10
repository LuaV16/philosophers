/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvargas- <lvargas-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 20:13:15 by lvargas-          #+#    #+#             */
/*   Updated: 2025/12/10 13:25:23 by lvargas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void ms_sleep(long long ms, t_arg *rules)
{
    long long    end;

    end = get_time_ms() + ms;
    while (!get_stop(rules) && get_time_ms() < end)
        usleep(100);
}

void check_eating_times(t_philo *philo)
{
    t_arg *rules;
    t_philo *first_philo;
    int n;
    int check;

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

void print_state(t_arg *rules, int id, const char *color, const char *msg)
{
    pthread_mutex_lock(&rules->stop_mutex);
    if (rules->stop_flag)
    {
        pthread_mutex_unlock(&rules->stop_mutex);
        return ;
    }
    pthread_mutex_lock(&rules->print_mutex);
    printf("%s[%lld] Philosopher %i %s\n" RESET,
           color, get_timestamp(rules->start_time), id, msg);
    pthread_mutex_unlock(&rules->print_mutex);
    pthread_mutex_unlock(&rules->stop_mutex);
}

int check_death(long long actual_time, t_philo *philo)
{
    long long ms_since_last_meal;

    pthread_mutex_lock(&philo->last_meal_mutex);
    ms_since_last_meal = actual_time - philo->last_meal;
    pthread_mutex_unlock(&philo->last_meal_mutex);
    if (ms_since_last_meal > philo->rules->time_to_die)
    {
        pthread_mutex_lock(&philo->rules->stop_mutex);
        if (philo->rules->stop_flag == 0)
        {
            philo->rules->stop_flag = 1;
            pthread_mutex_lock(&philo->rules->print_mutex);
            printf(DARK_PURPLE "[%lld] Philosopher %i died\n", get_timestamp(philo->rules->start_time), philo->id);
            pthread_mutex_unlock(&philo->rules->print_mutex);
        }
        pthread_mutex_unlock(&philo->rules->stop_mutex);
        return (1);
    }
    return (0);
}

void eating(t_philo *philo)
{
    t_arg *rules;
    int left;
    int right;

    rules = philo->rules;
    left = philo->id - 1;
    right = philo->id % rules->number_of_philosophers;
    if (rules->number_of_philosophers == 1)
    {
        print_state(rules, philo->id, YELLOW, "has taken a fork");
        ms_sleep(rules->time_to_die, rules);
        return ;
    }

    /* waiter: limit forks takers to available > 0 */
    pthread_mutex_lock(&rules->waiter_mutex);
    while (rules->available <= 0)
    {
        pthread_mutex_unlock(&rules->waiter_mutex);
        if (get_stop(rules))
            return ;
        usleep(1000);
        pthread_mutex_lock(&rules->waiter_mutex);
    }
    rules->available--;
    pthread_mutex_unlock(&rules->waiter_mutex);

    if (philo->id % 2 == 0)
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

    pthread_mutex_lock(&philo->last_meal_mutex);
    philo->last_meal = get_time_ms();
    pthread_mutex_unlock(&philo->last_meal_mutex);
    pthread_mutex_lock(&philo->order_mutex);
    philo->order = 2;
    pthread_mutex_unlock(&philo->order_mutex);
    print_state(rules, philo->id, YELLOW, "is eating");
    pthread_mutex_lock(&philo->eat_times_mutex);
    philo->times_that_philo_has_eaten += 1;
    pthread_mutex_unlock(&philo->eat_times_mutex);
    check_eating_times(philo);
    ms_sleep(rules->time_to_eat, rules);

    pthread_mutex_unlock(&rules->forks[left]);
    pthread_mutex_unlock(&rules->forks[right]);

    /* release waiter slot */
    pthread_mutex_lock(&rules->waiter_mutex);
    rules->available++;
    pthread_mutex_unlock(&rules->waiter_mutex);
}

void sleeping(t_philo *philo)
{
    t_arg *rules;

    rules = philo->rules;
    pthread_mutex_lock(&philo->order_mutex);
    philo->order = 3;
    pthread_mutex_unlock(&philo->order_mutex);
    print_state(rules, philo->id, BLUE, "is sleeping");
    ms_sleep(philo->rules->time_to_sleep, philo->rules);
}

void thinking(t_philo *philo)
{
    t_arg *rules;

    rules = philo->rules;
    pthread_mutex_lock(&philo->order_mutex);
    philo->order = 1;
    pthread_mutex_unlock(&philo->order_mutex);
    print_state(rules, philo->id, RED, "is thinking");
}

void *monitor_routine(void *arg)
{
    t_arg *rules;
    t_philo *all;
    int n;

    rules = (t_arg *)arg;
    all = rules->philos;
    while (!get_stop(rules))
    {
        n = 0;
        check_eating_times(rules->philos);
        while (n < rules->number_of_philosophers)
        {
            if (check_death(get_time_ms(), &all[n]) == 1)
                return (NULL);
            n++;
        }
        usleep(1000);
    }
    return (NULL);
}

void *philo_routine(void *philo)
{
    t_philo *p;

    p = (t_philo *)philo;
    pthread_mutex_lock(&p->last_meal_mutex);
    p->last_meal = get_time_ms();
    pthread_mutex_unlock(&p->last_meal_mutex);
    if (p->id % 2 == 0)
        usleep(1000);
    while (!get_stop(p->rules))
    {
        if (p->order == 1)
            eating(philo);
        if (get_stop(p->rules)) break;
        if (p->order == 2)
            sleeping(philo);
        if (get_stop(p->rules)) break;
        if (p->order == 3)
            thinking(philo);
        if (get_stop(p->rules)) break;
    }
    return (NULL);
}

void free_and_join(t_philo *philos, t_arg *rules)
{
    int n;

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
        pthread_mutex_destroy(&philos[n].order_mutex);
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
    pthread_mutex_destroy(&rules->waiter_mutex);
    free(philos);
}

int main(int argc, char *argv[])
{
    t_arg rules;
    t_philo *philos;
    pthread_t monitor;

    check_parsing(argc, argv);
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