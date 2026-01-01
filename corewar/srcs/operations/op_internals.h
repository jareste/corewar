/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:24:26 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/08 23:47:53 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OP_INTERNALS_H
# define OP_INTERNALS_H

int		m_op_live(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_ld(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_st(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_add(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_sub(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_and(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_or(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_xor(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_zjmp(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_ldi(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_sti(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_fork(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_lld(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_lldi(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_lfork(t_vm *vm, t_proc *p, t_arg *args);
int		m_op_aff(t_vm *vm, t_proc *p, t_arg *args);

/* op_utils.c */
int		mem_addr(int addr);
int32_t	m_mem_read(t_vm *vm, int addr, int size);
void	m_mem_write(t_vm *vm, int addr, int32_t value, int size);
int32_t	get_value(t_vm *vm, t_proc *p, t_arg *a);
t_champ	*find_champ_by_id(t_vm *vm, int id);

#endif /* OP_INTERNALS_H */
