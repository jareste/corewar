#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "asm.h"
#include "log.h"

int encode_instruction(t_instr *inst, uint8_t *code)
{
    int bits;
    int i;
    int size;
    int val;
    int b;
    uint8_t acb;
    int pos = inst->offset; /* Where it's written in the code buffer */

    if (!inst->op)
    {
        /* .code raw data */
        memcpy(&code[pos], inst->raw, inst->raw_len);
        return pos + inst->raw_len;
    }

    code[pos++] = inst->op->opcode; /* write opcode */

    /* argument coding byte (if any) */
    if (inst->op->has_pcode)
    {
        acb = 0;
        for (i = 0; i < inst->arg_count; ++i)
        {
            switch (inst->args[i].type)
            {
                case ARG_REG: bits = REG_CODE; break; /* 01 */
                case ARG_DIR: bits = DIR_CODE; break; /* 10 */
                case ARG_IND: bits = IND_CODE; break; /* 11 */
                default: bits = 0; break;
            }
            acb |= (bits << (6 - 2 * i));
        }
        code[pos++] = acb;
    }

    /* arguments */
    for (i = 0; i < inst->arg_count; ++i)
    {
        t_arg *a = &inst->args[i];
        size = 0;

        if (a->type == ARG_REG)
        {
            size = 1;
            code[pos++] = (uint8_t)a->u.value;
            continue;
        }

        if (a->type == ARG_DIR)
        {
            if (inst->op->has_idx)
                size = IND_SIZE; /* 2 */
            else
                size = DIR_SIZE; /* 4 */
        }
        else if (a->type == ARG_IND)
        {
            size = IND_SIZE; /* 2 */
        }

        val = a->u.value; /* this may come from label offset */

        /* write big-endian integer of 'size' bytes */
        for (b = size - 1; b >= 0; --b)
        {
            code[pos + b] = (uint8_t)(val & 0xFF);
            val >>= 8;
        }
        pos += size;
    }

    /* print for debug */
    log_msg(LOG_LEVEL_INFO, "Encoded instruction '%s'[%d] at offset %d: ", inst->op->name, inst->line_no, inst->offset);
    for (i = inst->offset; i < pos; ++i)
    {
        log_msg(LOG_LEVEL_INFO, "%02X ", code[i]);
    }
    log_msg(LOG_LEVEL_INFO, "\n");
    return pos;
}

int write_cor_file(const char *outname, t_header *header, uint8_t *code, int prog_size)
{
    uint32_t magic = COREWAR_EXEC_MAGIC;
    uint8_t buf[4];
    int fd;
    char comment[COMMENT_LENGTH + 1] = {0};
    char name[PROG_NAME_LENGTH + 1] = {0};
    uint8_t zero4[4] = {0};

    fd = open(outname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return ERROR;

    /* Magic number big endian */
    buf[0] = (magic >> 24) & 0xFF;
    buf[1] = (magic >> 16) & 0xFF;
    buf[2] = (magic >>  8) & 0xFF;
    buf[3] = (magic      ) & 0xFF;
    if (write(fd, buf, 4) != 4)
    {
        close(fd);
        return ERROR;
    }

    /* program name */
    log_msg(LOG_LEVEL_ERROR, "Writing program name: '%s'\n", header->prog_name);
    strncpy(name, header->prog_name, PROG_NAME_LENGTH + 1);
    if (write(fd, name, PROG_NAME_LENGTH) != PROG_NAME_LENGTH)
    {
        close(fd);
        return ERROR;
    }

    /* null padding */
    if (write(fd, zero4, 4) != 4)
    {
        close(fd);
        return ERROR;
    }

    /* program size (big endian) */
    buf[0] = (prog_size >> 24) & 0xFF;
    buf[1] = (prog_size >> 16) & 0xFF;
    buf[2] = (prog_size >>  8) & 0xFF;
    buf[3] = (prog_size      ) & 0xFF;
    if (write(fd, buf, 4) != 4)
    {
        close(fd);
        return ERROR;
    }

    /* comment (2048 bytes) */
    strncpy(comment, header->comment, COMMENT_LENGTH + 1);
    log_msg(LOG_LEVEL_ERROR, "Writing comment: '%s'\n", comment);
    if (write(fd, comment, COMMENT_LENGTH) != COMMENT_LENGTH)
    {
        close(fd);
        return ERROR;
    }
    if (write(1, comment, COMMENT_LENGTH) != COMMENT_LENGTH)
    {
        close(fd);
        return ERROR;
    }

    /* more null padding */
    if (write(fd, zero4, 4) != 4)
    {
        close(fd);
        return ERROR;
    }
    /* program code */
    if (write(fd, code, prog_size) != prog_size)
    {
        close(fd);
        return ERROR;
    }

    close(fd);
    return 0;
}

