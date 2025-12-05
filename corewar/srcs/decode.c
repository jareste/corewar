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
#include "corewar.h"
#include "operations.h"
#include "log.h"

 /* header_buf -> magic + prog_name + padding + prog_size + comment + padding */
int	decode_file(const char* filename, t_champ* champ)
{
	FILE*	file;
	size_t	read_size;
	uint8_t	header_buf[4 + PROG_NAME_LENGTH + 4 + 4 + COMMENT_LENGTH + 4];
	uint32_t magic;

	file = fopen(filename, "rb");
	if (!file)
	{
		log_msg(LOG_LEVEL_ERROR, "Error: Could not open file %s\n", filename);
		return ERROR;
	}

	/* read header */
	read_size = fread(header_buf, 1, sizeof(header_buf), file);
	if (read_size < sizeof(header_buf))
	{
		log_msg(LOG_LEVEL_ERROR, "Error: Could not read header from file %s\n", filename);
		fclose(file);
		return ERROR;
	}

	/* check Magic number */
	magic = (header_buf[0] << 24) | (header_buf[1] << 16) | (header_buf[2] << 8) | header_buf[3];
	if (magic != COREWAR_EXEC_MAGIC)
	{
		log_msg(LOG_LEVEL_ERROR, "Error: Invalid magic number in file %s\n", filename);
		fclose(file);
		return ERROR;
	}

	/* read program name */
	memcpy(champ->name, &header_buf[4], PROG_NAME_LENGTH);
	champ->name[PROG_NAME_LENGTH] = '\0';
	log_msg(LOG_LEVEL_INFO, "Program Name: '%s'\n", champ->name);
	/* read program size */
	champ->size = (header_buf[PROG_NAME_LENGTH + 8] << 24) |
				(header_buf[PROG_NAME_LENGTH + 9] << 16) |
				(header_buf[PROG_NAME_LENGTH + 10] << 8) |
				header_buf[PROG_NAME_LENGTH + 11];
	log_msg(LOG_LEVEL_INFO, "Program Size: %u %zu bytes\n", champ->size, CHAMP_MAX_SIZE);

	if (champ->size > CHAMP_MAX_SIZE)
	{
		log_msg(LOG_LEVEL_ERROR, "Error: Program size exceeds maximum in file %s\n", filename);
		fclose(file);
		return ERROR;
	}

	/* read comment */
	memcpy(champ->comment, &header_buf[PROG_NAME_LENGTH + 12], COMMENT_LENGTH);
	champ->comment[COMMENT_LENGTH] = '\0';
	log_msg(LOG_LEVEL_INFO, "Comment: '%s'\n", champ->comment);

	/* read program code */
	read_size = fread(champ->code, 1, champ->size, file);
	if (read_size < champ->size)
	{
		log_msg(LOG_LEVEL_ERROR, "Error: sizes differ %zu %zu\n", champ->size, read_size);
		log_msg(LOG_LEVEL_ERROR, "Error: Could not read program code from file %s\n", filename);
		fclose(file);
		return ERROR;
	}

	fclose(file);
	return SUCCESS;
}

int	read_n_bytes(uint8_t *mem, int addr, int n)
{
	int	v;
	int	i;

	v = 0;
	i = 0;
	while (i < n)
	{
		v <<= 8;
		v |= mem[mem_addr(addr + i)];
		i++;
	}
	return (v);
}

void	write_n_bytes(uint8_t *mem, int addr, int value, int n)
{
	int	i;
	int	pos;

	i = n - 1;
	while (i >= 0)
	{
		pos = mem_addr(addr + (n - 1 - i));
		mem[pos] = (uint8_t)((value >> (i * 8)) & 0xFF);
		i--;
	}
}

int	arg_size(const t_op *op, int param_type)
{
	if (param_type == PARAM_REGISTER)
		return (1);
	if (param_type == PARAM_INDIRECT)
		return (IND_SIZE);
	if (param_type == PARAM_DIRECT)
	{
		if (op->has_idx)
			return (IND_SIZE);
		return (DIR_SIZE);
	}
	return (0);
}

/* apply_idx_mod -> indirect */
int	get_arg_val(t_vm *vm, t_proc *p, t_arg *a, int apply_idx_mod)
{
	int	addr;

	if (a->type == PARAM_REGISTER)
		return (p->regs[a->value]);
	if (a->type == PARAM_DIRECT)
		return (a->value);
	if (apply_idx_mod)
		addr = p->pc + (a->value % IDX_MOD);
	else
		addr = p->pc + a->value;
	addr = mem_addr(addr);
	return (read_n_bytes(vm->memory, addr, 4));
}
