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
#include <libft.h>
#include <stdbool.h>
#include "log.h"
#include "../encode/encode.h"
#include "parse_internal.h"

static int	m_fill_header_string(char *src, char *pattern, char *dest, int n)
{
	sscanf(src, pattern, dest);
	if (n == 1)
		log_msg(LOG_I, "Program Name: '%s'\n", dest);
	else
		log_msg(LOG_I, "Program Comment: '%s'\n", dest);
	return (1);
}

static uint8_t	m_str_to_hex(char *p, int line_no)
{
	char	byte_str[3];

	if (!isxdigit((unsigned char)p[0]) || !isxdigit((unsigned char)p[1]))
	{
		log_msg(LOG_E, "Bad .code byte at line %u: '%s'\n", line_no, p);
		exit(1);
	}
	byte_str[0] = p[0];
	byte_str[1] = p[1];
	byte_str[2] = '\0';
	return ((uint8_t)strtoul(byte_str, NULL, 16)); /* todo make it with libft */
}

static int	m_handle_dot_code(char *line, int line_no, t_parser_state *p_st)
{
	t_instr	*inst;
	char	*p;
	uint8_t	tmp[256];
	int		n;

	p = line + 5;
	n = 0;
	while (*p)
	{
		while (*p && isspace((unsigned char)*p))
			p++;
		if (!*p)
			break ;
		tmp[n++] = m_str_to_hex(p, line_no);
		p += 2;
	}
	inst = NEW(t_instr, 1);
	inst->line_no = line_no;
	inst->op = NULL;
	inst->raw = malloc(n);
	memcpy(inst->raw, tmp, n);
	inst->raw_len = n;
	FT_LIST_ADD_LAST(&p_st->i_l, inst);
	return (1);
}

int	check_special_line(char *line, int line_no, t_header *header,\
	t_parser_state *p_st)
{
	if (strstr(line, NAME_CMD_STRING))
		return (m_fill_header_string(line,
				NAME_CMD_STRING" \"%[^\"]\"", header->prog_name, 1));
	else if (strstr(line, COMMENT_CMD_STRING))
		return (m_fill_header_string(line,
				COMMENT_CMD_STRING" \"%[^\"]\"", header->comment, 2));
	else if (strstr(line, ".extend"))
	{
		// m_extend_enabled = true;
		log_msg(LOG_I, "Extend directive found, extended features enabled.\n");
		return (1);
	}
	else if (strncmp(line, ".code", 5) == 0
		&& (line[5] == '\0' || isspace((unsigned char)line[5])))
		return (m_handle_dot_code(line, line_no, p_st));
	return (0);
}
