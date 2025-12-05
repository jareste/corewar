#include <string.h>
#include "corewar.h"
#include "log.h"
#include "operations.h"
#include "process.h"


static int  m_op_and(t_vm* vm, t_proc* p, t_arg* args);
static int  m_op_sti(t_vm* vm, t_proc* p, t_arg* args);
static int  m_op_st(t_vm* vm, t_proc* p, t_arg* args);
static int  m_op_live(t_vm* vm, t_proc* proc, t_arg* args);
static int  m_op_zjmp(t_vm* vm, t_proc* proc, t_arg* args);
static int  m_op_ld(t_vm* vm, t_proc* p, t_arg* args);
static int  m_op_lld(t_vm* vm, t_proc* p, t_arg* args);
static int  m_op_lfork(t_vm* vm, t_proc* p, t_arg* args);
static int  m_op_fork(t_vm* vm, t_proc* p, t_arg* args);
static int  m_op_add(t_vm *vm, t_proc *p, t_arg *args);
static int  m_op_sub(t_vm *vm, t_proc *p, t_arg *args);
static int  m_op_xor(t_vm *vm, t_proc *p, t_arg *args);
static int  m_op_or(t_vm *vm, t_proc *p, t_arg *args);
static int  m_op_aff(t_vm *vm, t_proc *p, t_arg *args);

static const op_func_t m_op_funcs[OP_COUNT] =
{
    NULL,               /* 0: no operation */
    m_op_live,          /* 1: live */
    m_op_ld,            /* 2: ld */
    m_op_st,            /* 3: st */
    m_op_add,           /* 4: add */
    m_op_sub,           /* 5: sub */
    m_op_and,           /* 6: and */
    m_op_or,            /* 7: or */
    m_op_xor,           /* 8: xor */
    m_op_zjmp,          /* 9: zjmp */
    NULL,               /* 10: ldi */
    m_op_sti,           /* 11: sti */
    m_op_fork,          /* 12: fork */
    m_op_lld,           /* 13: lld */
    NULL,               /* 14: lldi */
    m_op_lfork,         /* 15: lfork */
    m_op_aff            /* 16: aff */
};

int mem_addr(int addr)
{
    addr %= MEM_SIZE;
    if (addr < 0) addr += MEM_SIZE;
    return addr;
}

static int32_t m_mem_read(t_vm *vm, int addr, int size)
{
    uint8_t buf[4] = {0}; /* max size */
    int32_t v = 0;
    int i;

    addr = mem_addr(addr);
    for (i = 0; i < size; ++i)
        buf[i] = vm->memory[mem_addr(addr + i)];

    /* convert big-endian signed */
    for (i = 0; i < size; ++i)
        v = (v << 8) | buf[i];

    /* sign extend */
    if (size < 4 && (buf[0] & 0x80))
        v -= 1 << (size * 8);

    return v;
}


static void m_mem_write(t_vm *vm, int addr, int32_t value, int size)
{
    int i;

    addr = mem_addr(addr);
    for (i = size - 1; i >= 0; --i)
    {
        vm->memory[mem_addr(addr + i)] = (value & 0xFF);
        value >>= 8;
    }
}

static int32_t get_value(t_vm *vm, t_proc *p, t_arg *a)
{
    int addr;

    if (a->type == PARAM_REGISTER)
        return p->regs[a->value - 1];

    if (a->type == PARAM_INDIRECT)
    {
        addr = p->pc + (a->value % IDX_MOD);
        return m_mem_read(vm, addr, 4); /* indirect load of 4 bytes. */
    }

    return a->value; /* direct */
}


t_champ* find_champ_by_id(t_vm *vm, int id)
{
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        log_msg(LOG_D, "Checking champ id %d against %d\n", vm->champs[i].id, id);
        if (vm->champs[i].id == id)
            return &vm->champs[i];
    }
    return NULL;
}

static int m_op_add(t_vm *vm, t_proc *p, t_arg *args)
{
    int r1 = get_value(vm, p, &args[0]);
    int r2 = get_value(vm, p, &args[1]);
    int r3 = get_value(vm, p, &args[2]);

 
    (void)vm;
    if (r1 < 1 || r1 > REG_NUMBER ||
        r2 < 1 || r2 > REG_NUMBER ||
        r3 < 1 || r3 > REG_NUMBER)
    {
        log_msg(LOG_W,
                "Process %d: ADD with invalid register(s): r%d, r%d, r%d\n",
                p->id, r1, r2, r3);
        return 0;
    }

    p->regs[r3 - 1] = p->regs[r1 - 1] + p->regs[r2 - 1];;
    p->carry = (p->regs[r3 - 1] == 0);

    log_msg(LOG_I,
            "Process %d: add r%d(%d) + r%d(%d) = %d → r%d, carry=%d\n",
            p->id, r1, p->regs[r1 - 1], r2, p->regs[r2 - 1], p->regs[r3 - 1], r3, p->carry);

    return 0;
}

static int m_op_sub(t_vm *vm, t_proc *p, t_arg *args)
{
    int r1 = get_value(vm, p, &args[0]);
    int r2 = get_value(vm, p, &args[1]);
    int r3 = get_value(vm, p, &args[2]);

 
    (void)vm;
    if (r1 < 1 || r1 > REG_NUMBER ||
        r2 < 1 || r2 > REG_NUMBER ||
        r3 < 1 || r3 > REG_NUMBER)
    {
        log_msg(LOG_W,
                "Process %d: SUB with invalid register(s): r%d, r%d, r%d\n",
                p->id, r1, r2, r3);
        return 0;
    }

    p->regs[r3 - 1] = p->regs[r1 - 1] - p->regs[r2 - 1];;
    p->carry = (p->regs[r3 - 1] == 0);

    log_msg(LOG_I,
            "Process %d: sub r%d(%d) - r%d(%d) = %d → r%d, carry=%d\n",
            p->id, r1, p->regs[r1 - 1], r2, p->regs[r2 - 1], p->regs[r3 - 1], r3, p->carry);

    return 0;
}

static int m_op_or(t_vm *vm, t_proc *p, t_arg *args)
{
    int32_t arg1;
    int32_t arg2;
    int32_t result;
    int dest;

    arg1 = get_value(vm, p, &args[0]);
    arg2 = get_value(vm, p, &args[1]);
    dest = args[2].value;

    if (dest < 1 || dest > REG_NUMBER)
    {
        log_msg(LOG_W, "Invalid register %d in OR\n", dest);
        return 0;
    }

    result = arg1 | arg2;

    p->regs[dest - 1] = result;
    p->carry = (result == 0);

    log_msg(LOG_I,
        "Process %d: or %d | %d = %d → r%d\n",
        p->id, arg1, arg2, result, dest);

    return 0;
}

static int m_op_xor(t_vm *vm, t_proc *p, t_arg *args)
{
    int32_t arg1;
    int32_t arg2;
    int32_t result;
    int dest;

    arg1 = get_value(vm, p, &args[0]);
    arg2 = get_value(vm, p, &args[1]);
    dest = args[2].value;

    if (dest < 1 || dest > REG_NUMBER)
    {
        log_msg(LOG_W, "Invalid register %d in XOR\n", dest);
        return 0;
    }

    result = arg1 ^ arg2;

    p->regs[dest - 1] = result;
    p->carry = (result == 0);

    log_msg(LOG_I,
        "Process %d: xor %d ^ %d = %d → r%d\n",
        p->id, arg1, arg2, result, dest);

    return 0;
}

static int m_op_aff(t_vm *vm, t_proc *p, t_arg *args)
{
    int reg_num;
    char c;

    if (!vm->aff_enabled)
        return 0; /* AFF disabled */

    reg_num = args[0].value;
    if (reg_num < 1 || reg_num > REG_NUMBER)
    {
        log_msg(LOG_W,
                "Process %d: AFF with invalid register r%d\n",
                p->id, reg_num);
        return 0;
    }

    c = (char)(p->regs[reg_num - 1] % 256);

    log_msg(LOG_I,
            "Process %d: AFF r%d -> '%c'\n",
            p->id, reg_num, c);
    return 0;
}


/* op AND
 * Param1: value
 * Param2: value
 * Param3: register
 * Result: param1 & param2 stored in register
 * Carry: Yes -> if result == 0
 */
static int m_op_and(t_vm *vm, t_proc *p, t_arg *args)
{
    int32_t arg1;
    int32_t arg2;
    int32_t result;
    int dest;

    arg1 = get_value(vm, p, &args[0]);
    arg2 = get_value(vm, p, &args[1]);
    dest = args[2].value;

    if (dest < 1 || dest > REG_NUMBER)
    {
        log_msg(LOG_W, "Invalid register %d in AND\n", dest);
        return 0;
    }

    result = arg1 & arg2;

    p->regs[dest - 1] = result;
    p->carry = (result == 0);

    log_msg(LOG_I,
        "Process %d: and %d & %d = %d → r%d\n",
        p->id, arg1, arg2, result, dest);

    return 0;
}

static int m_op_st(t_vm *vm, t_proc *p, t_arg *args)
{
    int32_t src;
    int addr;

    if (args[0].value < 1 || args[0].value > REG_NUMBER)
    {
        log_msg(LOG_W, "Process %d: ST with invalid src r%d\n",
                p->id, args[0].value);
        return 0;
    }

    src = p->regs[args[0].value - 1];

    if (args[1].type == PARAM_REGISTER)
    {
        if (args[1].value < 1 || args[1].value > REG_NUMBER)
        {
            log_msg(LOG_W, "Process %d: ST with invalid dst r%d\n",
                    p->id, args[1].value);
            return 0;
        }

        p->regs[args[1].value - 1] = src;
        log_msg(LOG_I,
                "Process %d: st r%d (%d) → r%d\n",
                p->id, args[0].value, src, args[1].value);
    }
    else if (args[1].type == PARAM_INDIRECT)
    {
        addr = mem_addr(p->pc + (args[1].value % IDX_MOD));

        m_mem_write(vm, addr, src, 4);

        log_msg(LOG_I,
                "Process %d: st r%d (%d) → mem[%d] (pc %d + %d %% IDX_MOD)\n",
                p->id, args[0].value, src, addr, p->pc, args[1].value);
    }
    else
    {
        log_msg(LOG_W,
                "Process %d: ST with invalid second arg type %d\n",
                p->id, args[1].type);
    }

    return 0;
}

/* op STI
 * Param1: register (value to store)
 * Param2: value
 * Param3: value
 * Result: store value of reg at address (pc + (param2 + param3) % IDX_MOD)
 */
static int m_op_sti(t_vm *vm, t_proc *p, t_arg *args)
{
    int32_t reg_val;
    int32_t arg2;
    int32_t arg3;
    int32_t addr;
    int16_t offset;

    if ((args[0].value < 1) || (args[0].value > REG_NUMBER))
    {
        log_msg(LOG_W, "Process %d: STI with invalid src r%d\n",
                p->id, args[0].value);
        return 0;
    }

    reg_val = p->regs[args[0].value - 1];
    arg2 = get_value(vm, p, &args[1]);
    arg3 = get_value(vm, p, &args[2]);
    offset = (int16_t)((arg2 + arg3) % IDX_MOD);
    addr = mem_addr(p->pc + offset);

    m_mem_write(vm, addr, reg_val, 4);
    log_msg(LOG_I, "Process %d: sti r%d (%d) → mem[%d] (pc %d + (%d+%d) %% IDX_MOD)\n",
            p->id, args[0].value, reg_val, addr, p->pc, arg2, arg3);

    return 0;
}

static int m_op_live(t_vm *vm, t_proc *proc, t_arg *args)
{
    int32_t champ_id;
    t_champ* champ;
    
    champ_id = args[0].value;
    champ = find_champ_by_id(vm, champ_id);

    proc->last_live_cycle = vm->cycle;
    vm->lives_in_period++;

    log_msg(LOG_I,
            "Process %d: LIVE called for champ %d\n",
            proc->id, champ_id);

    if (champ) /* it exists */
    {
        vm->last_alive_player = champ_id;
        log_msg(LOG_I,
                "Champ %d (%s) is reported alive!\n",
                champ->id, champ->name);
    }
    else
    {
        log_msg(LOG_W,
                "LIVE called with invalid champ id %d\n",
                champ_id);
    }

    return 0; /* Success */
}

static int m_op_zjmp(t_vm *vm, t_proc *proc, t_arg *args)
{
    int16_t offset = args[0].value;
    int jump = offset % IDX_MOD;

    (void)vm;
    // proc->carry = 1; /* force it. REMOVE IT LATER! */
    log_msg(LOG_I,
            "Process %d: ZJMP called with offset %d (jump %d), carry=%d\n",
            proc->id, offset, jump, proc->carry);
    if (proc->carry)
    {
        proc->pc = (proc->pc + jump) % MEM_SIZE;
        if (proc->pc < 0) proc->pc += MEM_SIZE;

        log_msg(LOG_I,
                "Process %d: ZJMP succeeded, jumping by %d → new PC=%d\n",
                proc->id, jump, proc->pc);
    }
    else
    {
        log_msg(LOG_I,
                "Process %d: ZJMP failed (carry = 0)\n",
                proc->id);
        proc->pc = (proc->pc + 3) % MEM_SIZE; /* Move past the instruction */
    }

    return 0;
}

static int m_op_lld(t_vm *vm, t_proc *p, t_arg *args)
{
    int32_t arg1;
    int32_t arg2;
    int32_t addr;

    (void)vm;
    if (args[0].type == PARAM_INDIRECT)
    {
        /* long load does not use IDX_MOD */
        addr = p->pc + args[0].value;
        arg1 = m_mem_read(vm, addr, 4);
    }
    else
    {
        arg1 = get_value(vm, p, &args[0]);
    }

    arg2 = get_value(vm, p, &args[1]);

    p->regs[arg2 - 1] = arg1;
    p->carry = (arg1 == 0);

    log_msg(LOG_I,
        "Process %d: lld %d → r%d\n",
        p->id, arg1, arg2);

    return 0;
}

static int m_op_ld(t_vm *vm, t_proc *p, t_arg *args)
{
    int32_t arg1;
    int32_t arg2;
    t_arg lld_args[2];

    (void)vm;
    arg1 = get_value(vm, p, &args[0]);
    arg2 = get_value(vm, p, &args[1]);

    lld_args[0].type = PARAM_DIRECT;
    lld_args[0].value = arg1;
    lld_args[1].type = PARAM_REGISTER;
    lld_args[1].value = arg2;

    return m_op_lld(vm, p, lld_args);
}


static int m_do_fork(t_vm* vm, t_proc* p, int new_pc)
{
    int owner_id;
    t_proc *child;

    owner_id = p->regs[0];
    child = create_process(new_pid(), new_pc, owner_id);

    memcpy(child->regs, p->regs, sizeof(p->regs));
    child->carry           = p->carry;
    child->last_live_cycle = p->last_live_cycle;
    child->op_wait         = 0;
    child->opcode          = 0;

    FT_LIST_ADD_FIRST(&vm->procs, child);

    log_msg(LOG_I,
        "Process %d: fork → child %d at pc %d\n",
        p->id, child->id, child->pc);

    return 0;
}

static int m_op_fork(t_vm *vm, t_proc *p, t_arg *args)
{
    int16_t offset;
    int rel;
    int new_pc;

    offset = (int16_t)args[0].value;
    rel = offset % IDX_MOD;
    new_pc = mem_addr(p->pc + rel);

    return m_do_fork(vm, p, new_pc);
}

static int m_op_lfork(t_vm *vm, t_proc *p, t_arg *args)
{
    int16_t offset;
    int new_pc;

    offset = (int16_t)args[0].value;
    new_pc = mem_addr(p->pc + offset);

    return m_do_fork(vm, p, new_pc);
}

int op_execute(t_vm *vm, t_proc *proc, t_arg *args, uint8_t op_code)
{
    if (op_code < 1 || op_code >= OP_COUNT)
    {
        log_msg(LOG_E,
                "Process %d: Invalid opcode %d\n",
                proc->id, op_code);
        return -1; /* Error */
    }

    log_msg(LOG_D,
            "Process %d: Executing operation %d\n",
            proc->id, op_code);

    
    if (m_op_funcs[op_code])
    {
        return m_op_funcs[op_code](vm, proc, args);
    }
    else
    {
        log_msg(LOG_W,
                "Process %d: Operation %d not implemented yet\n",
                proc->id, op_code);
    }

    return 0; /* Success */
}