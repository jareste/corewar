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

void	quit(t_vm *vm, t_args *args)
{
	(void)vm;
	(void)args;
	log_msg(LOG_I, "Quitting bonus mode.\n");
	exit(0);
}

void	help(t_vm *vm, t_args *args)
{
	(void)vm;
	(void)args;
	ft_dprintf(1, "Available commands:\n");
	ft_dprintf(1, "  dump             - Dump the memory state\n");
	ft_dprintf(1, "  procs            - Dump the processes state\n");
	ft_dprintf(1, "  run <cycles>     - Run the VM for a specified number");
	ft_dprintf(1, " of cycles\n");
	ft_dprintf(1, "  pkill <proc_id>  - Kill a process by its ID\n");
	ft_dprintf(1, "  write <addr> <value> - Write a value to a ");
	ft_dprintf(1, "memory address\n");
	ft_dprintf(1, "  q                - Quit bonus mode\n");
	ft_dprintf(1, "  [h][?]           - Show this help message\n");
}
