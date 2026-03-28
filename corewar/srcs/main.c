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
#include "corewar.h"
#include "ft_printf.h"
#include "decode/decode.h"
#include "process/process.h"
#include "parse/parse.h"
#include "bonus/bonus.h"

void	init_vm(t_vm *vm)
{
	ft_memset(vm, 0, sizeof(t_vm));
	vm->cycle_to_die = CYCLE_TO_DIE;
	vm->next_cycle_to_die = CYCLE_TO_DIE;
	vm->last_alive_player = -1;
}

void	dump_memory(t_vm *vm)
{
	int	i;
	int	j;

	i = 0;
	while (i < MEM_SIZE)
	{
		put_hex_n((uint32_t)i, 4);
		ft_putstr_fd(" : ", 1);
		j = 0;
		while (j < 16)
		{
			put_hex_byte(vm->memory[i + j]);
			if (j != 15)
				ft_putchar_fd(' ', 1);
			j++;
		}
		ft_putchar_fd('\n', 1);
		i += 16;
	}
}

void	m_run(t_vm *vm)
{
	t_proc	*proc;

	while (1)
	{
		vm->cycle++;
		if (vm->dump_enabled && vm->cycle == vm->dump_cycle)
		{
			dump_memory(vm);
			exit(0);
		}
		proc = vm->procs;
		while (proc)
		{
			step_proc(vm, proc);
			proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
		}
		if (vm->cycle >= vm->next_cycle_to_die)
		{
			log_msg(LOG_I, "Cycle %d: cycle to die check\n", vm->cycle);
			proc_check_deads(vm);
		}
		merge_procs(vm);
	}
}

int	main(int argc, char **argv)
{
	t_vm	vm;
	bool	bonus;

	if (argc < 2)
	{
		ft_dprintf(2, USAGE_MSG, argv[0], MAX_PLAYERS);
		return (1);
	}
	if (log_init() != 0)
	{
		ft_dprintf(2, "Error: Could not initialize logging system\n");
		return (1);
	}
	init_vm(&vm);
	if (parse_av(&vm, argv + 1, argc - 1, &bonus) != 0)
	{
		log_close();
		return (1);
	}
	if (bonus)
		handle_bonus(&vm);
	m_run(&vm);
	return (0);
}
