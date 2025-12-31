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

static int32_t	m_read_signed(const uint8_t *p, int size)
{
	uint32_t	v;
	int			i;
	int			shift;

	v = 0;
	i = 0;
	while (i < size)
	{
		v = (v << 8) | p[i];
		i++;
	}
	if (size > 0 && size < 4)
	{
		shift = 32 - size * 8;
		return ((int32_t)((int32_t)(v << shift) >> shift));
	}
	return ((int32_t)v);
}

static int	m_read_reg_arg(t_exec_ctx *c, int i)
{
	if (c->pc >= MEM_SIZE)
		return (1);
	c->args[i].value = c->vm->memory[c->pc];
	c->pc++;
	c->adv++;
	return (0);
}

static int	m_read_dir_arg(t_exec_ctx *c, int i)
{
	int	size;

	size = DIR_SIZE;
	if (c->op->has_idx)
		size = IND_SIZE;
	if (c->pc + size > MEM_SIZE)
		return (1);
	c->args[i].value = m_read_signed(&c->vm->memory[c->pc], size);
	c->pc += size;
	c->adv += size;
	return (0);
}

static int	m_read_ind_arg(t_exec_ctx *c, int i)
{
	int	size;

	size = IND_SIZE;
	if (c->pc + size > MEM_SIZE)
		return (1);
	c->args[i].value = m_read_signed(&c->vm->memory[c->pc], size);
	c->pc += size;
	c->adv += size;
	return (0);
}

int	exec_read_args(t_exec_ctx *c)
{
	int	i;

	i = 0;
	while (i < c->op->nb_params)
	{
		if (c->args[i].type == PARAM_REGISTER && m_read_reg_arg(c, i))
			return (1);
		if (c->args[i].type == PARAM_DIRECT && m_read_dir_arg(c, i))
			return (1);
		if (c->args[i].type == PARAM_INDIRECT && m_read_ind_arg(c, i))
			return (1);
		if (c->args[i].type == PARAM_UNKNOWN)
			return (1);
		i++;
	}
	return (0);
}
