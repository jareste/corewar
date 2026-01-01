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
#include <libft.h>
#include "log.h"
#include "../encode/encode.h"
#include "parse_internal.h"

static void	m_eval_arg_expr(t_instr *inst, t_label *label_list, int i)
{
	int32_t	val;

	ft_assert(
		eval_expr(inst, i, &val, label_list) == 0,
		"Error in extended expression");
	log_msg(LOG_D, "  Final acc=%lld\n", (long long)val);
	inst->args[i].u_.value = val;
	free(inst->args[i].expr);
	inst->args[i].expr = NULL;
}

static void	m_update_label_arg(t_instr *inst, t_label *label, int i)
{
	if (inst->args[i].type == ARG_LABEL_DIR)
		inst->args[i].type = ARG_DIR;
	else if (inst->args[i].type == ARG_LABEL_IND)
		inst->args[i].type = ARG_IND;
	log_msg(LOG_D, "Normalized label '%s'", inst->args[i].u_.label);
	free(inst->args[i].u_.label);
	if (m_is_pc_relative_op(inst->op)
		|| inst->args[i].type == ARG_IND
		|| ft_strcmp(inst->op->name, "ld") == 0)
		inst->args[i].u_.value = label->offset - inst->offset;
	else
		inst->args[i].u_.value = label->offset;
	log_msg(LOG_D, " to value %d at line %d\n",
		inst->args[i].u_.value, inst->line_no);
}

static void	m_resolve_label_arg(t_instr *inst, t_label *label_list, int i)
{
	t_label	*label;

	label = label_list;
	while (label)
	{
		if (ft_strcmp(label->name, inst->args[i].u_.label) == 0)
		{
			m_update_label_arg(inst, label, i);
			break ;
		}
		label = ft_list_get_next((void **)&label_list, (void *)label);
	}
	ft_assert(label, "Label should be found here");
}

void	normalize_args(t_instr *inst_list, t_label *label_list)
{
	t_instr	*inst;
	int		i;

	inst = inst_list;
	while (inst)
	{
		i = 0;
		while (i < inst->arg_count)
		{
			if (inst->args[i].expr)
				m_eval_arg_expr(inst, label_list, i);
			else if (inst->args[i].type == ARG_LABEL_DIR
				|| inst->args[i].type == ARG_LABEL_IND)
				m_resolve_label_arg(inst, label_list, i);
			i++;
		}
		inst = ft_list_get_next((void **)&inst_list, (void *)inst);
	}
}
