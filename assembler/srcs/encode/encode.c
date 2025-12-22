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
#include <libft.h>
#include "../asm.h"
#include "log.h"
#include "encode_internal.h"

void	m_write_name(int fd, t_header *header, uint8_t *zero4)
{
	uint32_t	magic;
	uint32_t	tmp;
	char		name[PROG_NAME_LENGTH + 1];

	magic = COREWAR_EXEC_MAGIC;
	tmp = ((magic >> 24) & 0xFF) | ((magic >> 8) & 0xFF00)
		| ((magic << 8) & 0xFF0000) | ((magic << 24) & 0xFF000000);
	if (write(fd, &tmp, 4) != 4)
	{
		close(fd);
		exit(ERROR);
	}
	log_msg(LOG_E, "Writing program name: '%s'\n", header->prog_name);
	ft_strncpy(name, header->prog_name, PROG_NAME_LENGTH + 1);
	if (write(fd, name, PROG_NAME_LENGTH) != PROG_NAME_LENGTH)
	{
		close(fd);
		exit(ERROR);
	}
	if (write(fd, zero4, 4) != 4)
	{
		close(fd);
		exit(ERROR);
	}
}

void	m_write_size_comment(int fd, t_header *header, int prog_size,
					uint8_t *zero4)
{
	uint8_t	buf[4];
	char	comment[COMMENT_LENGTH + 1];

	buf[0] = (prog_size >> 24) & 0xFF;
	buf[1] = (prog_size >> 16) & 0xFF;
	buf[2] = (prog_size >> 8) & 0xFF;
	buf[3] = (prog_size) & 0xFF;
	if (write(fd, buf, 4) != 4)
	{
		close(fd);
		exit(ERROR);
	}
	ft_strncpy(comment, header->comment, COMMENT_LENGTH + 1);
	log_msg(LOG_E, "Writing comment: '%s'\n", comment);
	if (write(fd, comment, COMMENT_LENGTH) != COMMENT_LENGTH)
	{
		close(fd);
		exit(ERROR);
	}
	if (write(fd, zero4, 4) != 4)
	{
		close(fd);
		exit(ERROR);
	}
}

void	m_write_code(int fd, uint8_t *code, int prog_size)
{
	if (write(fd, code, prog_size) != prog_size)
	{
		close(fd);
		exit(ERROR);
	}
}

int	write_cor_file(const char *outname, t_header *header, uint8_t *code,
					int prog_size)
{
	int		fd;
	uint8_t	zero4[4];

	fd = open(outname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (ERROR);
	ft_memset(zero4, 0, 4);
	m_write_name(fd, header, zero4);
	m_write_size_comment(fd, header, prog_size, zero4);
	m_write_code(fd, code, prog_size);
	close(fd);
	return (0);
}

int	encode_instruction_list(t_instr *inst_list, uint8_t **code)
{
	t_instr	*inst;

	*code = ft_malloc(65536 * sizeof(uint8_t));
	inst = inst_list;
	while (inst)
	{
		encode_instruction(inst, *code);
		inst = FT_LIST_GET_NEXT(&inst_list, inst);
	}
	return (0);
}
