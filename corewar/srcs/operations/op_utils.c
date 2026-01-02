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

int	mem_addr(int addr)
{
	addr %= MEM_SIZE;
	if (addr < 0)
		addr += MEM_SIZE;
	return (addr);
}

int32_t	m_mem_read(t_vm *vm, int addr, int size)
{
	uint8_t	buf[4];
	int32_t	v;
	int		i;

	v = 0;
	ft_memset(buf, 0, sizeof(buf));
	addr = mem_addr(addr);
	i = 0;
	while (i < size)
	{
		buf[i] = vm->memory[mem_addr(addr + i)];
		i++;
	}
	i = 0;
	while (i < size)
	{
		v = (v << 8) | buf[i];
		i++;
	}
	if (size < 4 && (buf[0] & 0x80))
		v -= 1 << (size * 8);
	return (v);
}

void	m_mem_write(t_vm *vm, int addr, int32_t value, int size)
{
	int	i;

	addr = mem_addr(addr);
	i = size - 1;
	while (i >= 0)
	{
		vm->memory[mem_addr(addr + i)] = (value & 0xFF);
		value >>= 8;
		i--;
	}
}

int32_t	get_value(t_vm *vm, t_proc *p, t_arg *a)
{
	int	addr;

	if (a->type == PARAM_REGISTER)
    {
        if (a->value < 1 || a->value > REG_NUMBER)
        {
            log_msg(LOG_E, "Invalid register r%d\n", a->value);
            return (0);
        }
        return (p->regs[a->value - 1]);
    }
	if (a->type == PARAM_INDIRECT)
	{
		addr = p->pc + (a->value % IDX_MOD);
		return (m_mem_read(vm, addr, 4));
	}
	return (a->value);
}

t_champ	*find_champ_by_id(t_vm *vm, int id)
{
	int	i;

	i = 0;
	while (i < MAX_PLAYERS)
	{
		log_msg(LOG_D, "Checking champ id %d against %d\n",
			vm->champs[i].id, id);
		if (vm->champs[i].id == id)
			return (&vm->champs[i]);
		i++;
	}
	return (NULL);
}
