#ifndef PROCESS_H
# define PROCESS_H

#include <ft_list.h>
#include "corewar.h"
#include "decode.h"

t_proc* create_process(int id, int pc, int owner_id);
int new_pid();
void step_proc(t_vm *vm, t_proc *p);
void proc_check_deads(t_vm *vm);

#endif /* PROCESS_H */
