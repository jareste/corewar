#include "asm.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "log.h"
#include "encode.h"

static t_instr* m_instr = NULL;
static t_label* m_labels = NULL;
static int m_prog_size = 0;

static void m_print_instrs()
{
    t_instr* inst;
    t_label* label;
    int i;

    log_msg(LOG_LEVEL_INFO,"Instructions:\n");
    inst = m_instr;
    while (inst)
    {
        log_msg(LOG_LEVEL_INFO,"  Off[%d] Line %d: %s ", inst->offset, inst->line_no, inst->op->name);
        for (i = 0; i < inst->arg_count; ++i)
        {
            switch (inst->args[i].type)
            {
                case ARG_REG:
                    log_msg(LOG_LEVEL_INFO,"r%d", inst->args[i].u.value);
                    break;
                case ARG_DIR:
                    log_msg(LOG_LEVEL_INFO,"%%%d", inst->args[i].u.value);
                    break;
                case ARG_IND:
                    log_msg(LOG_LEVEL_INFO,"%d",  inst->args[i].u.value);
                    break;
                case ARG_LABEL_DIR:
                    log_msg(LOG_LEVEL_INFO,"%%:%s", inst->args[i].u.label);
                    break;
                case ARG_LABEL_IND:
                    log_msg(LOG_LEVEL_INFO,":%s",  inst->args[i].u.label);
                    break;
            }
            if (i < inst->arg_count - 1)
                log_msg(LOG_LEVEL_INFO,", ");
        }
        log_msg(LOG_LEVEL_INFO,"\n");

        inst = FT_LIST_GET_NEXT(&m_instr, inst);
    }

    log_msg(LOG_LEVEL_INFO,"Labels:\n");
    label = m_labels;
    while (label)
    {
        log_msg(LOG_LEVEL_INFO,"  %s at offset %d\n", label->name, label->offset);
        label = FT_LIST_GET_NEXT(&m_labels, label);
    }
}

static char *m_skip_spaces(char *s)
{
    while (*s && isspace((unsigned char)*s))
        s++;
    return s;
}

static t_label* m_new_label(const char *name)
{
    const char *p;
    t_label* label;

    if (!name || *name == '\0')
        return NULL;

    for (p = name; *p; ++p)
    {
        if (strchr(LABEL_CHARS, *p) == NULL)
            return NULL;
    }

    label = NEW(t_label, 1);
    label->name = strdup(name);

    return label;
}

static t_op *m_find_op(const char *name, size_t len)
{
    int i;

    for (i = 0; i <= REG_NUMBER; ++i)
    {
        if (op_tab[i].name &&
            strlen(op_tab[i].name) == len &&
            strncmp(op_tab[i].name, name, len) == 0)
        {
            return &op_tab[i];
        }
    }
    return NULL;
}

static int m_parse_int(const char *s, int *out)
{
    char *endptr;
    long val;

    errno = 0;
    val = strtol(s, &endptr, 10);
    if (errno != 0 || endptr == s || *endptr != '\0')
        return -1;
    if (val < INT_MIN || val > INT_MAX)
        return -1;
    *out = (int)val;
    return 0;
}

static int m_mask_for_arg_type(t_arg_type t)
{
    switch (t)
    {
        case ARG_REG:        return PARAM_REGISTER;
        case ARG_DIR:
        case ARG_LABEL_DIR:  return PARAM_DIRECT;
        case ARG_IND:
        case ARG_LABEL_IND:  return PARAM_INDIRECT;
        default:             return PARAM_UNKNOWN;
    }
}

static int m_parse_arg_token(const char *arg_str, t_arg *out)
{
    char *s = m_skip_spaces((char *)arg_str);
    size_t len = strlen(s);

    if (len == 0)
        return -1;

    if (s[0] == 'r') /* register */
    {
        int reg;
        if (len < 2)
            return -1;
        if (m_parse_int(s + 1, &reg) != 0)
            return -1;
        if (reg < 1 || reg > REG_NUMBER)
            return -1;

        out->type = ARG_REG;
        out->u.value = reg;
        return 0;
    }
    else if (s[0] == DIRECT_CHAR) /* % */
    {
        if (len < 2)
            return -1;
        if (s[1] == LABEL_CHAR) /* '%:label' */
        {
            out->type = ARG_LABEL_DIR;
            out->u.label = strdup(s + 2);
            if (!out->u.label)
                return -1;
            return 0;
        }
        else /* '%42' */
        {
            int val;
            if (m_parse_int(s + 1, &val) != 0)
                return -1;
            out->type = ARG_DIR;
            out->u.value = val;
            return 0;
        }
    }
    else /* indirect */
    {
        if (s[0] == LABEL_CHAR) /* ':label' */
        {
            out->type = ARG_LABEL_IND;
            out->u.label = strdup(s + 1);
            if (!out->u.label)
                return -1;
            return 0;
        }
        else /* '42' */
        {
            int val;
            if (m_parse_int(s, &val) != 0)
                return -1;
            out->type = ARG_IND;
            out->u.value = val;
            return 0;
        }
    }
}

static t_instr* m_new_instruction(char* instr_text, int line_no)
{
    t_instr* inst;
    t_op* op;
    int i;
    char* end;
    size_t len;
    char* arg_str;
    char* token;
    int mask;

    inst = NEW(t_instr, 1);

    /* find operation */
    end = instr_text + strcspn(instr_text, " \t\n\r\f\v");
    len = end - instr_text;

    /**/
    op = m_find_op(instr_text, len);
    if (!op)
    {
        log_msg(LOG_LEVEL_ERROR,
                "Error: Unknown operation '%.*s' at line %d\n",
                (int)len, instr_text, line_no);
        goto error;
    }

    log_msg(LOG_LEVEL_INFO, "Instruction '%s' at line %d\n", op->name, line_no);

    inst->line_no = line_no;
    inst->op = op;
    inst->arg_count = 0;
    memset(inst->args, 0, sizeof(inst->args));

    arg_str = m_skip_spaces(end);
    token = strtok(arg_str, ",");
    while (token && inst->arg_count < 3)
    {
        token = m_skip_spaces(token);

        if (m_parse_arg_token(token, &inst->args[inst->arg_count]) != 0)
        {
            log_msg(LOG_LEVEL_ERROR,
                    "Error: Invalid argument '%s' for '%s' at line %d\n",
                    token, op->name, line_no);
            goto error;
        }

        log_msg(LOG_LEVEL_DEBUG, "  Arg[%d]: '%s'\n",
                inst->arg_count, token);
        inst->arg_count++;

        token = strtok(NULL, ",");
    }

    if (inst->arg_count != op->nb_params)
    {
        log_msg(LOG_LEVEL_ERROR,
                "Error: Expected %d arguments for '%s' at line %d, got %d\n",
                op->nb_params, op->name, line_no, inst->arg_count);
        goto error;
    }

    for (i = 0; i < inst->arg_count; ++i)
    {
        mask = m_mask_for_arg_type(inst->args[i].type);
        if (!(inst->op->param_types[i] & mask))
        {
            log_msg(LOG_LEVEL_ERROR,
                    "Error: Wrong type for arg %d of '%s' at line %d\n",
                    i + 1, inst->op->name, line_no);
            goto error;
        }
    }

    return inst;

error:
    for (int j = 0; j < inst->arg_count; ++j)
    {
        if (inst->args[j].type == ARG_LABEL_DIR ||
            inst->args[j].type == ARG_LABEL_IND)
            free(inst->args[j].u.label);
    }
    free(inst);
    return NULL;
}

void m_compute_offsets()
{
    t_instr* inst;
    t_label* label;
    int offset = 0;

    inst = m_instr;
    while (inst)
    {
        inst->offset = offset;
        offset += 1; /* opcode */

        if (inst->op->has_pcode)
            offset += 1; /* pcode */

        for (int i = 0; i < inst->arg_count; ++i)
        {
            switch (inst->args[i].type)
            {
                case ARG_REG:
                    offset += 1;
                    break;
                case ARG_DIR:
                case ARG_LABEL_DIR:
                    offset += inst->op->has_idx ? 2 : 4;
                    break;
                case ARG_IND:
                case ARG_LABEL_IND:
                    offset += 2;
                    break;
            }
        }

        inst = FT_LIST_GET_NEXT(&m_instr, inst);
    }

    m_prog_size = offset;

    label = m_labels;
    while (label)
    {
        t_instr* target_inst = m_instr;
        while (target_inst)
        {
            if (target_inst->label && strcmp(target_inst->label, label->name) == 0)
            {
                label->offset = target_inst->offset;
                break;
            }
            target_inst = FT_LIST_GET_NEXT(&m_instr, target_inst);
        }
        label = FT_LIST_GET_NEXT(&m_labels, label);
    }
}

static inline int m_is_pc_relative_op(const t_op *op)
{
    return (op->opcode == 9 /* zjmp */
         || op->opcode == 12 /* fork */
         || op->opcode == 15 /* lfork */);
}

static void m_normalize_args()
{
    t_instr* inst;
    t_label* label;
    int i;
    bool found;

    inst = m_instr;
    while (inst)
    {
        for (i = 0; i < inst->arg_count; ++i)
        {
            if (inst->args[i].type == ARG_LABEL_DIR ||
                inst->args[i].type == ARG_LABEL_IND)
            {
                label = m_labels;
                found = false;
                while (label)
                {
                    if (strcmp(label->name, inst->args[i].u.label) == 0)
                    {
                        inst->args[i].type = (inst->args[i].type == ARG_LABEL_DIR) ? ARG_DIR : ARG_IND;
                        log_msg(LOG_LEVEL_DEBUG, "Normalized label '%s'", inst->args[i].u.label);
                        free(inst->args[i].u.label);

                        if (m_is_pc_relative_op(inst->op))
                            inst->args[i].u.value = label->offset - inst->offset;
                        else
                            inst->args[i].u.value = label->offset;

                        log_msg(LOG_LEVEL_DEBUG, " to value %d at line %d\n",
                                inst->args[i].u.value, inst->line_no);
                        found = true;
                        break;
                    }
                    label = FT_LIST_GET_NEXT(&m_labels, label);
                }
                if (!found)
                {
                    log_msg(LOG_LEVEL_ERROR,
                            "Error: Undefined label '%s' at line %d\n",
                            inst->args[i].u.label, inst->line_no);
                    
                    /* label not found. wrong code. */
                    ft_assert(false, "Undefined label");
                }
            }
        }
        inst = FT_LIST_GET_NEXT(&m_instr, inst);
    }
}

int parse_file(const char* filename, t_header* header)
{
    FILE* file;
    uint32_t line_no = 0;
    char buffer[256];
    char* line;
    char* label_name;
    char* colon;
    char* space;
    char* instr_part;
    t_label* def_label;
    t_instr *inst;

    file = fopen(filename, "r");
    if (!file)
    {
        log_msg(LOG_LEVEL_ERROR, "Error: Could not open file %s\n", filename);
        return ERROR;
    }

    while (!feof(file))
    {
        line_no++;
        if (fgets(buffer, sizeof(buffer), file) == NULL)
        {
            if (ferror(file))
            {
                log_msg(LOG_LEVEL_ERROR, "Error: Could not read from file %s\n", filename);
                fclose(file);
                return ERROR;
            }
            break;
        }

        /* remove '\n' at the end of the line */
        buffer[strcspn(buffer, "\n")] = '\0';

        log_msg(LOG_LEVEL_DEBUG, "Read line[%u] '%s'\n", line_no, buffer);

        /* We got name or comment? */
        if (strstr(buffer, NAME_CMD_STRING))
        {
            sscanf(buffer, NAME_CMD_STRING" \"%[^\"]\"", header->prog_name);
            log_msg(LOG_LEVEL_INFO, "Program Name: '%s'\n", header->prog_name);
            continue;
        }
        else if (strstr(buffer, COMMENT_CMD_STRING))
        {
            sscanf(buffer, COMMENT_CMD_STRING" \"%[^\"]\"", header->comment);
            log_msg(LOG_LEVEL_INFO, "Program Comment: '%s'\n", header->comment);
            continue;
        }

        line = m_skip_spaces(buffer);
        if (*line == '\0' || *line == COMMENT_CHAR)
            continue;

        /* Are we into a label line, or into instruction? */
        colon = strchr(line, LABEL_CHAR);
        space = line + strcspn(line, " \t\n\r\f\v");
        instr_part = NULL;
        def_label = NULL;

        /* If 'space' is before ':' -> instruction, else, label. */
        if (colon && colon > line && colon < space)
        {
            /* contains label. */
            *colon = '\0';
            label_name = line;

            def_label = m_new_label(label_name);
            if (!def_label)
            {
                log_msg(LOG_LEVEL_ERROR, "Error: Invalid label name '%s' at line %u\n", label_name, line_no);
                /* handle error */
                return ERROR;
            }

            /* what remains after ':' is potential instruction */
            instr_part = m_skip_spaces(colon + 1);
            log_msg(LOG_LEVEL_INFO, "Found label '%s' at line %u\n", def_label->name, line_no);
        }
        else
        {
            instr_part = line;
            log_msg(LOG_LEVEL_DEBUG, "No label found at line %u\n", line_no);
        }


        if (*instr_part != '\0' && *instr_part != COMMENT_CHAR)
        {
            inst = m_new_instruction(instr_part, line_no);
            if (!inst)
            {
                log_msg(LOG_LEVEL_ERROR, "Error: Invalid instruction at line %u\n", line_no);
                return ERROR;
            }

            if (def_label)
            {
                inst->label = def_label->name;

                FT_LIST_ADD_LAST(&m_labels, def_label);
            }

            FT_LIST_ADD_LAST(&m_instr, inst);
        }
    }

    fclose(file);
    m_compute_offsets();
    m_normalize_args();
    log_msg(LOG_LEVEL_INFO, "Finished parsing file %s\n", filename);
    log_msg(LOG_LEVEL_INFO, "#########################################################\n");
    m_print_instrs();


    t_instr* l_inst = m_instr;
    uint8_t code[65536]; /* max size */
    memset(code, 0, sizeof(code));
    while (l_inst)
    {
        encode_instruction(l_inst, code);

        l_inst = FT_LIST_GET_NEXT(&m_instr, l_inst);
    }

    write_cor_file("output.cor", header, code, m_prog_size);

    return 0;
}
