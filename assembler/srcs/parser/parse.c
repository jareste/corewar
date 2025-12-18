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
	static int	m_fd = -1;

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

void	m_print_labels(t_label *label_list)
{
	t_label	*lab;

	lab = label_list;
	log_msg(LOG_I, "Labels:\n");
	log_msg(LOG_I, "******************************************************\n");
	while (lab)
	{
		log_msg(LOG_I, "  %s at offset %d\n", lab->name, lab->offset);
		lab = FT_LIST_GET_NEXT(&label_list, lab);
	}
}

int	parse_file(const char *filename, t_header *header, t_parser_state *p_st)
{
	uint32_t	line_no;
	char		*line;
	int			prog_size;

	line_no = 0;
	m_handle_file(filename, FILE_CREATE);
	while (m_read_line(&line, &line_no) == 0)
	{
		log_msg(LOG_D, "Read line[%u] '%s'\n", line_no, line);
		if (check_special_line(line, line_no, header, p_st) != 0)
			continue ;
		handle_label_colon(line, line_no, p_st);
	}
	m_print_labels(p_st->l_l);
	m_handle_file(NULL, FILE_CLOSE);
	prog_size = m_compute_offsets(p_st->i_l, p_st->l_l);
	normalize_args(p_st->i_l, p_st->l_l);
	log_msg(LOG_I, "Finished parsing file %s\n", filename);
	log_msg(LOG_I, "######################################################\n");
	m_print_instrs(p_st->i_l, p_st->l_l);
	p_st->prog_size = prog_size;
	return (0);
}
