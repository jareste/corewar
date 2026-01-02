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
#include "ft_printf.h"
#include "../corewar.h"
#include "../process/process.h"
#include "../decode/decode.h"
#include "parse_internal.h"

int	parse_int_strict(char *s, int *out)
{
	long	val;
	int		sign;
	int		i;

	if (!is_number_str(s))
		return (-1);
	sign = 1;
	i = 0;
	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	val = 0;
	while (s[i])
	{
		val = val * 10 + (s[i] - '0');
		if (val > 2147483647L)
			return (-1);
		i++;
	}
	*out = (int)(val * sign);
	return (0);
}

int	id_used(t_vm *vm, int id)
{
	int	i;

	i = 0;
	while (i < MAX_PLAYERS)
	{
		if (vm->champs[i].id == id)
			return (1);
		i++;
	}
	return (0);
}

int	find_free_slot(t_vm *vm)
{
	int	i;

	i = 0;
	while (i < MAX_PLAYERS)
	{
		if (vm->champs[i].id == 0)
			return (i);
		i++;
	}
	ft_dprintf(2, "No free champion slot left\n");
	exit(1);
}

int	find_free_id(t_vm *vm)
{
	int	id;

	id = 1;
	while (id <= MAX_PLAYERS)
	{
		if (!id_used(vm, id))
			return (id);
		id++;
	}
	return (-1);
}

void	load_champ_code(t_vm *vm, t_champ *champ)
{
	t_proc	*proc;
	int		offset;

	offset = (MEM_SIZE / MAX_PLAYERS) * (champ->id - 1);
	ft_memcpy(&vm->memory[offset], champ->code, champ->size);
	proc = create_process(new_pid(), offset, champ->id - 1);
	log_msg(LOG_I, "Adding %p at offset %p\n", vm->procs, &vm->procs);
	printf("Creating process %d for champion %d at pc %d\n",
		proc->id, champ->id, offset);
	ft_list_add_first((void **)&vm->procs, (void *)proc);
	vm->last_alive_player = champ->id - 1;
	ft_memcpy(vm->la_name, champ->name, PROG_NAME_LENGTH + 1);
	log_msg(LOG_I, "Loaded champion[%d] '%s' at offset %d\n",
		champ->id, champ->name, offset);
}
