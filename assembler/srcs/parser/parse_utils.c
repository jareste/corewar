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
#include "parse_internal.h"
#include "log.h"
#include "../encode/encode.h"

void	m_print_instr_args(t_instr *inst)
{
	int			i;
	t_arg_type	type;

	log_msg(LOG_I, "  Off[%d] Line %d: %s ",
		inst->offset, inst->line_no, inst->op->name);
	i = 0;
	while (i < inst->arg_count)
	{
		type = inst->args[i].type;
		if (type == ARG_REG)
			log_msg(LOG_I, "r%d", inst->args[i].u_.value);
		else if (type == ARG_DIR)
			log_msg(LOG_I, "%%%d", inst->args[i].u_.value);
		else if (type == ARG_IND)
			log_msg(LOG_I, "%d", inst->args[i].u_.value);
		else if (type == ARG_LABEL_DIR)
			log_msg(LOG_I, "%%:%s", inst->args[i].u_.label);
		else if (type == ARG_LABEL_IND)
			log_msg(LOG_I, ":%s", inst->args[i].u_.label);
		if (i < inst->arg_count - 1)
			log_msg(LOG_I, ", ");
		i++;
	}
	log_msg(LOG_I, "\n");
}

void	m_print_instr_noop(t_instr *inst)
{
	int	i;

	log_msg(LOG_I, "  Off[%d] Line %d: .code ",
		inst->offset, inst->line_no);
	i = 0;
	while (i < inst->raw_len)
	{
		log_msg(LOG_I, "%02X ", (uint8_t)inst->raw[i]);
		i++;
	}
	log_msg(LOG_I, "\n");
}

void	m_print_instrs(t_instr *inst_list, t_label *label_list)
{
	t_label	*label;
	t_instr	*inst;

	log_msg(LOG_I, "Instructions:\n");
	inst = inst_list;
	while (inst)
	{
		if (!inst->op)
			m_print_instr_noop(inst);
		else
			m_print_instr_args(inst);
		inst = FT_LIST_GET_NEXT(&inst_list, inst);
	}
	log_msg(LOG_I, "Labels:\n");
	label = label_list;
	while (label)
	{
		log_msg(LOG_I, "  %s at offset %d\n", label->name, label->offset);
		label = FT_LIST_GET_NEXT(&label_list, label);
	}
}

int	m_is_pc_relative_op(const t_op *op)
{
	return (op->has_idx != 0);
}

char	*m_skip_spaces(char *s)
{
	while (*s && isspace((unsigned char)*s))
		s++;
	return (s);
}
