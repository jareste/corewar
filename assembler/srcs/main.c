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

#include <stdio.h>
#include <string.h>
#include "asm.h"
#include "parser/parse.h"
#include "log.h"
#include <stdint.h>
#include "encode/encode.h"
#include <libft.h>
#include <ft_printf.h>

int	m_initialize(t_header *h, t_parser_state *ps, uint8_t **code, char **argv)
{
	if (log_init() != 0)
	{
		ft_dprintf(2, "Error: Could not initialize logging system\n");
		return (1);
	}
	memset(h, 0, sizeof(t_header));
	memset(ps, 0, sizeof(t_parser_state));
	if (parse_file(argv[1], h, ps) != 0)
	{
		ft_dprintf(2, "Error: Failed to parse file %s\n", argv[1]);
		return (1);
	}
	encode_instruction_list(ps->i_l, code);
	ft_assert(*code, "Encoding failed");
	return (0);
}

void	m_create_out_file(const char *av, t_header *h,
			uint8_t *code, t_parser_state *ps)
{
	char	outname[256];
	char	*extension;

	extension = ft_strrchr(av, '.');
	memset(outname, 0, sizeof(outname));
	if (extension)
	{
		ft_strncpy(outname, av, extension - av);
		ft_strncpy(outname + (extension - av), ".cor", 4);
	}
	else
	{
		ft_strncpy(outname, av, sizeof(outname) - 5);
		ft_strncpy(outname + strlen(outname), ".cor", 4);
	}
	write_cor_file(outname, h, code, ps->prog_size);
	free(code);
}

int	main(int argc, char **argv)
{
	t_header		h;
	t_parser_state	parser_state;
	uint8_t			*code;

	if (argc != 2)
	{
		ft_dprintf(2, "Usage: %s <source_file>\n", argv[0]);
		return (1);
	}
	if (m_initialize(&h, &parser_state, &code, argv) != 0)
	{
		log_close();
		return (1);
	}
	m_create_out_file(argv[1], &h, code, &parser_state);
	log_close();
	ft_printf("Successfully assembled %s\n", argv[1]);
	exit(0);
	return (0);
}
