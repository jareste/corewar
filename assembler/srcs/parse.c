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
#include <stdbool.h>
#include "log.h"
#include "encode.h"

static t_instr* m_instr = NULL;
static t_label* m_labels = NULL;
static int m_prog_size = 0;
static bool m_extend_enabled = false;

static void m_print_instrs()
{
    t_instr* inst;
    t_label* label;
    int i;

    log_msg(LOG_LEVEL_INFO,"Instructions:\n");
    inst = m_instr;
    while (inst)
    {
        if (!inst->op)
        {
            log_msg(LOG_LEVEL_INFO,"  Off[%d] Line %d: .code ", inst->offset, inst->line_no);
            for (i = 0; i < inst->raw_len; ++i)
            {
                log_msg(LOG_LEVEL_INFO,"%02X ", (uint8_t)inst->raw[i]);
            }
            log_msg(LOG_LEVEL_INFO,"\n");
            inst = FT_LIST_GET_NEXT(&m_instr, inst);
            continue;
        }

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

static inline int m_is_pc_relative_op(const t_op *op)
{
    // if (strcmp(op->name, "ld") == 0)
    // {
    //     return 1;
    // }
    return op->has_idx != 0;
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

static int m_parse_reg(const char *s, int *out)
{
    char *endptr;
    long val;

    errno = 0;
    val = strtol(s, &endptr, 10);
    log_msg(LOG_LEVEL_DEBUG, "Parsing int Errno=%d, endptr='%d' val=%d\n", errno, endptr==s, *endptr!='\0');
    if (errno != 0 || endptr == s || (*endptr != '\0' && !isspace((unsigned char)*endptr)))
        return -1;
    if (val < INT_MIN || val > INT_MAX)
        return -1;
    *out = (int)val;
    log_msg(LOG_LEVEL_DEBUG, "Parsed int: %d\n", *out);
    return 0;
}

static int m_parse_num32(const char *s, int32_t *out)
{
    char *endptr;
    long long val;

    if (strncmp(s, "0x", 2) == 0 || strncmp(s, "0X", 2) == 0)
    {
        /* hexadecimal */
        s += 2;
        errno = 0;
        val = strtoll(s, &endptr, 16);
    }
    else
    {
        errno = 0;
        val = strtoll(s, &endptr, 10);
    }

    log_msg(LOG_LEVEL_DEBUG, "Parsing int32 Errno=%d, endptr='%d' val=%lld\n", errno, endptr==s, val);
    if (errno != 0 || endptr == s || (*endptr != '\0' && !isspace((unsigned char)*endptr)))
        return -1;

    if (val < 0 && val < INT32_MIN)
        return -1;
    if (val > (long long)UINT32_MAX)
        return -1;

    *out = (int32_t)val;
    return endptr - s;
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
    int reg;
    int val;
    char* end;

    if (len == 0)
        return -1;

    if (s[0] == 'r') /* register */
    {
        log_msg(LOG_LEVEL_DEBUG, "Parsing register argument: '%s'\n", s);
        log_msg(LOG_LEVEL_DEBUG, "Length: %zu\n", len);
        if (len < 2)
            return -1;

        reg = 0;
        m_parse_reg(s + 1, &reg);
        log_msg(LOG_LEVEL_DEBUG, "Register number parsed: %d\n", reg);
        if (m_parse_reg(s + 1, &reg) != 0)
            return -1;
        if (reg < 0 || reg > REG_NUMBER)
            return -1;

        out->type = ARG_REG;
        out->u.value = reg;
        return 0;
    }
    else if (s[0] == DIRECT_CHAR) /* % */
    {
        log_msg(LOG_LEVEL_DEBUG, "Parsing direct argument: '%s'\n", s);
        if (len < 2)
            return -1;
        if (s[1] == LABEL_CHAR) /* '%:label' */
        {
            out->type = ARG_LABEL_DIR;
            end = s + 2 + strcspn(s + 2, " \t\n\r\f\v");
            out->u.label = strndup(s + 2, end - (s + 2));
            if (!out->u.label)
                return -1;
            log_msg(LOG_LEVEL_DEBUG, "Direct label parsed: '%s' %zu\n", out->u.label, strlen(out->u.label));
            return 0;
        }
        else /* '%42' */
        {
            if (m_parse_num32(s + 1, &val) == -1)
                return -1;
            out->type = ARG_DIR;
            out->u.value = val;
            return 0;
        }
    }
    else /* indirect */
    {
        log_msg(LOG_LEVEL_DEBUG, "Parsing indirect argument: '%s'\n", s);
        if (s[0] == LABEL_CHAR) /* ':label' */
        {
            out->type = ARG_LABEL_IND;

            end = s + 1 + strcspn(s + 1, " \t\n\r\f\v");
            out->u.label = strndup(s + 1, end - (s + 1));
            if (!out->u.label)
                return -1;
            log_msg(LOG_LEVEL_DEBUG, "Indirect label parsed: '%s' %zu\n", out->u.label, strlen(out->u.label));
            return 0;
        }
        else /* '42' */
        {
            if (m_parse_num32(s, &val) == -1)
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
    char *comment;
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
    comment = strchr(arg_str, COMMENT_CHAR);
    if (comment)
        *comment = '\0';
    token = strtok(arg_str, ",");
    log_msg(LOG_LEVEL_DEBUG, "  Parsing arguments: '%s'\n", arg_str);
    while (token && inst->arg_count < 3)
    {
        token = m_skip_spaces(token);

        if (strchr(token, '+') || strchr(token, '-'))
        {
            memset(&inst->args[inst->arg_count], 0, sizeof(t_arg));

            if (token[0] == DIRECT_CHAR)        // '%'
                inst->args[inst->arg_count].type = ARG_DIR;
            else
                inst->args[inst->arg_count].type = ARG_IND;

            inst->args[inst->arg_count].expr = strdup(token);
            if (!inst->args[inst->arg_count].expr)
                goto error;

            log_msg(LOG_LEVEL_DEBUG, "  Extended arg[%d]: '%s'\n",
                    inst->arg_count, token);

            inst->arg_count++;
            token = strtok(NULL, ",");
            continue;
        }


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

int m_find_nearest_instruction_offset(int line_no)
{
    t_instr* inst = m_instr;
    int nearest_offset = 0;
    int nearest_line = 0;

    log_msg(LOG_LEVEL_DEBUG, "Finding nearest instruction offset for line %d\n", line_no);
    while (inst)
    {
        log_msg(LOG_LEVEL_DEBUG, "  Checking instruction at line %d offset %d\n", inst->line_no, inst->offset);
        if (inst->line_no > line_no && (inst->line_no < nearest_line || nearest_line == 0))
        {
            nearest_line = inst->line_no;
            nearest_offset = inst->offset;
        }
        inst = FT_LIST_GET_NEXT(&m_instr, inst);
    }

    if (nearest_line == 0)
    {
        return m_prog_size;
    }

    return nearest_offset;
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

        if (!inst->op) /* .code */
        {
            offset += inst->raw_len;
            inst = FT_LIST_GET_NEXT(&m_instr, inst);
            continue;
        }


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
        if (label->offset == 0)
        {
            label->offset = m_find_nearest_instruction_offset(label->line_no);
            log_msg(LOG_LEVEL_DEBUG, "Label '%s' at line %d assigned offset %d\n",
                    label->name, label->line_no, label->offset);
        }

        label = FT_LIST_GET_NEXT(&m_labels, label);
    }

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

static t_label *find_label(const char *name)
{
    t_label *lab = m_labels;
    while (lab) {
        if (strcmp(lab->name, name) == 0)
            return lab;
        lab = FT_LIST_GET_NEXT(&m_labels, lab);
    }
    return NULL;
}

static int eval_expr(const char *expr, t_instr *inst, t_arg_type type, int32_t *out)
{
    const char *s = expr;
    int64_t acc = 0;
    int sign = +1;
    int has_label = 0;
    int len;
    int64_t term;
    char name[128];
    char *endptr;
    long long v;
    t_label *lab;

    (void)inst;

    log_msg(LOG_LEVEL_DEBUG, "Evaluating expr '%s'\n", expr);
    log_msg(LOG_LEVEL_DEBUG, "  Initial acc=%lld\n", acc);
    log_msg(LOG_LEVEL_DEBUG, " instruction %s[%d] at offset %d\n", inst->op->name, inst->line_no, inst->offset);

    if (*s == DIRECT_CHAR)
        s++;

    while (*s)
    {
        while (*s && isspace((unsigned char)*s))
            s++;

        if (*s == '+' || *s == '-')
        {
            sign = (*s == '+') ? +1 : -1;
            s++;
            continue;
        }

        if (*s == '\0')
            break;

        term = 0;

        if (*s == LABEL_CHAR)
        {
            log_msg(LOG_LEVEL_DEBUG, "  Parsing label term in expr\n");
            s++;
            len = 0;

            while (s[len] && strchr(LABEL_CHARS, s[len]) != NULL && len < (int)sizeof(name) - 1)
            {
                name[len] = s[len];
                len++;
            }
            name[len] = '\0';
            s += len;

            lab = find_label(name);
            if (!lab)
            {
                log_msg(LOG_LEVEL_ERROR, "Error: Undefined label '%s' in expr '%s'\n", name, expr);
                return -1;
            }
            term = lab->offset;
            has_label = 1;
        }
        else
        {
            // errno = 0;
            // v = strtoll(s, &endptr, 10);
            // if (errno != 0 || endptr == s)
            // {
            //     log_msg(LOG_LEVEL_ERROR, "Error: Bad number in expr '%s'\n", expr);
            //     return -1;
            // }
            endptr = (char*)s;
            endptr += m_parse_num32(s, (int32_t *)&v);
            if (endptr == s || endptr < s)
            {
                log_msg(LOG_LEVEL_ERROR, "Error: Bad number in expr '%s'\n", expr);
                return -1;
            }
            term = v;
            s = endptr;
        }

        acc += sign * term;
        sign = +1;
    }

    log_msg(LOG_LEVEL_DEBUG, "  Final acc=%lld\n", acc);
    if ((m_is_pc_relative_op(inst->op) || (type == ARG_IND)) && has_label)
        acc -= inst->offset;

    log_msg(LOG_LEVEL_DEBUG, "  Final acc=%lld\n", acc);
    *out = (int32_t)acc;
    return 0;
}

// static inline int arg_size(const t_instr *inst, int i)
// {
//     switch (inst->args[i].type) {
//         case ARG_REG: return 1;
//         case ARG_DIR:
//         case ARG_LABEL_DIR: return inst->op->has_idx ? 2 : 4;
//         case ARG_IND:
//         case ARG_LABEL_IND: return 2;
//     }
//     return 0;
// }

static void m_normalize_args()
{
    t_instr* inst;
    t_label* label;
    int i;
    bool found;
    int32_t val;

    inst = m_instr;
    while (inst)
    {
        for (i = 0; i < inst->arg_count; ++i)
        {

            if (inst->args[i].expr)
            {
                if (eval_expr(inst->args[i].expr, inst, inst->args[i].type, &val) != 0)
                {
                    ft_assert(false, "Error in extended expression");
                }
                inst->args[i].u.value = val;
                free(inst->args[i].expr);
                inst->args[i].expr = NULL;
                continue;
            }

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

                        if (m_is_pc_relative_op(inst->op) || (inst->args[i].type == ARG_IND) || (strcmp(inst->op->name, "ld") == 0))
                            inst->args[i].u.value = label->offset - inst->offset;
                        else
                        {
                            inst->args[i].u.value = label->offset;
                        }

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

        line = m_skip_spaces(buffer);
        if (*line == '\0' || *line == COMMENT_CHAR)
            continue;


        log_msg(LOG_LEVEL_DEBUG, "Read line[%u] '%s'\n", line_no, buffer);
        log_msg(LOG_LEVEL_DEBUG, "%d, %d\n", strncmp(buffer, ".code", 5), isspace((unsigned char)buffer[5]));

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
        else if (strstr(buffer, ".extend"))
        {
            m_extend_enabled = true;
            log_msg(LOG_LEVEL_INFO, "Extend directive found, extended features enabled.\n");
            continue;
        }
        else if (strncmp(line, ".code", 5) == 0 && 
            (line[5] == '\0' || isspace((unsigned char)line[5])))
        { /* TODO fixme */
            // if (!m_extend_enabled)
            // {
            //     log_msg(LOG_LEVEL_ERROR, ".code used without .extend at line %u\n", line_no);
            //     return ERROR;
            // }

            t_instr *inst = NEW(t_instr, 1);
            memset(inst, 0, sizeof(*inst));
            inst->line_no = line_no;
            inst->op = NULL;

            char *p = line + 5;
            p = m_skip_spaces(p);

            uint8_t tmp[256];
            int     n = 0;

            while (*p)
            {
                while (*p && isspace((unsigned char)*p))
                    p++;
                if (!*p) break;

                char byte_str[3] = {0};
                if (!isxdigit((unsigned char)p[0]) ||
                    !isxdigit((unsigned char)p[1]))
                {
                    log_msg(LOG_LEVEL_ERROR, "Bad .code byte at line %u: '%s'\n",
                            line_no, p);
                    free(inst);
                    return ERROR;
                }
                byte_str[0] = p[0];
                byte_str[1] = p[1];
                unsigned long v = strtoul(byte_str, NULL, 16);
                tmp[n++] = (uint8_t)v;
                p += 2;
            }

            inst->raw = malloc(n);
            memcpy(inst->raw, tmp, n);
            inst->raw_len = n;

            FT_LIST_ADD_LAST(&m_instr, inst);
            continue;
        }

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

            def_label->line_no = line_no;
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
        else if (def_label)
        {
            /* line with only label */
            FT_LIST_ADD_LAST(&m_labels, def_label);
        }

    }

    log_msg(LOG_LEVEL_INFO,"Labels:\n");
    log_msg(LOG_LEVEL_INFO, "*****************************************************************\n");
    t_label* l_label = m_labels;
    while (l_label)
    {
        log_msg(LOG_LEVEL_INFO,"  %s at offset %d\n", l_label->name, l_label->offset);
        l_label = FT_LIST_GET_NEXT(&m_labels, l_label);
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

    char outname[256];
    char* extension;
    extension = strrchr(filename, '.');
    if (extension)
        snprintf(outname, sizeof(outname), "%.*s.cor", (int)(extension - filename), filename);
    else
        snprintf(outname, sizeof(outname), "%s.cor", filename);

    write_cor_file(outname, header, code, m_prog_size);

    return 0;
}
