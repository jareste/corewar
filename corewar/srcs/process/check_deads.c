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
#include "log.h"
#include "ft_printf.h"
#include "../corewar.h"
#include "../process/process.h"
#include "../operations/operations.h"

// static void m_print_all_procs(t_vm *vm)
// {
// 	t_proc	*proc;

// 	proc = vm->procs;
// 	log_msg(LOG_D, "Current processes:\n");
// 	while (proc)
// 	{
// 		log_msg(LOG_D, "  Process[%d] %p: last_live_cycle=%d\n",
// 			proc->id, (void *)proc, proc->last_live_cycle);
// 		proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
// 	}
// }

static void	m_kill_deads(t_vm *vm)
{
	t_proc	*proc;
	t_proc	*to_delete;

	proc = vm->procs;
	while (proc)
	{
		if (vm->cycle - proc->last_live_cycle >= vm->cycle_to_die)
		{
			log_msg(LOG_I, "Process %d: has died (last live at cycle %d)\n",
				proc->id, proc->last_live_cycle);
			to_delete = proc;
			proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
			ft_list_pop((void **)&vm->procs, (void *)to_delete);
			log_msg(LOG_I, "Process[%d] %p has died\n",
				to_delete->id, (void *)to_delete);
			free(to_delete);
			continue ;
		}
		proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
	}
}

static int	m_count_alive_champs(t_vm *vm)
{
	t_proc	*proc;
	int		alive_champs[MAX_PLAYERS];
	int		i;
	int		count;

	memset(alive_champs, 0, sizeof(alive_champs));
	proc = vm->procs;
	while (proc)
	{
		i = proc->regs[0];
		if (i >= 0 && i < MAX_PLAYERS)
			alive_champs[i] = 1;
		proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
	}
	count = 0;
	i = 0;
	while (i < MAX_PLAYERS)
	{
		if (alive_champs[i])
			count++;
		i++;
	}
	log_msg(LOG_D, "Alive champions count: %d\n", count);
	return (count);
}

static void	m_check_vm_finished(t_vm *vm)
{
	if ((vm->procs == NULL) || (m_count_alive_champs(vm) == 1))
	{
		log_msg(LOG_I, "All processes have died\n");
		ft_dprintf(1, "The winner is Champion %d: '%s'\n",
			vm->last_alive_player + 1, vm->la_name);
		exit(0);
	}
}

static void	m_decrease_cycle_to_die(t_vm *vm)
{
	if (vm->lives_in_period >= NBR_LIVE)
	{
		vm->cycle_to_die -= CYCLE_DELTA;
		log_msg(LOG_I,
			"Decreasing cycle_to_die to %d\n", vm->cycle_to_die);
	}
	vm->lives_in_period = 0;
	if (((vm->cycle / vm->cycle_to_die) % vm->cycle_to_die) == 0)
	{
		vm->cycle_to_die -= CYCLE_DELTA;
		log_msg(LOG_I,
			"Decreasing cycle_to_die to %d\n", vm->cycle_to_die);
	}
}

void	proc_check_deads(t_vm *vm)
{
	m_kill_deads(vm);
	m_check_vm_finished(vm);
	m_decrease_cycle_to_die(vm);
}
