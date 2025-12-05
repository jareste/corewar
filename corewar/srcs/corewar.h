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

#ifndef COREWAR_H
# define COREWAR_H
# include "../../op/op.h"
# include <ft_malloc.h>
# include <ft_list.h>
# include <error_codes.h>
# include <stdint.h>
// #include "process.h"

typedef struct s_champ
{
	int		id;
	char	name[PROG_NAME_LENGTH + 1];
	char	comment[COMMENT_LENGTH + 1];
	size_t	size;
	uint8_t	code[CHAMP_MAX_SIZE];
}	t_champ;

typedef struct s_proc
{
	list_item_t	l;
	int			id;
	int			regs[REG_NUMBER];
	int			pc;
	int			carry;
	int			last_live_cycle;
	int			op_wait; /* cycles to actually execute this opcode */
	int			opcode;
}	t_proc;

typedef struct s_vm
{
	uint8_t	memory[MEM_SIZE];
	t_proc	*procs;
	t_champ	champs[MAX_PLAYERS];
	int		cycle;
	int		cycle_to_die;
	int		last_check_cycle;
	int		lives_in_period;
	int		last_alive_player;
	int		aff_enabled;
}	t_vm;

typedef struct s_arg
{
	int	type;
	int	value;
}	t_arg;

#endif /* COREWAR_H */
