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

int	m_op_aff(t_vm *vm, t_proc *p, t_arg *args)
{
	int		reg_num;
	char	c;

	if (!vm->aff_enabled)
		return (0);
	reg_num = args[0].value;
	if (reg_num < 1 || reg_num > REG_NUMBER)
	{
		log_msg(LOG_W, "Process %d: AFF with invalid register r%d\n",
			p->id, reg_num);
		return (0);
	}
	c = (char)(p->regs[reg_num - 1] % 256);
	log_msg(LOG_I, "Process %d: AFF r%d -> '%c'\n",
		p->id, reg_num, c);
	return (0);
}

/* op AND
* Param1: value
* Param2: value
* Param3: register
* Result: param1 & param2 stored in register
* Carry: Yes -> if result == 0
*/
int	m_op_and(t_vm *vm, t_proc *p, t_arg *args)
{
	int32_t	arg1;
	int32_t	arg2;
	int32_t	result;
	int		dest;

	arg1 = get_value(vm, p, &args[0]);
	arg2 = get_value(vm, p, &args[1]);
	dest = args[2].value;
	if (dest < 1 || dest > REG_NUMBER)
	{
		log_msg(LOG_W, "Invalid register %d in AND\n", dest);
		return (0);
	}
	result = arg1 & arg2;
	p->regs[dest - 1] = result;
	p->carry = (result == 0);
	log_msg(LOG_I,
		"Process %d: and %d & %d = %d → r%d\n",
		p->id, arg1, arg2, result, dest);
	return (0);
}
