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
#include "log.h"
#include "ft_printf.h"
#include "../corewar.h"
#include "../operations/operations.h"
#include "decode.h"

void	m_read_header_champ(t_champ *champ,
	uint8_t header_buf[4 + PROG_NAME_LENGTH + 4 + 4 + COMMENT_LENGTH + 4])
{
	uint32_t	magic;

	magic = (header_buf[0] << 24) | (header_buf[1] << 16)
		| (header_buf[2] << 8) | header_buf[3];
	if (magic != COREWAR_EXEC_MAGIC)
	{
		ft_dprintf(2, "Error: Invalid magic number in file\n");
		exit(1);
	}
	memcpy(champ->name, &header_buf[4], PROG_NAME_LENGTH);
	champ->name[PROG_NAME_LENGTH] = '\0';
	log_msg(LOG_I, "Program Name: '%s'\n", champ->name);
	champ->size = (header_buf[PROG_NAME_LENGTH + 8] << 24)
		| (header_buf[PROG_NAME_LENGTH + 9] << 16)
		| (header_buf[PROG_NAME_LENGTH + 10] << 8)
		| header_buf[PROG_NAME_LENGTH + 11];
	log_msg(LOG_I, "Program Size: %u %zu bytes\n", champ->size, CHAMP_MAX_SIZE);
	if (champ->size > CHAMP_MAX_SIZE)
	{
		ft_dprintf(2, "Error: Program size exceeds maximum size\n");
		exit(1);
	}
	memcpy(champ->comment, &header_buf[PROG_NAME_LENGTH + 12], COMMENT_LENGTH);
	champ->comment[COMMENT_LENGTH] = '\0';
	log_msg(LOG_I, "Comment: '%s'\n", champ->comment);
}

static ssize_t	m_read_file(int fd, void *buf, size_t count)
{
	size_t	total;
	ssize_t	r;
	uint8_t	*p;

	total = 0;
	p = buf;
	while (total < count)
	{
		r = read(fd, p + total, count - total);
		if (r < 0)
			return (-1);
		if (r == 0)
			break ;
		total += (size_t)r;
	}
	return ((ssize_t)total);
}

/* header_buf -> magic + prog_name + padding + prog_size + comment + padding */
int	decode_file(const char *filename, t_champ *champ)
{
	int		fd;
	ssize_t	read_size;
	uint8_t	header_buf[4 + PROG_NAME_LENGTH + 4 + 4 + COMMENT_LENGTH + 4];

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		ft_dprintf(2, "Error: Could not open file %s\n", filename);
		exit(1);
	}
	read_size = m_read_file(fd, header_buf, sizeof(header_buf));
	if (read_size < 0 || (size_t)read_size < sizeof(header_buf))
	{
		ft_dprintf(2, "Error: Could not read header from file %s\n", filename);
		exit(1);
	}
	m_read_header_champ(champ, header_buf);
	read_size = m_read_file(fd, champ->code, champ->size);
	if (read_size < 0 || (size_t)read_size < champ->size)
	{
		ft_dprintf(2, "Error: sizes differ %zu %zd\n", champ->size, read_size);
		exit(1);
	}
	close(fd);
	return (SUCCESS);
}
