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

#include <string.h>
#include "ft_malloc.h"
#include "log.h"
#include "../corewar.h"
#include "../process/process.h"
#include "../operations/operations.h"
#include "gnl.h"
#include "ft_printf.h"
#include "bonus_internal.h"

void	kill_process(t_vm *vm, t_args *args)
{
	t_proc	*proc;
	int		proc_id;

	if (args->argc < 2)
	{
		log_msg(LOG_W, "Usage: pkill <process_id>\n");
		return ;
	}
	proc_id = ft_atoi(args->argv[1]);
	proc = vm->procs;
	while (proc)
	{
		if (proc->id == proc_id)
		{
			ft_list_pop((void **)&vm->procs, (void *)proc);
			log_msg(LOG_I, "Process[%d] %p has been killed\n",
				proc->id, (void *)proc);
			free(proc);
			return ;
		}
		proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
	}
	log_msg(LOG_W, "Process %d not found\n", proc_id);
}

void	write_mem(t_vm *vm, t_args *args)
{
	int		addr;
	int		value;
	char	*endptr;

	if (args->argc < 3)
	{
		log_msg(LOG_W, "Usage: write <address> <value>\n");
		return ;
	}
	addr = ft_strtol(args->argv[1], &endptr, 10);
	if (*endptr != '\0' || addr < 0 || addr >= MEM_SIZE)
	{
		log_msg(LOG_W, "Invalid address or out of bound: %s\n", args->argv[1]);
		return ;
	}
	value = ft_strtol(args->argv[2], &endptr, 10);
	if (*endptr != '\0')
	{
		log_msg(LOG_W, "Invalid value: %s\n", args->argv[2]);
		return ;
	}
	vm->memory[addr] = (uint8_t)(value & 0xFF);
	log_msg(LOG_I, "Wrote value %d to memory address %d\n", value, addr);
}
