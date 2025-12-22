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

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <libft.h>
#include "../asm.h"
#include "log.h"

void	m_handle_pcode(t_instr *inst, uint8_t *code, int *pos)
{
	uint8_t	acb;
	int		i;
	int		bits;

	code[(*pos)++] = inst->op->opcode;
	if (inst->op->has_pcode)
	{
		acb = 0;
		i = 0;
		while (i < inst->arg_count)
		{
			if (inst->args[i].type == ARG_REG)
				bits = REG_CODE;
			else if (inst->args[i].type == ARG_DIR)
				bits = DIR_CODE;
			else if (inst->args[i].type == ARG_IND)
				bits = IND_CODE;
			else
				bits = 0;
			acb |= (bits << (6 - 2 * i));
			i++;
		}
		code[(*pos)++] = acb;
	}
}

int	m_get_arg_size(t_arg *a, int *size, int has_idx)
{
	*size = 0;
	if (a->type == ARG_REG)
	{
		*size = 1;
		return (0);
	}
	else if (a->type == ARG_DIR)
	{
		if (has_idx)
			*size = IND_SIZE;
		else
			*size = DIR_SIZE;
	}
	else if (a->type == ARG_IND)
	{
		*size = IND_SIZE;
	}
	return (1);
}

void	m_print_instr(t_instr *inst, uint8_t *code, int pos)
{
	int	i;

	log_msg(LOG_I, "Encoded instruction '%s'[%d] at offset %d: ",
		inst->op->name, inst->line_no, inst->offset);
	i = inst->offset;
	while (i < pos)
	{
		log_msg(LOG_I, "%02X ", code[i]);
		i++;
	}
	log_msg(LOG_I, "\n");
}

void	m_handle_instr_args(t_instr *inst, uint8_t *code, int *pos)
{
	int	i;
	int	val;
	int	b;
	int	size;

	i = 0;
	while (i < inst->arg_count)
	{
		if (m_get_arg_size(&inst->args[i], &size, inst->op->has_idx) == 0)
		{
			code[(*pos)++] = (uint8_t)inst->args[i].u_.value;
			i++;
			continue ;
		}
		val = inst->args[i].u_.value;
		b = size - 1;
		while (b >= 0)
		{
			code[(*pos) + b] = (uint8_t)(val & 0xFF);
			val >>= 8;
			b--;
		}
		(*pos) += size;
		i++;
	}
}

int	encode_instruction(t_instr *inst, uint8_t *code)
{
	int		pos;

	pos = inst->offset;
	if (!inst->op)
	{
		ft_memcpy(&code[pos], inst->raw, inst->raw_len);
		return (pos + inst->raw_len);
	}
	m_handle_pcode(inst, code, &pos);
	m_handle_instr_args(inst, code, &pos);
	m_print_instr(inst, code, pos);
	return (pos);
}
