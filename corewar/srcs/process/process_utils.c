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

int	exec_fetch_opcode(t_exec_ctx *c)
{
	c->prev_pc = c->proc->pc;
	c->opcode = c->vm->memory[c->prev_pc];
	if (c->opcode < 1 || c->opcode > 16)
		return (1);
	c->op = &op_tab[c->opcode];
	return (0);
}

void	exec_init(t_exec_ctx *c, t_vm *vm, t_proc *proc)
{
	memset(c, 0, sizeof(*c));
	c->vm = vm;
	c->proc = proc;
	c->adv = 1;
}

int	exec_validate_regs(t_exec_ctx *c)
{
	int	i;

	i = 0;
	while (i < c->op->nb_params)
	{
		if (c->args[i].type == PARAM_REGISTER)
		{
			if (c->args[i].value < 1 || c->args[i].value > REG_NUMBER)
				return (1);
		}
		i++;
	}
	return (0);
}

void	exec_advance_pc(t_exec_ctx *c)
{
	if (c->opcode == OP_ZJMP)
		return ;
	c->proc->pc = (c->prev_pc + c->adv) % MEM_SIZE;
}
