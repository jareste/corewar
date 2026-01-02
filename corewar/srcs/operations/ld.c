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

int	m_op_ldi(t_vm *vm, t_proc *p, t_arg *args)
{
	int32_t	a;
	int32_t	b;
	int32_t	addr;
	int32_t	val;
	int		reg;

	log_msg(LOG_D, "Process [%d] %p: LDI called\n", p->id, (void *)p);
	reg = args[2].value;
	if (reg < 1 || reg > REG_NUMBER)
	{
		log_msg(LOG_W, "Invalid register r%d in LDI\n", reg);
		return (0);
	}
	a = get_value(vm, p, &args[0]);
	b = get_value(vm, p, &args[1]);
	addr = p->pc + ((a + b) % IDX_MOD);
	val = m_mem_read(vm, addr, 4);
	p->regs[reg - 1] = val;
	p->carry = (val == 0);
	log_msg(LOG_I,
		"Process %d: ldi (%d + %d) %% IDX_MOD -> [%d] = %d → r%d\n",
		p->id, a, b, addr, val, reg);
	return (0);
}

int	m_op_lldi(t_vm *vm, t_proc *p, t_arg *args)
{
	int32_t	a;
	int32_t	b;
	int32_t	addr;
	int32_t	val;
	int		reg;

	log_msg(LOG_D, "Process [%d] %p: LLDI called\n", p->id, (void *)p);
	reg = args[2].value;
	if (reg < 1 || reg > REG_NUMBER)
	{
		log_msg(LOG_W, "Invalid register r%d in LLDI\n", reg);
		return (0);
	}
	a = get_value(vm, p, &args[0]);
	b = get_value(vm, p, &args[1]);
	addr = p->pc + (a + b);
	val = m_mem_read(vm, addr, 4);
	p->regs[reg - 1] = val;
	p->carry = (val == 0);
	log_msg(LOG_I,
		"Process %d: lldi (%d + %d) -> [%d] = %d → r%d\n",
		p->id, a, b, addr, val, reg);
	return (0);
}

int	m_op_lld(t_vm *vm, t_proc *p, t_arg *args)
{
	int32_t	arg1;
	int32_t	arg2;
	int32_t	addr;

	log_msg(LOG_D, "Process [%d] %p: LLD called\n", p->id, (void *)p);
	if (args[0].type == PARAM_INDIRECT)
	{
		addr = p->pc + args[0].value;
		arg1 = m_mem_read(vm, addr, 4);
	}
	else
		arg1 = get_value(vm, p, &args[0]);
	if (args[1].value < 1 || args[1].value > REG_NUMBER)
	{
		log_msg(LOG_W, "Invalid register r%d in LLD\n", args[1].value);
		return (0);
	}
	arg2 = get_value(vm, p, &args[1]);
	p->regs[arg2 - 1] = arg1;
	p->carry = (arg1 == 0);
	log_msg(LOG_I,
		"Process %d: lld %d → r%d\n",
		p->id, arg1, arg2);
	return (0);
}

int	m_op_ld(t_vm *vm, t_proc *p, t_arg *args)
{
	int32_t	arg1;
	int32_t	arg2;
	t_arg	lld_args[2];

	(void)vm;
	log_msg(LOG_D, "Process [%d] %p: LD called\n", p->id, (void *)p);
	arg1 = get_value(vm, p, &args[0]);
	arg2 = get_value(vm, p, &args[1]);
	lld_args[0].type = PARAM_DIRECT;
	lld_args[0].value = arg1;
	lld_args[1].type = PARAM_REGISTER;
	lld_args[1].value = arg2;
	return (m_op_lld(vm, p, lld_args));
}
