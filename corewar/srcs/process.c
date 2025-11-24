#include <string.h>
#include "corewar.h"
#include "process.h"
#include "exec.h"

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
