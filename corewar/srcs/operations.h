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

#ifndef OPERATIONS_H
# define OPERATIONS_H

typedef int (*op_func_t)(t_vm*, t_proc*, t_arg*);

t_champ	*find_champ_by_id(t_vm *vm, int id);
int		op_execute(t_vm *vm, t_proc *proc, t_arg *args, uint8_t op_code);
int		mem_addr(int addr);

#endif /* OPERATIONS_H */
