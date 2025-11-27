#include "corewar.h"
#include "log.h"

typedef int (*op_func_t)(t_vm*, t_proc*, t_arg*);

static int m_op_live(t_vm *vm, t_proc *proc, t_arg *args);
static int m_op_zjmp(t_vm *vm, t_proc *proc, t_arg *args);

const op_func_t m_op_funcs[OP_COUNT] = {
    NULL,               /* 0: no operation */
    m_op_live,          /* 1: live */
    NULL,               /* 2: ld */
    NULL,               /* 3: st */
    NULL,               /* 4: add */
    NULL,               /* 5: sub */
    NULL,               /* 6: and */
    NULL,               /* 7: or */
    NULL,               /* 8: xor */
    m_op_zjmp,          /* 9: zjmp */
    NULL,               /* 10: ldi */
    NULL,               /* 11: sti */
    NULL,               /* 12: fork */
    NULL,               /* 13: lld */
    NULL,               /* 14: lldi */
    NULL,               /* 15: lfork */
    NULL                /* 16: aff */
};

t_champ* find_champ_by_id(t_vm *vm, int id)
{
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (vm->champs[i].id == id)
            return &vm->champs[i];
    }
    return NULL;
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
    proc->carry = 1; /* force it. REMOVE IT LATER! */
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