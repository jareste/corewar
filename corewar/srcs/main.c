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

void	init_vm(t_vm *vm)
{
	ft_memset(vm->memory, 0, MEM_SIZE);
	vm->procs = NULL;
	vm->cycle = 0;
	vm->cycle_to_die = CYCLE_TO_DIE;
	vm->last_check_cycle = 0;
	vm->last_alive_player = -1;
}

void	m_dump_memory(t_vm *vm)
{
	int	i;
	int	j;

	i = 0;
	while (i < MEM_SIZE)
	{
		put_hex_n((uint32_t)i, 4);
		ft_putstr_fd(" : ", 1);
		j = 0;
		while (j < 32)
		{
			put_hex_byte(vm->memory[i + j]);
			if (j != 31)
				ft_putchar_fd(' ', 1);
			j++;
		}
		ft_putchar_fd('\n', 1);
		i += 32;
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
			m_dump_memory(vm);
			exit(0);
		}
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
	if (parse_av(&vm, argv + 1, argc - 1) != 0)
	{
		log_close();
		return (1);
	}
	m_run(&vm);
	return (0);
}
