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

#include <string.h>
#include "ft_malloc.h"
#include "log.h"
#include "../corewar.h"
#include "../process/process.h"
#include "../operations/operations.h"
#include "gnl.h"
#include "ft_printf.h"
#include "bonus_internal.h"

void	dump_procs(t_vm *vm, t_args *args)
{
	t_proc	*p;

	(void)args;
	log_msg(LOG_I, "Dumping processes at cycle %d:\n", vm->cycle);
	p = vm->procs;
	while (p)
	{
		log_msg(LOG_I, "  Process %d: pc=%d carry=%d wait=%d opcode=%d\n",
			p->id, p->pc, p->carry, p->op_wait, p->opcode);
		p = ft_list_get_next((void **)&vm->procs, (void *)p);
	}
}

void	dump_memory_b(t_vm *vm, t_args *args)
{
	(void)args;
	log_msg(LOG_I, "Dumping memory at cycle %d:\n", vm->cycle);
	dump_memory(vm);
}

static void	m_run_one_cycle(t_vm *vm)
{
	t_proc	*proc;

	vm->cycle++;
	proc = vm->procs;
	while (proc)
	{
		step_proc(vm, proc);
		proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
	}
	if (vm->cycle == vm->next_cycle_to_die)
	{
		log_msg(LOG_I, "Cycle %d: cycle to die(%d) check\n",
			vm->cycle, vm->cycle_to_die);
		proc_check_deads(vm);
	}
	merge_procs(vm);
}

void	run_cycles(t_vm *vm, t_args *args)
{
	int		cycles;

	if (args->argc < 2)
	{
		log_msg(LOG_W, "Usage: run <cycles>\n");
		return ;
	}
	cycles = ft_atoi(args->argv[1]);
	log_msg(LOG_I, "Running for %d cycles...\n", cycles);
	while (cycles)
	{
		m_run_one_cycle(vm);
		cycles--;
	}
}
