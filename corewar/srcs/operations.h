#ifndef OPERATIONS_H
# define OPERATIONS_H

t_champ* find_champ_by_id(t_vm *vm, int id);
int op_execute(t_vm *vm, t_proc *proc, t_arg *args, uint8_t op_code);
int mem_addr(int addr);

#endif /* OPERATIONS_H */
