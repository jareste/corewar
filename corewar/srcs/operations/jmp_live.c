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

int	m_op_live(t_vm *vm, t_proc *proc, t_arg *args)
{
	int32_t	champ_id;
	t_champ	*champ;

	champ_id = args[0].value;
	champ = find_champ_by_id(vm, champ_id);
	proc->last_live_cycle = vm->cycle;
	vm->lives_in_period++;
	log_msg(LOG_I,
		"Process %d: LIVE called for champ %d\n",
		proc->id, champ_id);
	if (champ)
	{
		vm->last_alive_player = champ_id;
		log_msg(LOG_I,
			"Champ %d (%s) is reported alive!\n",
			champ->id, champ->name);
	}
	else
	{
		log_msg(LOG_W,
			"LIVE called with invalid champ id %d\n",
			champ_id);
	}
	return (0);
}

int	m_op_zjmp(t_vm *vm, t_proc *proc, t_arg *args)
{
	int16_t	offset;
	int		jump;

	(void)vm;
	offset = args[0].value;
	jump = offset % IDX_MOD;
	log_msg(LOG_I, "Process %d: ZJMP offset %d (jmp %d), carry=%d\n",
		proc->id, offset, jump, proc->carry);
	if (proc->carry)
	{
		proc->pc = (proc->pc + jump) % MEM_SIZE;
		if (proc->pc < 0)
			proc->pc += MEM_SIZE;
		log_msg(LOG_I, "Process %d: ZJMP succeeded, jmp by %d → new PC=%d\n",
			proc->id, jump, proc->pc);
	}
	else
	{
		log_msg(LOG_I, "Process %d: ZJMP failed (carry = 0)\n",
			proc->id);
		proc->pc = (proc->pc + 3) % MEM_SIZE;
	}
	return (0);
}
