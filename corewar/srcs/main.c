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
#include "corewar.h"
#include "ft_printf.h"
#include "decode/decode.h"
#include "process/process.h"

void	init_vm(t_vm *vm)
{
	memset(vm->memory, 0, MEM_SIZE);
	vm->procs = NULL;
	vm->cycle = 0;
	vm->cycle_to_die = CYCLE_TO_DIE;
	vm->last_check_cycle = 0;
	vm->last_alive_player = -1;
}

void	load_champ_into_vm(t_vm *vm, t_champ *champ, int player_id)
{
	t_proc	*proc;
	int		offset;

	offset = (MEM_SIZE / MAX_PLAYERS) * player_id;
	memcpy(&vm->memory[offset], champ->code, champ->size);
	proc = create_process(new_pid(), offset, player_id);
	log_msg(LOG_I, "Adding %p at offset %p\n", vm->procs, &vm->procs);
	ft_list_add_first((void **)&vm->procs, (void *)proc);
	vm->last_alive_player = player_id;
	log_msg(LOG_I, "Loaded champion '%s' at offset %d\n", champ->name, offset);
}

void	m_run(t_vm *vm)
{
	t_proc	*proc;

	while (1)
	{
		vm->cycle++;
		proc = vm->procs;
		while (proc)
		{
			step_proc(vm, proc);
			proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
		}
		if (vm->cycle % vm->cycle_to_die == 0)
		{
			log_msg(LOG_I, "Cycle %d: cycle to die check\n", vm->cycle);
			proc_check_deads(vm);
		}
	}
}

/* should i check for some flag? */
int	m_create_champs_from_av(t_vm *vm, char **av, int ac)
{
	t_champ	champ;
	int		i;

	i = 1;
	while (i < ac)
	{
		if (decode_file(av[i], &champ) != 0)
		{
			ft_dprintf(2, "Error: Failed to decode file %s\n", av[i]);
			return (1);
		}
		load_champ_into_vm(vm, &champ, i - 1);
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_vm	vm;

	if (argc < 2)
	{
		ft_dprintf(2, "Usage: %s <source_file>\n", argv[0]);
		return (1);
	}
	if (log_init() != 0)
	{
		ft_dprintf(2, "Error: Could not initialize logging system\n");
		return (1);
	}
	init_vm(&vm);
	if (m_create_champs_from_av(&vm, argv, argc) != 0)
	{
		log_close();
		return (1);
	}
	m_run(&vm);
	return (0);
}
