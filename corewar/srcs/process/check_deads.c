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
#include "../corewar.h"
#include "../process/process.h"
#include "../operations/operations.h"

t_champ	*get_winner(t_vm *vm)
{
	t_champ	*champ;

	ft_assert(vm->last_alive_player > -1, "Invalid last_alive_player");
	champ = find_champ_by_id(vm, vm->last_alive_player);
	return (champ);
}

void	m_kill_deads(t_vm *vm)
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
			free(to_delete);
			continue ;
		}
		proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
	}
}

void	m_check_vm_finished(t_vm *vm)
{
	t_champ	*winner;

	if (vm->procs == NULL)
	{
		log_msg(LOG_I, "All processes have died\n");
		winner = get_winner(vm);
		if (winner)
		{
			log_msg(LOG_I, "The winner is Champion %d: %s\n",
				winner->id, winner->name);
		}
		else
		{
			log_msg(LOG_I, "No winner could be determined.\n");
		}
		exit(0);
	}
}

void	m_decrease_cycle_to_die(t_vm *vm)
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
