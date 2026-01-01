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

// static const t_op_func	m_op_funcs[OP_COUNT] =
// {
// 	NULL,               /* 0: no operation */
// 	m_op_live,          /* 1: live */
// 	m_op_ld,            /* 2: ld */
// 	m_op_st,            /* 3: st */
// 	m_op_add,           /* 4: add */
// 	m_op_sub,           /* 5: sub */
// 	m_op_and,           /* 6: and */
// 	m_op_or,            /* 7: or */
// 	m_op_xor,           /* 8: xor */
// 	m_op_zjmp,          /* 9: zjmp */
// 	m_op_ldi,           /* 10: ldi */
// 	m_op_sti,           /* 11: sti */
// 	m_op_fork,          /* 12: fork */
// 	m_op_lld,           /* 13: lld */
// 	m_op_lldi,          /* 14: lldi */
// 	m_op_lfork,         /* 15: lfork */
// 	m_op_aff            /* 16: aff */
// };
static void	m_init_operations(t_op_func ops[OP_COUNT])
{
	static bool	initialized = false;

	if (initialized)
		return ;
	ops[0] = NULL;
	ops[OP_LIVE] = m_op_live;
	ops[OP_LD] = m_op_ld;
	ops[OP_ST] = m_op_st;
	ops[OP_ADD] = m_op_add;
	ops[OP_SUB] = m_op_sub;
	ops[OP_AND] = m_op_and;
	ops[OP_OR] = m_op_or;
	ops[OP_XOR] = m_op_xor;
	ops[OP_ZJMP] = m_op_zjmp;
	ops[OP_LDI] = m_op_ldi;
	ops[OP_STI] = m_op_sti;
	ops[OP_FORK] = m_op_fork;
	ops[OP_LLD] = m_op_lld;
	ops[OP_LLDI] = m_op_lldi;
	ops[OP_LFORK] = m_op_lfork;
	ops[OP_AFF] = m_op_aff;
	initialized = true;
}

int	op_execute(t_vm *vm, t_proc *proc, t_arg *args, uint8_t op_code)
{
	static t_op_func	m_op_funcs[OP_COUNT];

	m_init_operations(m_op_funcs);
	if (op_code < 1 || op_code >= OP_COUNT)
	{
		log_msg(LOG_E,
			"Process %d: Invalid opcode %d\n",
			proc->id, op_code);
		return (-1);
	}
	log_msg(LOG_D,
		"Process %d: Executing operation %d\n",
		proc->id, op_code);
	if (m_op_funcs[op_code])
	{
		return (m_op_funcs[op_code](vm, proc, args));
	}
	else
	{
		log_msg(LOG_W,
			"Process %d: Operation %d not implemented yet\n",
			proc->id, op_code);
	}
	return (0);
}
