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

#ifndef PROCESS_INTERNAL_H
# define PROCESS_INTERNAL_H

typedef struct s_exec_ctx
{
	t_vm	*vm;
	t_proc	*proc;
	t_arg	args[3];
	t_op	*op;
	uint8_t	opcode;
	int		prev_pc;
	int		pc;
	int		adv;
}	t_exec_ctx;

/* exec_read_arg.c */
int		exec_read_args(t_exec_ctx *c);

/* exec_dec_arg_types.c */
int		exec_decode_arg_types(t_exec_ctx *c);

/* exec_utils.c */
void	exec_advance_pc(t_exec_ctx *c);
int		exec_validate_regs(t_exec_ctx *c);
void	exec_init(t_exec_ctx *c, t_vm *vm, t_proc *proc);
int		exec_fetch_opcode(t_exec_ctx *c);

#endif /* PROCESS_INTERNAL_H */
