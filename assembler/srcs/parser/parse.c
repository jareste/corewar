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

#define FILE_CREATE	0x01
#define FILE_CLOSE	0x02
#define FILE_GET	0x04

t_label	*find_label(t_label *label_list, const char *name)
{
	t_label	*lab;

	lab = label_list;
	while (lab)
	{
		if (ft_strcmp(lab->name, name) == 0)
			return (lab);
		lab = FT_LIST_GET_NEXT(&label_list, lab);
	}
	return (NULL);
}

int	m_handle_file(const char *filename, int options)
{
	static int m_fd = -1;

	if (options & FILE_CLOSE)
	{
		if (m_fd != -1)
		{
			close(m_fd);
			m_fd = -1;
		}
		return (-1);
	}
	else if (options & FILE_CREATE)
	{
		if (m_fd != -1)
			close(m_fd);
		m_fd = open(filename, O_RDONLY);
		ft_assert(m_fd != -1, "Failed to open file");
		return (m_fd);
	}
	ft_assert(m_fd != -1, "File not opened");
	return (m_fd);
}

/* static buffer just for line to exist. */
int	m_read_line(char **line, uint32_t *line_no)
{
	static char	*buffer = NULL;

	(*line_no)++;
	free(buffer);
	buffer = get_next_line(m_handle_file(NULL, FILE_GET));
	if (!buffer)
		return (1);
	buffer[ft_strcspn(buffer, "\n")] = '\0';
	*line = m_skip_spaces(buffer);
	if (**line == '\0' || **line == COMMENT_CHAR)
		return (m_read_line(line, line_no));
	return (0);
}

int	parse_file(const char *filename, t_header *header, t_parser_state *p_st)
{
	uint32_t line_no = 0;
	// int res;
	char *line;
	char *colon;
	char *space;
	char *instr_part;
	t_label *def_label;
	t_instr *inst;
	t_label *label_list = NULL;
	t_instr *instr_list = NULL;

	m_handle_file(filename, FILE_CREATE);
	while (m_read_line(&line, &line_no) == 0)
	{
		log_msg(LOG_D, "Read line[%u] '%s'\n", line_no, line);

		/* We got name or comment? */
		if (strstr(line, NAME_CMD_STRING))
		{
			sscanf(line, NAME_CMD_STRING" \"%[^\"]\"", header->prog_name);
			log_msg(LOG_I, "Program Name: '%s'\n", header->prog_name);
			continue;
		}
		else if (strstr(line, COMMENT_CMD_STRING))
		{
			sscanf(line, COMMENT_CMD_STRING" \"%[^\"]\"", header->comment);
			log_msg(LOG_I, "Program Comment: '%s'\n", header->comment);
			continue;
		}
		else if (strstr(line, ".extend"))
		{
			// m_extend_enabled = true;
			log_msg(LOG_I, "Extend directive found, extended features enabled.\n");
			continue;
		}
		else if (strncmp(line, ".code", 5) == 0 && 
			(line[5] == '\0' || isspace((unsigned char)line[5])))
		{ /* TODO fixme */
			// if (!m_extend_enabled)
			// {
			//     log_msg(LOG_E, ".code used without .extend at line %u\n", line_no);
			//     return ERROR;
			// }

			t_instr *inst = NEW(t_instr, 1);
			memset(inst, 0, sizeof(*inst));
			inst->line_no = line_no;
			inst->op = NULL;

			char *p = line + 5;
			p = m_skip_spaces(p);

			uint8_t tmp[256];
			int     n = 0;

			while (*p)
			{
				while (*p && isspace((unsigned char)*p))
					p++;
				if (!*p) break;

				char byte_str[3] = {0};
				if (!isxdigit((unsigned char)p[0]) ||
					!isxdigit((unsigned char)p[1]))
				{
					log_msg(LOG_E, "Bad .code byte at line %u: '%s'\n",
							line_no, p);
					free(inst);
					return ERROR;
				}
				byte_str[0] = p[0];
				byte_str[1] = p[1];
				unsigned long v = strtoul(byte_str, NULL, 16);
				tmp[n++] = (uint8_t)v;
				p += 2;
			}

			inst->raw = malloc(n);
			memcpy(inst->raw, tmp, n);
			inst->raw_len = n;

			FT_LIST_ADD_LAST(&instr_list, inst);
			continue;
		}

		// if (header->prog_name[0] == '\0' || header->comment[0] == '\0')
		// {
		// 	log_msg(LOG_E, "Error: Missing .name or .comment before code at line %u\n", line_no);
		// 	return ERROR;
		// }

		/* Are we into a label line, or into instruction? */
		colon = strchr(line, LABEL_CHAR);
		space = line + strcspn(line, " \t\n\r\f\v");
		instr_part = NULL;
		def_label = NULL;

		/* If 'space' is before ':' -> instruction, else, label. */
		if (colon && colon > line && colon < space)
		{
			/* contains label. */
			*colon = '\0';
			// label_name = line;

			def_label = m_new_label(line);
			if (!def_label)
			{
				log_msg(LOG_E, "Error: Invalid label name '%s' at line %u\n", line, line_no);
				/* handle error */
				return ERROR;
			}

			def_label->line_no = line_no;
			/* what remains after ':' is potential instruction */
			instr_part = m_skip_spaces(colon + 1);
			log_msg(LOG_I, "Found label '%s' at line %u\n", def_label->name, line_no);
		}
		else
		{
			instr_part = line;
			log_msg(LOG_D, "No label found at line %u\n", line_no);
		}


		if (*instr_part != '\0' && *instr_part != COMMENT_CHAR)
		{
			inst = m_new_instruction(instr_part, line_no);
			if (!inst)
			{
				log_msg(LOG_E, "Error: Invalid instruction at line %u\n", line_no);
				return ERROR;
			}

			if (def_label)
			{
				inst->label = def_label->name;

				FT_LIST_ADD_LAST(&label_list, def_label);
			}

			FT_LIST_ADD_LAST(&instr_list, inst);
		}
		else if (def_label)
		{
			/* line with only label */
			FT_LIST_ADD_LAST(&label_list, def_label);
		}

	}

	log_msg(LOG_I,"Labels:\n");
	log_msg(LOG_I, "*****************************************************************\n");
	t_label* l_label = label_list;
	while (l_label)
	{
		log_msg(LOG_I,"  %s at offset %d\n", l_label->name, l_label->offset);
		l_label = FT_LIST_GET_NEXT(&label_list, l_label);
	}

	m_handle_file(NULL, FILE_CLOSE);
	int prog_size = 0;
	prog_size = m_compute_offsets(instr_list, label_list);
	normalize_args(instr_list, label_list);
	log_msg(LOG_I, "Finished parsing file %s\n", filename);
	log_msg(LOG_I, "#########################################################\n");
	m_print_instrs(instr_list, label_list);

	p_st->label_list = label_list;
	p_st->instr_list =  instr_list;
	// return 0;


	t_instr* l_inst = instr_list;
	uint8_t code[65536]; /* max size */
	memset(code, 0, sizeof(code));
	while (l_inst)
	{
		encode_instruction(l_inst, code);

		l_inst = FT_LIST_GET_NEXT(&instr_list, l_inst);
	}

	char outname[256];
	char* extension;
	extension = strrchr(filename, '.');
	if (extension)
		snprintf(outname, sizeof(outname), "%.*s.cor", (int)(extension - filename), filename);
	else
		snprintf(outname, sizeof(outname), "%s.cor", filename);

	write_cor_file(outname, header, code, prog_size);
    exit(0); /* TODO fix leaks. */

	return 0;
}
