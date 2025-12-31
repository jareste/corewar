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

static void	m_set_type_from_acb(t_arg *a, uint8_t acb)
{
	if (acb == REG_CODE)
		a->type = PARAM_REGISTER;
	else if (acb == DIR_CODE)
		a->type = PARAM_DIRECT;
	else if (acb == IND_CODE)
		a->type = PARAM_INDIRECT;
	else
		a->type = PARAM_UNKNOWN;
}

static int	m_decode_arg_types_pcode(t_exec_ctx *c)
{
	uint8_t	pcode;
	int		i;

	if (c->pc >= MEM_SIZE)
		return (1);
	pcode = c->vm->memory[c->pc];
	c->pc++;
	c->adv++;
	i = 0;
	while (i < c->op->nb_params)
	{
		m_set_type_from_acb(&c->args[i], (pcode >> (6 - 2 * i)) & 0x03);
		i++;
	}
	return (0);
}

static void	m_decode_arg_types_infer(t_exec_ctx *c)
{
	int	i;

	i = 0;
	while (i < c->op->nb_params)
	{
		if (c->op->param_types[i] & PARAM_REGISTER)
			c->args[i].type = PARAM_REGISTER;
		else if (c->op->param_types[i] & PARAM_DIRECT)
			c->args[i].type = PARAM_DIRECT;
		else if (c->op->param_types[i] & PARAM_INDIRECT)
			c->args[i].type = PARAM_INDIRECT;
		else
			c->args[i].type = PARAM_UNKNOWN;
		i++;
	}
}

int	exec_decode_arg_types(t_exec_ctx *c)
{
	c->pc = (c->prev_pc + 1) % MEM_SIZE;
	if (c->op->has_pcode)
		return (m_decode_arg_types_pcode(c));
	m_decode_arg_types_infer(c);
	return (0);
}
