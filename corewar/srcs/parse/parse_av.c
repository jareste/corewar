/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:24:26 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/08 23:47:53 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "libft.h"
#include "../corewar.h"
#include "ft_printf.h"
#include "../process/process.h"
#include "../decode/decode.h"
#include "parse_internal.h"

int	is_cor_file(char *s)
{
	int	len;

	len = ft_strlen(s);
	if (len < 5)
		return (0);
	return (ft_strncmp(s + len - 4, ".cor", 4) == 0);
}

int	is_number_str(char *s)
{
	int	i;

	if (!s || !s[0])
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

void	m_count_champs(t_vm *vm, char **av, int ac)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (i < ac)
	{
		if (is_cor_file(av[i]))
			count++;
		i++;
	}
	if (count > MAX_PLAYERS)
	{
		ft_dprintf(2, "Error: Too many champions provided: %d\n", count);
		exit(1);
	}
	if (count == 0)
	{
		ft_dprintf(2, "Error: No champion files provided\n");
		exit(1);
	}
	vm->champs_number = count;
}

int	parse_av(t_vm *vm, char **av, int ac, bool *bonus)
{
	int	i;

	i = 0;
	*bonus = false;
	m_count_champs(vm, av, ac);
	while (i < ac)
	{
		if (ft_strncmp(av[i], "-b", 2) == 0 && av[i][2] == '\0')
		{
			*bonus = true;
			i += 1;
		}
		else
			handle_av(vm, av, ac, &i);
	}
	if (vm->champ_count == 0)
	{
		ft_dprintf(2, "No champions provided\n");
		return (-1);
	}
	return (0);
}
