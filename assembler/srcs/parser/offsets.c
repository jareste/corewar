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

#include "../asm.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include "log.h"
#include "../encode/encode.h"
#include "parse_internal.h"

int	m_find_nearest_offset(t_instr *inst_list, int line_no, int prog_size)
{
	t_instr	*inst;
	int		nearest_offset;
	int		nearest_line;

	inst = inst_list;
	nearest_offset = 0;
	nearest_line = 0;
	log_msg(LOG_D, "Finding nearest instr offset for line %d\n", line_no);
	while (inst)
	{
		log_msg(LOG_D, "  Checking instruction at line %d offset %d\n",
			inst->line_no, inst->offset);
		if (inst->line_no > line_no && ((inst->line_no < nearest_line)
				|| (nearest_line == 0)))
		{
			nearest_line = inst->line_no;
			nearest_offset = inst->offset;
		}
		inst = FT_LIST_GET_NEXT(&inst_list, inst);
	}
	if (nearest_line == 0)
		return (prog_size);
	return (nearest_offset);
}

int	m_compute_inst_size(t_instr *inst)
{
	int			size;
	int			i;
	t_arg_type	t;

	if (!inst->op)
		return (inst->raw_len);
	size = 1;
	if (inst->op->has_pcode)
		size += 1;
	i = 0;
	while (i < inst->arg_count)
	{
		t = inst->args[i].type;
		if (t == ARG_REG)
			size += 1;
		else if ((t == ARG_DIR || t == ARG_LABEL_DIR) && inst->op->has_idx)
			size += 2;
		else if ((t == ARG_DIR || t == ARG_LABEL_DIR) && !inst->op->has_idx)
			size += 4;
		else if (t == ARG_IND || t == ARG_LABEL_IND)
			size += 2;
		i++;
	}
	return (size);
}

int	m_compute_all_instructions_size(t_instr *inst_list)
{
	t_instr	*inst;
	int		offset;

	inst = inst_list;
	offset = 0;
	while (inst)
	{
		inst->offset = offset;
		offset += m_compute_inst_size(inst);
		inst = FT_LIST_GET_NEXT(&inst_list, inst);
	}
	return (offset);
}

void	m_compute_all_labels(t_label *label_list, t_instr *il, int off)
{
	t_label	*label;
	t_instr	*inst;

	label = label_list;
	while (label)
	{
		if (label->offset == 0)
		{
			label->offset = m_find_nearest_offset(il, label->line_no, off);
			log_msg(LOG_D, "Label '%s' at line %d assigned offset %d\n",
				label->name, label->line_no, label->offset);
		}
		inst = il;
		while (inst)
		{
			if (inst->label && strcmp(inst->label, label->name) == 0)
			{
				label->offset = inst->offset;
				break ;
			}
			inst = FT_LIST_GET_NEXT(&il, inst);
		}
		label = FT_LIST_GET_NEXT(&label_list, label);
	}
}

int	m_compute_offsets(t_instr *inst_list, t_label *label_list)
{
	int		offset;

	offset = m_compute_all_instructions_size(inst_list);
	m_compute_all_labels(label_list, inst_list, offset);
	return (offset);
}
