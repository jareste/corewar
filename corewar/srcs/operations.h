#ifndef OPERATIONS_H
# define OPERATIONS_H

int op_execute(t_vm *vm, t_proc *proc, t_arg *args, uint8_t op_code);
int mem_addr(int addr);

#endif /* OPERATIONS_H */
