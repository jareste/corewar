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
#include "process_internal.h"

int	new_pid(void)
{
	static int	pid = 0;

	return (++pid);
}

t_proc	*create_process(int id, int pc, int owner_id)
{
	t_proc	*proc;

	proc = (t_proc *)ft_malloc(sizeof(t_proc));
	proc->id = id;
	memset(proc->regs, 0, sizeof(proc->regs));
	proc->pc = pc % MEM_SIZE;
	proc->carry = 0;
	proc->last_live_cycle = 0;
	proc->op_wait = 0;
	proc->opcode = 0;
	proc->regs[0] = owner_id;
	proc->l.next = NULL;
	proc->l.prev = NULL;
	log_msg(LOG_I, "Created process %d for owner %d at pc %d\n",
		proc->id, owner_id, proc->pc);
	return (proc);
}

void	step_proc(t_vm *vm, t_proc *p)
{
	uint8_t	opcode;
	t_op	*op;

	if (p->op_wait > 0)
	{
		p->op_wait--;
		if (p->op_wait == 0)
			execute_instruction(vm, p);
		return ;
	}
	opcode = vm->memory[p->pc];
	if (opcode < 1 || opcode > 16)
	{
		log_msg(LOG_W, "Process %d: Inv opcode %d at pc %d. Adv 1b.\n",
			p->id, opcode, p->pc);
		p->pc = (p->pc + 1) % MEM_SIZE;
		return ;
	}
	op = &op_tab[opcode];
	p->opcode = opcode;
	p->op_wait = op->nb_cycles - 1;
	log_msg(LOG_I, "Process %d: Fetched opcode %s at pc %d. \
		Will execute in %d cycles.\n", p->id, op->name, p->pc, p->op_wait + 1);
}
