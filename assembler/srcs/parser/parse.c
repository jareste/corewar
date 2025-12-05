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

static t_instr* m_new_instruction(char* instr_text, int line_no)
{
	t_instr* inst;
	t_op* op;
	int i;
	char* end;
	size_t len;
	char* arg_str;
	char* token;
	char *comment;
	int mask;

	inst = NEW(t_instr, 1);

	/* find operation */
	end = instr_text + strcspn(instr_text, " \t\n\r\f\v");
	len = end - instr_text;

	/**/
	op = m_find_op(instr_text, len);
	if (!op)
	{
		log_msg(LOG_E,
				"Error: Unknown operation '%.*s' at line %d\n",
				(int)len, instr_text, line_no);
		goto error;
	}

	log_msg(LOG_I, "Instruction '%s' at line %d\n", op->name, line_no);

	inst->line_no = line_no;
	inst->op = op;
	inst->arg_count = 0;
	memset(inst->args, 0, sizeof(inst->args));

	arg_str = m_skip_spaces(end);
	comment = strchr(arg_str, COMMENT_CHAR);
	if (comment)
		*comment = '\0';
	token = strtok(arg_str, ",");
	log_msg(LOG_D, "  Parsing arguments: '%s'\n", arg_str);
	while (token && inst->arg_count < 3)
	{
		token = m_skip_spaces(token);

		if (strchr(token, '+') || strchr(token, '-'))
		{
			memset(&inst->args[inst->arg_count], 0, sizeof(t_arg));

			if (token[0] == DIRECT_CHAR)        // '%'
				inst->args[inst->arg_count].type = ARG_DIR;
			else
				inst->args[inst->arg_count].type = ARG_IND;

			inst->args[inst->arg_count].expr = strdup(token);
			if (!inst->args[inst->arg_count].expr)
				goto error;

			log_msg(LOG_D, "  Extended arg[%d]: '%s'\n",
					inst->arg_count, token);

			inst->arg_count++;
			token = strtok(NULL, ",");
			continue;
		}


		if (m_parse_arg_token(token, &inst->args[inst->arg_count]) != 0)
		{
			log_msg(LOG_E,
					"Error: Invalid argument '%s' for '%s' at line %d\n",
					token, op->name, line_no);
			goto error;
		}

		log_msg(LOG_D, "  Arg[%d]: '%s'\n",
				inst->arg_count, token);
		inst->arg_count++;

		token = strtok(NULL, ",");
	}

	if (inst->arg_count != op->nb_params)
	{
		log_msg(LOG_E,
				"Error: Expected %d arguments for '%s' at line %d, got %d\n",
				op->nb_params, op->name, line_no, inst->arg_count);
		goto error;
	}

	for (i = 0; i < inst->arg_count; ++i)
	{
		mask = m_mask_for_arg_type(inst->args[i].type);
		if (!(inst->op->param_types[i] & mask))
		{
			log_msg(LOG_E,
					"Error: Wrong type for arg %d of '%s' at line %d\n",
					i + 1, inst->op->name, line_no);
			goto error;
		}
	}

	return inst;

error:
	for (int j = 0; j < inst->arg_count; ++j)
	{
		if (inst->args[j].type == ARG_LABEL_DIR ||
			inst->args[j].type == ARG_LABEL_IND)
			free(inst->args[j].u.label);
	}
	free(inst);
	return NULL;
}

t_label *find_label(t_label *label_list, const char *name)
{
	t_label *lab;

	lab = label_list;
	while (lab)
	{
		if (strcmp(lab->name, name) == 0)
			return lab;
		lab = FT_LIST_GET_NEXT(&label_list, lab);
	}
	return NULL;
}

static int eval_expr(const char *expr, t_instr *inst, t_arg_type type, int32_t *out, t_label *label_list)
{
	const char *s = expr;
	int64_t acc = 0;
	int sign = +1;
	int has_label = 0;
	int len;
	int64_t term;
	char name[128];
	char *endptr;
	long long v;
	t_label *lab;

	(void)inst;

	log_msg(LOG_D, "Evaluating expr '%s'\n", expr);
	log_msg(LOG_D, "  Initial acc=%lld\n", acc);
	log_msg(LOG_D, " instruction %s[%d] at offset %d\n", inst->op->name, inst->line_no, inst->offset);

	if (*s == DIRECT_CHAR)
		s++;

	while (*s)
	{
		while (*s && isspace((unsigned char)*s))
			s++;

		if (*s == '+' || *s == '-')
		{
			sign = (*s == '+') ? +1 : -1;
			s++;
			continue;
		}

		if (*s == '\0')
			break;

		term = 0;

		if (*s == LABEL_CHAR)
		{
			log_msg(LOG_D, "  Parsing label term in expr\n");
			s++;
			len = 0;

			while (s[len] && strchr(LABEL_CHARS, s[len]) != NULL && len < (int)sizeof(name) - 1)
			{
				name[len] = s[len];
				len++;
			}
			name[len] = '\0';
			s += len;

			lab = find_label(label_list, name);
			if (!lab)
			{
				log_msg(LOG_E, "Error: Undefined label '%s' in expr '%s'\n", name, expr);
				return -1;
			}
			term = lab->offset;
			has_label = 1;
		}
		else
		{
			// errno = 0;
			// v = strtoll(s, &endptr, 10);
			// if (errno != 0 || endptr == s)
			// {
			//     log_msg(LOG_E, "Error: Bad number in expr '%s'\n", expr);
			//     return -1;
			// }
			endptr = (char*)s;
			endptr += m_parse_num32(s, (int32_t *)&v);
			if (endptr == s || endptr < s)
			{
				log_msg(LOG_E, "Error: Bad number in expr '%s'\n", expr);
				return -1;
			}
			term = v;
			s = endptr;
		}

		acc += sign * term;
		sign = +1;
	}

	log_msg(LOG_D, "  Final acc=%lld\n", acc);
	if ((m_is_pc_relative_op(inst->op) || (type == ARG_IND)) && has_label)
		acc -= inst->offset;

	log_msg(LOG_D, "  Final acc=%lld\n", acc);
	*out = (int32_t)acc;
	return 0;
}

// static inline int arg_size(const t_instr *inst, int i)
// {
//     switch (inst->args[i].type) {
//         case ARG_REG: return 1;
//         case ARG_DIR:
//         case ARG_LABEL_DIR: return inst->op->has_idx ? 2 : 4;
//         case ARG_IND:
//         case ARG_LABEL_IND: return 2;
//     }
//     return 0;
// }

static void m_normalize_args(t_instr* inst_list, t_label* label_list)
{
	t_instr* inst;
	t_label* label;
	int i;
	bool found;
	int32_t val;

	inst = inst_list;
	while (inst)
	{
		for (i = 0; i < inst->arg_count; ++i)
		{

			if (inst->args[i].expr)
			{
				if (eval_expr(inst->args[i].expr, inst, inst->args[i].type, &val, label_list) != 0)
				{
					ft_assert(false, "Error in extended expression");
				}
				inst->args[i].u.value = val;
				free(inst->args[i].expr);
				inst->args[i].expr = NULL;
				continue;
			}

			if (inst->args[i].type == ARG_LABEL_DIR ||
				inst->args[i].type == ARG_LABEL_IND)
			{
				label = label_list;
				found = false;
				while (label)
				{
					if (strcmp(label->name, inst->args[i].u.label) == 0)
					{
						inst->args[i].type = (inst->args[i].type == ARG_LABEL_DIR) ? ARG_DIR : ARG_IND;
						log_msg(LOG_D, "Normalized label '%s'", inst->args[i].u.label);
						free(inst->args[i].u.label);

						if (m_is_pc_relative_op(inst->op) || (inst->args[i].type == ARG_IND) || (strcmp(inst->op->name, "ld") == 0))
							inst->args[i].u.value = label->offset - inst->offset;
						else
						{
							inst->args[i].u.value = label->offset;
						}

						log_msg(LOG_D, " to value %d at line %d\n",
								inst->args[i].u.value, inst->line_no);
						found = true;
						break;
					}
					label = FT_LIST_GET_NEXT(&label_list, label);
				}
				if (!found)
				{
					log_msg(LOG_E,
							"Error: Undefined label '%s' at line %d\n",
							inst->args[i].u.label, inst->line_no);
					
					/* label not found. wrong code. */
					ft_assert(false, "Undefined label");
				}
			}
		}
		inst = FT_LIST_GET_NEXT(&inst_list, inst);
	}
}

int parse_file(const char* filename, t_header* header)
{
	FILE* file;
	uint32_t line_no = 0;
	char buffer[256];
	char* line;
	char* label_name;
	char* colon;
	char* space;
	char* instr_part;
	t_label* def_label;
	t_instr *inst;

	t_label* label_list = NULL;
	t_instr *instr_list = NULL;

	file = fopen(filename, "r");
	if (!file)
	{
		log_msg(LOG_E, "Error: Could not open file %s\n", filename);
		return ERROR;
	}

	while (!feof(file))
	{
		line_no++;
		if (fgets(buffer, sizeof(buffer), file) == NULL)
		{
			if (ferror(file))
			{
				log_msg(LOG_E, "Error: Could not read from file %s\n", filename);
				fclose(file);
				return ERROR;
			}
			break;
		}

		/* remove '\n' at the end of the line */
		buffer[strcspn(buffer, "\n")] = '\0';

		line = m_skip_spaces(buffer);
		if (*line == '\0' || *line == COMMENT_CHAR)
			continue;


		log_msg(LOG_D, "Read line[%u] '%s'\n", line_no, buffer);
		log_msg(LOG_D, "%d, %d\n", strncmp(buffer, ".code", 5), isspace((unsigned char)buffer[5]));

		/* We got name or comment? */
		if (strstr(buffer, NAME_CMD_STRING))
		{
			sscanf(buffer, NAME_CMD_STRING" \"%[^\"]\"", header->prog_name);
			log_msg(LOG_I, "Program Name: '%s'\n", header->prog_name);
			continue;
		}
		else if (strstr(buffer, COMMENT_CMD_STRING))
		{
			sscanf(buffer, COMMENT_CMD_STRING" \"%[^\"]\"", header->comment);
			log_msg(LOG_I, "Program Comment: '%s'\n", header->comment);
			continue;
		}
		else if (strstr(buffer, ".extend"))
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
			label_name = line;

			def_label = m_new_label(label_name);
			if (!def_label)
			{
				log_msg(LOG_E, "Error: Invalid label name '%s' at line %u\n", label_name, line_no);
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

	fclose(file);
	int prog_size = 0;
	prog_size = m_compute_offsets(instr_list, label_list);
	m_normalize_args(instr_list, label_list);
	log_msg(LOG_I, "Finished parsing file %s\n", filename);
	log_msg(LOG_I, "#########################################################\n");
	m_print_instrs(instr_list, label_list);

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
