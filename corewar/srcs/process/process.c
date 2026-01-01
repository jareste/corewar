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
	return (proc);
}

#ifdef DEBUG /* KNH */
// void	decode_inst(const uint8_t *code, int code_len, int offset)
// {
// 	int			size;
// 	int			pos;
// 	int			nargs;
// 	int			i;
// 	int			b;
// 	int			atype;
// 	uint8_t		opcode;
// 	uint8_t		acb;
// 	uint8_t		r;
// 	int32_t		val;
// 	const t_op	*op;

// 	pos = offset;
// 	if (pos >= code_len)
// 		return;
// 	opcode = code[pos++];
// 	op = NULL;
// 	if (opcode >= 1 && opcode <= 16)
// 		op = &op_tab[opcode];
// 	log_msg(LOG_E, "opcode 0x%02X", opcode);
// 	if (op)
// 		log_msg(LOG_E, " -> %s", op->name);
// 	log_msg(LOG_E, " at %d\n", offset);
// 	acb = 0;
// 	if (op && op->has_pcode && pos < code_len)
// 	{
// 		acb = code[pos++];
// 		log_msg(LOG_E, " ACB=0x%02X\n", acb);
// 	}
// 	nargs = op ? op->nb_params : 0;
// 	for (i = 0; i < nargs && pos < code_len; ++i)
// 	{
// 		atype = 0;
// 		if (op && op->has_pcode)
// 		{
// 			/* 1=REG, 2=DIR, 3=IND */
// 			atype = (acb >> (6 - 2 * i)) & 0x3;
// 		}
// 		else if (op)
// 		{
// 			/* fallback: infer from PARAM_* bitmask */
// 			if (op->param_types[i] & PARAM_REGISTER)
// 				atype = REG_CODE; /* 1 */
// 			else if (op->param_types[i] & PARAM_DIRECT)
// 				atype = DIR_CODE; /* 2 */
// 			else if (op->param_types[i] & PARAM_INDIRECT)
// 				atype = IND_CODE; /* 3 */
// 			else
// 				atype = 0;
// 		}

// 		if (atype == REG_CODE) /* register */
// 		{
// 			if (pos >= code_len)
// 			{
// 				log_msg(LOG_E, "  truncated REG\n");
// 				return;
// 			}
// 			r = code[pos++];
// 			log_msg(LOG_E, "  arg%d: REG r%u\n", i + 1, r);
// 		}
// 		else if (atype == DIR_CODE || atype == IND_CODE)
// 		{
// 			if (atype == IND_CODE)
// 				size = IND_SIZE;
// 			else
// 				size = (op && op->has_idx) ? IND_SIZE : DIR_SIZE;

// 			if (pos + size > code_len)
// 			{
// 				log_msg(LOG_E, "  truncated arg\n");
// 				return;
// 			}

// 			val = read_signed(&code[pos], size);
// 			log_msg(LOG_E, "  arg%d: %s %d (0x",
// 					i + 1, atype == DIR_CODE ? "DIR" : "IND", val);
// 			for (b = 0; b < size; ++b)
// 				log_msg(LOG_E, "%02X", code[pos + b]);
// 			log_msg(LOG_E, ")\n");
// 			pos += size;
// 		}
// 		else
// 		{
// 			log_msg(LOG_E, "  arg%d: UNKNOWN type (acb mismatch?)\n", i + 1);
// 		}
// 	}
// }
#else

void	decode_inst(const uint8_t *code, int code_len, int offset)
{
	(void)code;
	(void)code_len;
	(void)offset;
}

#endif

void	execute_instruction(t_vm *vm, t_proc *proc)
{
	t_exec_ctx	c;

	exec_init(&c, vm, proc);
	if (exec_fetch_opcode(&c))
		return ;
	log_msg(LOG_I, "Process %d: Executing opcode %s at pc %d.\n",
		proc->id, c.op->name, c.prev_pc);
	decode_inst(vm->memory, MEM_SIZE, c.prev_pc);
	if (exec_decode_arg_types(&c))
		return ;
	if (exec_read_args(&c))
		return ;
	if (exec_validate_regs(&c))
		return ;
	if (op_execute(vm, proc, c.args, c.opcode) != 0)
		log_msg(LOG_E, "Process %d: Error executing opcode %s\n",
			proc->id, c.op->name);
	exec_advance_pc(&c);
}

void	step_proc(t_vm *vm, t_proc *p)
{
	uint8_t	opcode;
	t_op	*op;

	if (p->op_wait > 0)
	{
		p->op_wait--;
		return ;
	}
	execute_instruction(vm, p);
	opcode = vm->memory[p->pc];
	if (opcode < 1 || opcode > 16)
	{
		log_msg(LOG_W, "Process %d: Inv opcode %02X at pc %d. Adv 1b.\n",
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
