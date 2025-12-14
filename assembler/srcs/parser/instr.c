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

static char	*m_fill_instr(char *s, int line_no, t_instr **out_inst)
{
	t_instr	*inst;
	char	*end;
	t_op	*op;
	size_t	len;

	inst = NEW(t_instr, 1);
	*out_inst = inst;
	end = s + ft_strcspn(s, " \t\n\r\f\v"); /* TODO add it to libft */
	len = end - s;
	op = m_find_op(s, len);
	if (!op)
	{
		log_msg(LOG_E, "Error: Unknown operation '%.*s' at line %d\n",
			(int)len, s, line_no);
		return (NULL);
	}
	log_msg(LOG_I, "Instruction '%s' at line %d\n", op->name, line_no);
	inst->line_no = line_no;
	inst->op = op;
	inst->arg_count = 0;
	ft_memset(inst->args, 0, sizeof(inst->args));
	return (end);
}

static t_instr	*m_free_inst(t_instr *inst)
{
	int	j;

	if (!inst)
		return (NULL);
	j = 0;
	while (j < inst->arg_count)
	{
		if ((inst->args[j].type == ARG_LABEL_DIR)
			|| (inst->args[j].type == ARG_LABEL_IND))
			free(inst->args[j].u_.label);
		j++;
	}
	free(inst);
	return (NULL);
}

int	m_parse_tokens(char *arg_str, t_instr *inst)
{
	char	*token;

	token = strtok(arg_str, ","); /* TODO add it to libft */
	log_msg(LOG_D, "  Parsing arguments: '%s'\n", arg_str);
	while (token && inst->arg_count < 3)
	{
		token = m_skip_spaces(token);
		if (ft_strchr(token, '+') || ft_strchr(token, '-'))
		{
			ft_memset(&inst->args[inst->arg_count], 0, sizeof(t_arg));
			if (token[0] == DIRECT_CHAR)
				inst->args[inst->arg_count].type = ARG_DIR;
			else
				inst->args[inst->arg_count].type = ARG_IND;
			inst->args[inst->arg_count].expr = strdup(token);
			if (!inst->args[inst->arg_count].expr)
				return (-1);
		}
		else if (m_parse_arg_token(token, &inst->args[inst->arg_count]) != 0)
			return (-1);
		log_msg(LOG_D, "  Arg[%d]: '%s'\n", inst->arg_count, token);
		inst->arg_count++;
		token = strtok(NULL, ",");
	}
	return (1);
}

static int	m_validate_args(t_instr *in)
{
	int	i;
	int	mask;

	if (in->arg_count != in->op->nb_params)
	{
		log_msg(LOG_E, "Error: Expected %d args for '%s' at line %d, got %d\n",
			in->op->nb_params, in->op->name, in->line_no, in->arg_count);
		return (-1);
	}
	i = 0;
	while (i < in->arg_count)
	{
		mask = m_mask_for_arg_type(in->args[i].type);
		if (!(in->op->param_types[i] & mask))
		{
			log_msg(LOG_E, "Error: Wrong type for arg %d of '%s' at line %d\n",
				i + 1, in->op->name, in->line_no);
			return (-1);
		}
		i++;
	}
	return (1);
}

t_instr	*m_new_instruction(char *instr_text, int line_no)
{
	t_instr	*inst;
	char	*end;
	char	*arg_str;
	char	*comment;

	end = m_fill_instr(instr_text, line_no, &inst);
	if (!end)
		return (m_free_inst(inst));
	arg_str = m_skip_spaces(end);
	comment = ft_strchr(arg_str, COMMENT_CHAR);
	if (comment)
		*comment = '\0';
	if (m_parse_tokens(arg_str, inst) != 1)
		return (m_free_inst(inst));
	if (m_validate_args(inst) != 1)
		return (m_free_inst(inst));
	return (inst);
}
