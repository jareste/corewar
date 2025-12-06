#include <stdio.h>
#include "asm.h"
#include "parser/parse.h"
#include "log.h"

int main(int argc, char **argv)
{
    t_header h;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    if (log_init() != 0)
    {
        fprintf(stderr, "Error: Could not initialize logging system\n");
        return 1;
    }

    if (parse_file(argv[1], &h) != 0)
    {
        fprintf(stderr, "Error: Failed to parse file %s\n", argv[1]);
        return 1;
    }


    printf("Successfully assembled %s\n", argv[1]);
    exit(0);
    return 0;
}
