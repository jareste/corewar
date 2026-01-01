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

static int	m_fill_header_string(char *src, char *dest, size_t max)
{
	char	*start;
	char	*end;
	size_t	len;

	start = ft_strchr(src, '"');
	if (!start)
		return (0);
	start++;
	end = ft_strchr(start, '"');
	if (!end)
		return (0);
	len = (size_t)(end - start);
	if (len > max)
		len = max;
	ft_memcpy(dest, start, len);
	if (max == PROG_NAME_LENGTH)
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
	return ((uint8_t)ft_strtoul(byte_str, NULL, 16));
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
		while (*p && ft_isspace((unsigned char)*p))
			p++;
		if (!*p)
			break ;
		tmp[n++] = m_str_to_hex(p, line_no);
		p += 2;
	}
	inst = ft_calloc(1, sizeof(t_instr));
	inst->line_no = line_no;
	inst->op = NULL;
	inst->raw = ft_malloc(n);
	ft_memcpy(inst->raw, tmp, n);
	inst->raw_len = n;
	ft_list_add_last((void **)&p_st->i_l, (void *)inst);
	return (1);
}

int	check_special_line(char *line, int line_no, t_header *header,\
	t_parser_state *p_st)
{
	if (ft_strnstr(line, NAME_CMD_STRING, sizeof(NAME_CMD_STRING) - 1))
		return (m_fill_header_string(line,
				header->prog_name, PROG_NAME_LENGTH));
	else if (ft_strnstr(line, COMMENT_CMD_STRING,
			sizeof(COMMENT_CMD_STRING) - 1))
		return (m_fill_header_string(line,
				header->comment, COMMENT_LENGTH));
	else if (ft_strnstr(line, ".extend", sizeof(".extend") - 1))
	{
		log_msg(LOG_I, "Extend directive found, extended features enabled.\n");
		return (1);
	}
	else if (ft_strncmp(line, ".code", 5) == 0
		&& (line[5] == '\0' || ft_isspace((unsigned char)line[5])))
		return (m_handle_dot_code(line, line_no, p_st));
	return (0);
}
