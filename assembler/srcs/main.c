#include <stdio.h>
#include <string.h>
#include "asm.h"
#include "parser/parse.h"
#include "log.h"
#include <stdint.h>
#include "encode/encode.h"
#include <libft.h>
#include <ft_printf.h>

int	main(int argc, char **argv)
{
	t_header 		h;
	t_parser_state	parser_state;

	if (argc != 2)
	{
		ft_dprintf(2, "Usage: %s <source_file>\n", argv[0]);
		return 1;
	}

	if (log_init() != 0)
	{
		ft_dprintf(2, "Error: Could not initialize logging system\n");
		return 1;
	}

	memset(&h, 0, sizeof(h));
	memset(&parser_state, 0, sizeof(parser_state));
	if (parse_file(argv[1], &h, &parser_state) != 0)
	{
		ft_dprintf(2, "Error: Failed to parse file %s\n", argv[1]);
		return 1;
	}

	uint8_t *code;
	encode_instruction_list(parser_state.i_l, &code);
	ft_assert(code, "Encoding failed");

	char outname[256];
	char* extension;
	extension = ft_strrchr(argv[1], '.');
	memset(outname, 0, sizeof(outname));
	if (extension)
	{
		ft_strncpy(outname, argv[1], extension - argv[1]);
		ft_strncpy(outname + (extension - argv[1]), ".cor", 4);
	}
	else
	{
		ft_strncpy(outname, argv[1], sizeof(outname) - 5);
		ft_strncpy(outname + strlen(outname), ".cor", 4);
	}

	write_cor_file(outname, &h, code, parser_state.prog_size);
	free(code);

	ft_printf("Successfully assembled %s\n", argv[1]);
	exit(0);
	return 0;
}
