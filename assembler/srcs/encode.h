#ifndef ENCODE_H
# define ENCODE_H

#include <stdint.h>

int encode_instruction(t_instr *inst, uint8_t *code);

int write_cor_file(const char *outname, t_header *header, uint8_t *code, int prog_size);

#endif /* ENCODE_H */
