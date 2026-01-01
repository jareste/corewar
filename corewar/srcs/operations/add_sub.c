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

int	m_op_add(t_vm *vm, t_proc *p, t_arg *args)
{
	int	r1;
	int	r2;
	int	r3;

	(void)vm;
	r1 = get_value(vm, p, &args[0]);
	r2 = get_value(vm, p, &args[1]);
	r3 = get_value(vm, p, &args[2]);
	if ((r1 < 1) || (r1 > REG_NUMBER)
		|| (r2 < 1) || (r2 > REG_NUMBER)
		|| (r3 < 1) || (r3 > REG_NUMBER))
	{
		log_msg(LOG_W, "Process %d: ADD with inv reg: r%d, r%d, r%d\n",
			p->id, r1, r2, r3);
		return (0);
	}
	p->regs[r3 - 1] = p->regs[r1 - 1] + p->regs[r2 - 1];
	p->carry = (p->regs[r3 - 1] == 0);
	log_msg(LOG_I,
		"Process %d: add r%d(%d) + r%d(%d) = %d → r%d, carry=%d\n",
		p->id, r1, p->regs[r1 - 1], r2, p->regs[r2 - 1],
		p->regs[r3 - 1], r3, p->carry);
	return (0);
}

int	m_op_sub(t_vm *vm, t_proc *p, t_arg *args)
{
	int	r1;
	int	r2;
	int	r3;

	(void)vm;
	r1 = get_value(vm, p, &args[0]);
	r2 = get_value(vm, p, &args[1]);
	r3 = get_value(vm, p, &args[2]);
	if (r1 < 1 || r1 > REG_NUMBER
		|| r2 < 1 || r2 > REG_NUMBER
		|| r3 < 1 || r3 > REG_NUMBER)
	{
		log_msg(LOG_W, "Process %d: SUB with inv reg: r%d, r%d, r%d\n",
			p->id, r1, r2, r3);
		return (0);
	}
	p->regs[r3 - 1] = p->regs[r1 - 1] - p->regs[r2 - 1];
	p->carry = (p->regs[r3 - 1] == 0);
	log_msg(LOG_I, "Process %d: sub r%d(%d) - r%d(%d) = %d → r%d, carry=%d\n",
		p->id, r1, p->regs[r1 - 1], r2, p->regs[r2 - 1],
		p->regs[r3 - 1], r3, p->carry);
	return (0);
}
