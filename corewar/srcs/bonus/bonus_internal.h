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

#ifndef BONUS_INTERNAL_H
# define BONUS_INTERNAL_H

# include "libft.h"

typedef struct args
{
	int32_t	argc;
	char	**argv;
}	t_args;

typedef void	(*t_cmd_func)(t_vm *, t_args *args);

typedef struct s_cmd
{
	char		*name;
	t_cmd_func	func;
}	t_cmd;

typedef struct s_cmd_ctx
{
	char		*name;
	t_cmd_func	func;
	t_vm		*vm;
	t_args		*args;
}	t_cmd_ctx;

void	dump_memory_b(t_vm *vm, t_args *args);
void	dump_procs(t_vm *vm, t_args *args);
void	run_cycles(t_vm *vm, t_args *args);
void	quit(t_vm *vm, t_args *args);
void	help(t_vm *vm, t_args *args);
void	kill_process(t_vm *vm, t_args *args);
void	write_mem(t_vm *vm, t_args *args);

void	ft_realloc(void **ptr, size_t old_size, size_t new_size);

void	register_bonus_cmds(void);
int		cmds(t_cmd_ctx *ctx);

#endif /* BONUS_INTERNAL_H */
