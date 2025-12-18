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
#include <gnl.h>
#include <libft.h>

static int	m_handle_instr_part(char *instr_part, uint32_t line_no,
		t_parser_state *p_st, t_label *def_label)
{
	t_instr	*inst;

	if (*instr_part != '\0' && *instr_part != COMMENT_CHAR)
	{
		inst = m_new_instruction(instr_part, line_no);
		if (!inst)
		{
			log_msg(LOG_E, "Error: Invalid instruction at line %u\n", line_no);
			exit(1);
		}
		if (def_label)
		{
			inst->label = def_label->name;
			FT_LIST_ADD_LAST(&p_st->l_l, def_label);
		}
		FT_LIST_ADD_LAST(&p_st->i_l, inst);
	}
	else if (def_label)
	{
		FT_LIST_ADD_LAST(&p_st->l_l, def_label);
	}
	return (0);
}

static char	*m_handle_label(char *colon, char *line, t_label **def_label,
		int line_no)
{
	char	*instr_part;

	*colon = '\0';
	*def_label = m_new_label(line);
	if (!*def_label)
	{
		log_msg(LOG_E, "Invalid label name '%s' at %u\n", line, line_no);
		exit(1);
	}
	(*def_label)->line_no = line_no;
	instr_part = m_skip_spaces(colon + 1);
	log_msg(LOG_I, "Found label '%s' at %u\n", (*def_label)->name, line_no);
	return (instr_part);
}

int	handle_label_colon(char *line, uint32_t line_no, t_parser_state *p_st)
{
	char	*colon;
	char	*space;
	char	*instr_part;
	t_label	*def_label;

	colon = ft_strchr(line, LABEL_CHAR);
	space = line + ft_strcspn(line, " \t\n\r\f\v");
	def_label = NULL;
	if (colon && colon > line && colon < space)
		instr_part = m_handle_label(colon, line, &def_label, line_no);
	else
	{
		instr_part = line;
		log_msg(LOG_D, "No label found at %u\n", line_no);
	}
	return (m_handle_instr_part(instr_part, line_no, p_st, def_label));
}
