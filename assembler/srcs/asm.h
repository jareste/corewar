#ifndef ASM_H
# define ASM_H
# include "../../op/op.h"
#include <ft_malloc.h>
#include <ft_list.h>
#include <error_codes.h>

typedef enum e_arg_kind
{
    ARG_REG,
    ARG_DIR,
    ARG_IND,
    ARG_LABEL_DIR,
    ARG_LABEL_IND,
} t_arg_type;

typedef struct s_arg
{
    t_arg_type  type;
    union
    {
        int         value;
        char*       label;
    } u;
    char* expr;

} t_arg;

typedef struct s_instr
{
    list_item_t l;
    t_op*       op;
    int         line_no;
    int         offset; /* offset from start of code */
    t_arg       args[3];
    int         arg_count;
    char*       label; /* label defined on this line (if any) */
} t_instr;

typedef struct s_label
{
    list_item_t l;
    char*       name;
    int         offset; /* byte offset where label points */
    int         line_no;
} t_label;

#endif
