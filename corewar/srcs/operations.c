#include "corewar.h"
#include "log.h"

typedef int (*op_func_t)(t_vm*, t_proc*, t_arg*);

static int m_op_and(t_vm *vm, t_proc *p, t_arg *args);
static int m_op_sti(t_vm *vm, t_proc *p, t_arg *args);
static int m_op_live(t_vm *vm, t_proc *proc, t_arg *args);
static int m_op_zjmp(t_vm *vm, t_proc *proc, t_arg *args);

const op_func_t m_op_funcs[OP_COUNT] = {
    NULL,               /* 0: no operation */
    m_op_live,          /* 1: live */
    NULL,               /* 2: ld */
    NULL,               /* 3: st */
    NULL,               /* 4: add */
    NULL,               /* 5: sub */
    m_op_and,           /* 6: and */
    NULL,               /* 7: or */
    NULL,               /* 8: xor */
    m_op_zjmp,          /* 9: zjmp */
    NULL,               /* 10: ldi */
    m_op_sti,           /* 11: sti */
    NULL,               /* 12: fork */
    NULL,               /* 13: lld */
    NULL,               /* 14: lldi */
    NULL,               /* 15: lfork */
    NULL                /* 16: aff */
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
        if (vm->champs[i].id == id)
            return &vm->champs[i];
    }
    return NULL;
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
        log_msg(LOG_LEVEL_WARN, "Invalid register %d in AND\n", dest);
        return 0;
    }

    result = arg1 & arg2;

    p->regs[dest - 1] = result;
    p->carry = (result == 0);

    log_msg(LOG_LEVEL_INFO,
        "Process %d: and %d & %d = %d → r%d\n",
        p->id, arg1, arg2, result, dest);

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
    int dest;
    int32_t arg1;
    int32_t arg2;
    int32_t arg3;
    int32_t offset;
    int reg = args[0].value;

    if (reg < 1 || reg > REG_NUMBER)
    {
        log_msg(LOG_LEVEL_WARN, "Invalid register %d in STI\n", reg);
        return 0;
    }

    arg1 = p->regs[reg - 1];

    arg2 = get_value(vm, p, &args[1]);
    arg3 = get_value(vm, p, &args[2]);

    offset = (arg2 + arg3) % IDX_MOD;

    dest = mem_addr(p->pc + offset);   // now p->pc == old_pc

    m_mem_write(vm, dest, arg1, 4);
    log_msg(LOG_LEVEL_INFO,
        "Process %d: sti r%d (%d) to %d (pc %d + (%d+%d) %% IDX_MOD)\n",
        p->id, reg, arg1, dest, p->pc, arg2, arg3);

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

    vm->last_alive_player = champ_id;

    log_msg(LOG_LEVEL_INFO,
            "Process %d: LIVE called for champ %d\n",
            proc->id, champ_id);

    if (champ)
    {
        log_msg(LOG_LEVEL_INFO,
                "Champ %d (%s) is reported alive!\n",
                champ->id, champ->name);
    }
    else
    {
        log_msg(LOG_LEVEL_WARN,
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
    log_msg(LOG_LEVEL_INFO,
            "Process %d: ZJMP called with offset %d (jump %d), carry=%d\n",
            proc->id, offset, jump, proc->carry);
    if (proc->carry)
    {
        proc->pc = (proc->pc + jump) % MEM_SIZE;
        if (proc->pc < 0) proc->pc += MEM_SIZE;

        log_msg(LOG_LEVEL_INFO,
                "Process %d: ZJMP succeeded, jumping by %d → new PC=%d\n",
                proc->id, jump, proc->pc);
    }
    else
    {
        log_msg(LOG_LEVEL_INFO,
                "Process %d: ZJMP failed (carry = 0)\n",
                proc->id);
        proc->pc = (proc->pc + 3) % MEM_SIZE; /* Move past the instruction */
    }

    return 0;
}


int op_execute(t_vm *vm, t_proc *proc, t_arg *args, uint8_t op_code)
{
    /* Placeholder for actual operation execution logic */
    (void)vm;
    (void)proc;
    (void)args;
    (void)op_code;
    
    if (op_code < 1 || op_code >= OP_COUNT)
    {
        log_msg(LOG_LEVEL_ERROR,
                "Process %d: Invalid opcode %d\n",
                proc->id, op_code);
        return -1; /* Error */
    }

    log_msg(LOG_LEVEL_DEBUG,
            "Process %d: Executing operation %d\n",
            proc->id, op_code);

    
    if (m_op_funcs[op_code])
    {
        return m_op_funcs[op_code](vm, proc, args);
    }
    else
    {
        log_msg(LOG_LEVEL_WARN,
                "Process %d: Operation %d not implemented yet\n",
                proc->id, op_code);
    }

    return 0; /* Success */
}