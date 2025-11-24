#ifndef ASM_H
# define ASM_H
# include "../../op/op.h"
#include <ft_malloc.h>
#include <ft_list.h>
#include <error_codes.h>
#include <stdint.h>
// #include "process.h"

typedef struct
{
    int     id;
    char    name[PROG_NAME_LENGTH + 1];
    char    comment[COMMENT_LENGTH + 1];
    size_t  size;
    uint8_t code[CHAMP_MAX_SIZE];
} t_champ;

typedef struct
{
    list_item_t  l;
    int     id;
    int     regs[REG_NUMBER];
    int     pc;
    int     carry;
    int     last_live_cycle;
    int     op_wait; /* cycles to actually execute this opcode */
    int     opcode;

} t_proc;

typedef struct
{
    uint8_t memory[MEM_SIZE];
    t_proc* procs;
    t_champ champs[MAX_PLAYERS];
    int cycle;
    int cycle_to_die;
    int last_check_cycle;
    int lives_in_period;

    int last_alive_player;
} t_vm;

typedef struct s_arg
{
    int type;
    int value;
} t_arg;

#endif
