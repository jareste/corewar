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

// static int	m_op_st(t_vm *vm, t_proc *p, t_arg *args)
// {
// 	int32_t	src;
// 	int		addr;

// 	if (args[0].value < 1 || args[0].value > REG_NUMBER)
// 	{
// 		log_msg(LOG_W, "Process %d: ST with invalid src r%d\n",
// 			p->id, args[0].value);
// 		return (0);
// 	}
// 	src = p->regs[args[0].value - 1];
// 	if (args[1].type == PARAM_REGISTER)
// 	{
// 		if (args[1].value < 1 || args[1].value > REG_NUMBER)
// 		{
// 			log_msg(LOG_W, "Process %d: ST with invalid dst r%d\n",
// 				p->id, args[1].value);
// 			return (0);
// 		}
// 		p->regs[args[1].value - 1] = src;
// 		log_msg(LOG_I, "Process %d: st r%d (%d) → r%d\n",
// 			p->id, args[0].value, src, args[1].value);
// 	}
// 	else if (args[1].type == PARAM_INDIRECT)
// 	{
// 		addr = mem_addr(p->pc + (args[1].value % IDX_MOD));
// 		m_mem_write(vm, addr, src, 4);
// 		log_msg(LOG_I,
// 			"Process %d: st r%d (%d) → mem[%d] (pc %d + %d %% IDX_MOD)\n",
// 			p->id, args[0].value, src, addr, p->pc, args[1].value);
// 	}
// 	else
// 	{
// 		log_msg(LOG_W, "Process %d: ST with invalid second arg type %d\n",
// 			p->id, args[1].type);
// 	}
// 	return (0);
// }

int	m_op_st(t_vm *vm, t_proc *p, t_arg *args)
{
	int32_t	src;
	int		addr;

	if (args[0].value < 1 || args[0].value > REG_NUMBER)
	{
		return (0);
	}
	src = p->regs[args[0].value - 1];
	if (args[1].type == PARAM_REGISTER)
	{
		if (args[1].value < 1 || args[1].value > REG_NUMBER)
		{
			log_msg(LOG_W, "Process %d: ST with invalid dst r%d\n",
				p->id, args[1].value);
			return (0);
		}
		p->regs[args[1].value - 1] = src;
	}
	else if (args[1].type == PARAM_INDIRECT)
	{
		addr = mem_addr(p->pc + (args[1].value % IDX_MOD));
		m_mem_write(vm, addr, src, 4);
	}
	return (0);
}

/* op STI
* Param1: register (value to store)
* Param2: value
* Param3: value
* Result: store value of reg at address (pc + (param2 + param3) % IDX_MOD)
*/
int	m_op_sti(t_vm *vm, t_proc *p, t_arg *args)
{
	int32_t	reg_val;
	int32_t	arg2;
	int32_t	arg3;
	int32_t	addr;
	int16_t	offset;

	if ((args[0].value < 1) || (args[0].value > REG_NUMBER))
	{
		log_msg(LOG_W, "Process %d: STI with invalid src r%d\n",
			p->id, args[0].value);
		return (0);
	}
	reg_val = p->regs[args[0].value - 1];
	arg2 = get_value(vm, p, &args[1]);
	arg3 = get_value(vm, p, &args[2]);
	offset = (int16_t)((arg2 + arg3) % IDX_MOD);
	addr = mem_addr(p->pc + offset);
	m_mem_write(vm, addr, reg_val, 4);
	log_msg(LOG_I, "P %d: sti r%d (%d) → m[%d] (pc %d + (%d+%d) %% IDX_MOD)\n",
		p->id, args[0].value, reg_val, addr, p->pc, arg2, arg3);
	return (0);
}
