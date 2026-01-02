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
#include "../corewar.h"
#include "log.h"
#include "operations.h"
#include "../process/process.h"
#include "libft.h"
#include "op_internals.h"

static int	m_do_fork(t_vm *vm, t_proc *p, int new_pc)
{
	int		owner_id;
	t_proc	*child;

	owner_id = p->regs[0];
	child = create_process(new_pid(), new_pc, owner_id);
	memcpy(child->regs, p->regs, sizeof(p->regs));
	child->carry = p->carry;
	child->last_live_cycle = p->last_live_cycle;
	child->op_wait = 0;
	child->opcode = 0;
	ft_list_add_last((void **)&vm->procs, (void *)child);
	log_msg(LOG_I,
		"Process %d: fork → child %d at pc %d\n",
		p->id, child->id, child->pc);
	return (0);
}

int	m_op_fork(t_vm *vm, t_proc *p, t_arg *args)
{
	int16_t	offset;
	int		rel;
	int		new_pc;

	offset = (int16_t)args[0].value;
	rel = offset % IDX_MOD;
	new_pc = mem_addr(p->pc + rel);
	return (m_do_fork(vm, p, new_pc));
}

int	m_op_lfork(t_vm *vm, t_proc *p, t_arg *args)
{
	int16_t	offset;
	int		new_pc;

	offset = (int16_t)args[0].value;
	new_pc = mem_addr(p->pc + offset);
	return (m_do_fork(vm, p, new_pc));
}
