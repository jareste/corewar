#include <string.h>
#include "log.h"
#include "corewar.h"
#include "process.h"
#include "operations.h"

static int m_pid = 0;

int new_pid()
{
    return ++m_pid;
}

t_proc* create_process(int id, int pc, int owner_id)
{
    t_proc* proc;
    
    proc = (t_proc*)ft_malloc(sizeof(t_proc));

    proc->id = id;
    memset(proc->regs, 0, sizeof(proc->regs));
    proc->pc = pc % MEM_SIZE;
    proc->carry = 0;
    proc->last_live_cycle = 0;
    proc->op_wait = 0;
    proc->opcode = 0;

    proc->regs[0] = owner_id; /* convention: first register holds player ID */

    proc->l.next = NULL;
    proc->l.prev = NULL;

    return proc;
}

static int32_t read_signed(const uint8_t *p, int size)
{
    uint32_t v = 0;
    int i;
    int shift;

    for (i = 0; i < size; ++i)
        v = (v << 8) | p[i];

    /* Sign-extend from size bytes to 32 bits */
    if (size > 0 && size < 4)
    {
        shift = 32 - size * 8;
        /* cast to int32_t so the >> is arithmetic */
        return (int32_t)((int32_t)(v << shift) >> shift);
    }
    return (int32_t)v;
}

void decode_inst(const uint8_t *code, int code_len, int offset)
{
    int size;
    int pos = offset;
    int nargs;
    int i;
    int b;
    int atype;
    uint8_t opcode;
    uint8_t acb;
    uint8_t r;
    int32_t val;
    const t_op *op = NULL;

    if (pos >= code_len)
        return;

    opcode = code[pos++];

    if (opcode >= 1 && opcode <= 16)
        op = &op_tab[opcode];

    log_msg(LOG_LEVEL_ERROR, "opcode 0x%02X", opcode);
    if (op)
        log_msg(LOG_LEVEL_ERROR, " -> %s", op->name);
    log_msg(LOG_LEVEL_ERROR, " at %d\n", offset);

    acb = 0;
    if (op && op->has_pcode && pos < code_len)
    {
        acb = code[pos++];
        log_msg(LOG_LEVEL_ERROR, " ACB=0x%02X\n", acb);
    }

    nargs = op ? op->nb_params : 0;
    for (i = 0; i < nargs && pos < code_len; ++i)
    {
        atype = 0;

        if (op && op->has_pcode)
        {
            /* 1=REG, 2=DIR, 3=IND */
            atype = (acb >> (6 - 2 * i)) & 0x3;
        }
        else if (op)
        {
            /* fallback: infer from PARAM_* bitmask */
            if (op->param_types[i] & PARAM_REGISTER) atype = REG_CODE; /* 1 */
            else if (op->param_types[i] & PARAM_DIRECT)   atype = DIR_CODE; /* 2 */
            else if (op->param_types[i] & PARAM_INDIRECT) atype = IND_CODE; /* 3 */
            else                                          atype = 0;
        }

        if (atype == REG_CODE) /* register */
        {
            if (pos >= code_len) { log_msg(LOG_LEVEL_ERROR, "  truncated REG\n"); return; }
            r = code[pos++];
            log_msg(LOG_LEVEL_ERROR, "  arg%d: REG r%u\n", i + 1, r);
        }
        else if (atype == DIR_CODE || atype == IND_CODE)
        {
            if (atype == IND_CODE)
                size = IND_SIZE;
            else
                size = (op && op->has_idx) ? IND_SIZE : DIR_SIZE;

            if (pos + size > code_len)
            {
                log_msg(LOG_LEVEL_ERROR, "  truncated arg\n");
                return;
            }

            val = read_signed(&code[pos], size);
            log_msg(LOG_LEVEL_ERROR, "  arg%d: %s %d (0x",
                    i + 1, atype == DIR_CODE ? "DIR" : "IND", val);
            for (b = 0; b < size; ++b)
                log_msg(LOG_LEVEL_ERROR, "%02X", code[pos + b]);
            log_msg(LOG_LEVEL_ERROR, ")\n");
            pos += size;
        }
        else
        {
            log_msg(LOG_LEVEL_ERROR, "  arg%d: UNKNOWN type (acb mismatch?)\n", i + 1);
        }
    }
}

void execute_instruction(t_vm* vm, t_proc* proc)
{
    t_arg args[3];
    uint8_t opcode;
    int size;
    int i;
    int total_advance;
    int pc;
    uint8_t pcode;
    uint8_t acb;
    bool bad_reg;
    t_op* op;
    int prev_pc = proc->pc;

    opcode = vm->memory[prev_pc];
    if (opcode < 1 || opcode > 16)
        return;

    op = &op_tab[opcode];
    log_msg(LOG_LEVEL_INFO,
            "Process %d: Executing opcode %s at pc %d.\n",
            proc->id, op->name, prev_pc);

    memset(args, 0, sizeof(args));

    /* DEBUG. disass */
    decode_inst(vm->memory, MEM_SIZE, prev_pc);

    pc = prev_pc + 1; /* after opcode */
    total_advance = 1; /* start with opcode size */

    /* Decode argument types */
    if (op->has_pcode)
    {
        if (pc >= MEM_SIZE)
        {
            log_msg(LOG_LEVEL_WARN,
                    "Process %d: truncated pcode at pc %d\n", proc->id, pc);
            return;
        }

        pcode = vm->memory[pc];
        pc++;
        total_advance++;

        for (i = 0; i < op->nb_params; ++i)
        {
            acb = (pcode >> (6 - i * 2)) & 0x03;
            switch (acb)
            {
                case REG_CODE: args[i].type = PARAM_REGISTER; break;
                case DIR_CODE: args[i].type = PARAM_DIRECT;   break;
                case IND_CODE: args[i].type = PARAM_INDIRECT; break;
                default:       args[i].type = PARAM_UNKNOWN;  break;
            }
        }
    }
    else
    {
        /* No pcode -> infer from op->param_types (which uses PARAM_* bitmask) */
        for (i = 0; i < op->nb_params; ++i)
        {
            if (op->param_types[i] & PARAM_REGISTER)
                args[i].type = PARAM_REGISTER;
            else if (op->param_types[i] & PARAM_DIRECT)
                args[i].type = PARAM_DIRECT;
            else if (op->param_types[i] & PARAM_INDIRECT)
                args[i].type = PARAM_INDIRECT;
            else
                args[i].type = PARAM_UNKNOWN;

            log_msg(LOG_LEVEL_DEBUG,
                    "Arg %d type set to %d from op param_types\n",
                    i + 1, args[i].type);
        }
    }

    /* Read argument values */
    for (i = 0; i < op->nb_params; ++i)
    {
        if (args[i].type == PARAM_REGISTER)
        {
            if (pc >= MEM_SIZE)
            {
                log_msg(LOG_LEVEL_WARN,
                        "Process %d: truncated register at pc %d\n",
                        proc->id, pc);
                return;
            }
            args[i].value = vm->memory[pc];
            pc++;
            total_advance++;

            log_msg(LOG_LEVEL_DEBUG, "Register arg read r%d\n", args[i].value);
        }
        else if (args[i].type == PARAM_DIRECT)
        {
            size = op->has_idx ? IND_SIZE : DIR_SIZE; /* DIR: 2 or 4 */
            if (pc + size > MEM_SIZE)
            {
                log_msg(LOG_LEVEL_WARN,
                        "Process %d: truncated direct at pc %d\n",
                        proc->id, pc);
                return;
            }
            args[i].value = read_signed(&vm->memory[pc], size);
            pc += size;
            total_advance += size;

            log_msg(LOG_LEVEL_DEBUG,
                    "Direct arg read value %d (size=%d)\n",
                    args[i].value, size);
        }
        else if (args[i].type == PARAM_INDIRECT)
        {
            size = IND_SIZE;
            if (pc + size > MEM_SIZE)
            {
                log_msg(LOG_LEVEL_WARN,
                        "Process %d: truncated indirect at pc %d\n",
                        proc->id, pc);
                return;
            }
            args[i].value = read_signed(&vm->memory[pc], size);
            pc += size;
            total_advance += size;

            log_msg(LOG_LEVEL_DEBUG,
                    "Indirect arg read value %d\n", args[i].value);
        }
        else
        {
            log_msg(LOG_LEVEL_WARN,
                    "Process %d: unknown arg type for arg %d, treating as invalid\n",
                    proc->id, i + 1);
            /* Assert or simply mark it as invalid and advance? */
            ft_assert(false, "Unknown argument type");
        }
    }

    /* Validate we readed proper registers */
    bad_reg = false;
    for (i = 0; i < op->nb_params; ++i)
    {
        if (args[i].type == PARAM_REGISTER &&
            (args[i].value < 1 || args[i].value > REG_NUMBER))
        {
            bad_reg = true;
            log_msg(LOG_LEVEL_WARN,
                    "Process %d: invalid register r%d in arg %d\n",
                    proc->id, args[i].value, i + 1);
        }
    }

    ft_assert(!bad_reg, "Invalid register detected");

    /* TODO: actually execute semantics using 'op' & 'args' */
    if (op_execute(vm, proc, args, opcode) != 0)
    {
        log_msg(LOG_LEVEL_ERROR,
                "Process %d: Error executing opcode %s\n",
                proc->id, op->name);
    }

    /* Advance PC after instruction */
    if (opcode != 9 /* ZJMP */) /* zjmp already performs a jump lol */
    {
        proc->pc = (prev_pc + total_advance) % MEM_SIZE;
        log_msg(LOG_LEVEL_DEBUG,
            "Process %d: Advanced pc by %d to %d.\n",
            proc->id, total_advance, proc->pc);
    }

}

void proc_check_deads(t_vm *vm)
{
    t_proc* proc;

    proc = vm->procs;
    while (proc)
    {
        if (vm->cycle - proc->last_live_cycle >= CYCLE_TO_DIE)
        {
            log_msg(LOG_LEVEL_INFO,
                    "Process %d: has died (last live at cycle %d)\n",
                    proc->id, proc->last_live_cycle);

            t_proc* to_delete = proc;
            proc = FT_LIST_GET_NEXT(&vm->procs, proc);

            FT_LIST_POP(&vm->procs, to_delete);
            free(to_delete);
            continue;
        }

        proc = FT_LIST_GET_NEXT(&vm->procs, proc);
    }

}

void step_proc(t_vm *vm, t_proc *p)
{
    uint8_t opcode;
    t_op* op;

    if (p->op_wait > 0)
    {
        p->op_wait--;
        return;
    }

    execute_instruction(vm, p);
    opcode = vm->memory[p->pc];
    if (opcode < 1 || opcode > 16)
    {
        log_msg(LOG_LEVEL_WARN, "Process %d: Invalid opcode %02X at pc %d. Advancing 1 byte.\n",
                p->id, opcode, p->pc);
        p->pc = (p->pc + 1) % MEM_SIZE;
        return;
    }

    op = &op_tab[opcode];
    p->opcode = opcode;
    p->op_wait = op->nb_cycles - 1; /* minus 1 because we execute one cycle now */
    log_msg(LOG_LEVEL_INFO, "Process %d: Fetched opcode %s at pc %d. Will execute in %d cycles.\n",
            p->id, op->name, p->pc, p->op_wait + 1);
}

