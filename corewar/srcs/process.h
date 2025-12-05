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

#ifndef PROCESS_H
# define PROCESS_H

# include <ft_list.h>
# include "corewar.h"
# include "decode.h"

t_proc	*create_process(int id, int pc, int owner_id);
int		new_pid(void);
void	step_proc(t_vm *vm, t_proc *p);
void	proc_check_deads(t_vm *vm);

#endif /* PROCESS_H */
