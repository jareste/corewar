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
#include "libft.h"
#include "gnl.h"
#include "ft_printf.h"

typedef struct args
{
	int32_t	argc;
	char**	argv;
} t_args;

#define HELP_CMD "Available commands:\n" \
				 "  dump             - Dump the memory state\n" \
				 "  procs            - Dump the processes state\n" \
				 "  run <cycles>     - Run the VM for a specified number of cycles\n" \
				 "  pkill <proc_id>  - Kill a process by its ID\n" \
				 "  write <addr> <value> - Write a value to a memory address\n" \
				 "  q                - Quit bonus mode\n" \
				 "  [h][?]           - Show this help message\n"

void m_dump_memory(t_vm *vm, t_args *args);
void m_dump_procs(t_vm *vm, t_args *args);
void m_run_cycles(t_vm *vm, t_args *args);
void m_quit(t_vm *vm, t_args *args);
void m_help(t_vm *vm, t_args *args);
void m_kill_process(t_vm *vm, t_args *args);
void m_write_mem(t_vm *vm, t_args *args);

typedef void	(*t_cmd_func)(t_vm *, t_args *args);

typedef struct s_cmd
{
	char		*name;
	t_cmd_func	func;
} t_cmd;

static const t_cmd	m_cmds[] = {
	{"dump", m_dump_memory},
	{"procs", m_dump_procs},
	{"run", m_run_cycles},
	{"pkill", m_kill_process},
	{"q", m_quit},
	{"quit", m_quit},
	{"h", m_help},
	{"write", m_write_mem},
	{"help", m_help},
	{"?", m_help},
	{NULL, NULL}
};

void m_release_args(t_args *args)
{
	int	i;

	if (!args->argv)
		return ;
	i = 0;
	while (i < args->argc)
	{
		free(args->argv[i]);
		i++;
	}
	free(args->argv);
	args->argv = NULL;
	args->argc = 0;
}

void	ft_realloc(void **ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;

	if (new_size == 0)
	{
		free(*ptr);
		*ptr = NULL;
		return ;
	}
	new_ptr = calloc(1, new_size);
	ft_assert(new_ptr != NULL, "realloc failed");
	if (*ptr)
	{
		if (old_size > new_size)
			old_size = new_size;
		memcpy(new_ptr, *ptr, old_size);
		free(*ptr);
	}
	*ptr = new_ptr;
}

int	m_build_args(char *line, t_args *args)
{
	char	*token;
	size_t	old_size;
	size_t	new_size;

	if (!line)
		return (-1);
	m_release_args(args);
	token = strtok(line, " \t\r\n");
	if (!token)
	{
		free(line);
		return (-1);
	}
	while (token)
	{
		old_size = sizeof(char *) * (args->argc + 1);
		new_size = sizeof(char *) * (args->argc + 2);
		ft_realloc((void **)&args->argv, old_size, new_size);
		args->argv[args->argc] = strdup(token);
		ft_assert(args->argv[args->argc] != NULL, "strdup failed");
		args->argc++;
		args->argv[args->argc] = NULL;
		token = strtok(NULL, " \t\r\n");
	}
	free(line);
	return (1);
}

void m_dump_procs(t_vm *vm, t_args *args)
{
	t_proc	*p;

	(void)args;
	log_msg(LOG_I, "Dumping processes at cycle %d:\n", vm->cycle);
	p = vm->procs;
	while (p)
	{
		log_msg(LOG_I, "  Process %d: pc=%d carry=%d wait=%d opcode=%d\n",
			p->id, p->pc, p->carry, p->op_wait, p->opcode);
		p = ft_list_get_next((void **)&vm->procs, (void *)p);
	}
}

void m_dump_memory(t_vm *vm, t_args *args)
{
	(void)args;
	log_msg(LOG_I, "Dumping memory at cycle %d:\n", vm->cycle);
	dump_memory(vm);
}

void m_run_cycles(t_vm *vm, t_args *args)
{
	int	cycles;

	if (args->argc < 2)
	{
		log_msg(LOG_W, "Usage: run <cycles>\n");
		return ;
	}
	cycles = ft_atoi(args->argv[1]);
	log_msg(LOG_I, "Running for %d cycles...\n", cycles);
	while (cycles)
	{
		vm->cycle++;
		t_proc	*proc = vm->procs;
		while (proc)
		{
			step_proc(vm, proc);
			proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
		}
		if (vm->cycle == vm->next_cycle_to_die)
		{
			log_msg(LOG_I, "Cycle %d: cycle to die(%d) check\n", vm->cycle, vm->cycle_to_die);
			proc_check_deads(vm);
		}
		cycles--;
	}
}

void m_quit(t_vm *vm, t_args *args)
{
	(void)vm;
	(void)args;
	log_msg(LOG_I, "Quitting bonus mode.\n");
	exit(0);
}

void m_help(t_vm *vm, t_args *args)
{
	(void)vm;
	(void)args;
	ft_dprintf(1, "%s", HELP_CMD);
}

void m_kill_process(t_vm *vm, t_args *args)
{
	int	proc_id;

	if (args->argc < 2)
	{
		log_msg(LOG_W, "Usage: pkill <process_id>\n");
		return ;
	}
	proc_id = ft_atoi(args->argv[1]);
	t_proc	*proc = vm->procs;
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

void m_write_mem(t_vm *vm, t_args *args)
{
	int	addr;
	int	value;
	char* endptr;

	if (args->argc < 3)
	{
		log_msg(LOG_W, "Usage: write <address> <value>\n");
		return ;
	}
	addr = strtol(args->argv[1], &endptr, 10);
	if (*endptr != '\0')
	{
		log_msg(LOG_W, "Invalid address: %s\n", args->argv[1]);
		return ;
	}
	value = strtol(args->argv[2], &endptr, 10);
	if (*endptr != '\0')
	{
		log_msg(LOG_W, "Invalid value: %s\n", args->argv[2]);
		return ;
	}
	if (addr < 0 || addr >= MEM_SIZE)
	{
		log_msg(LOG_W, "Address out of bounds: %d\n", addr);
		return ;
	}
	vm->memory[addr] = (uint8_t)(value & 0xFF);
	log_msg(LOG_I, "Wrote value %d to memory address %d\n", value, addr);
}

void	handle_bonus(t_vm *vm)
{
	t_args	args;
	int32_t	i;

	memset(&args, 0, sizeof(args));
	can_print_log(true);
	while (1)
	{
		ft_dprintf(1, "\nBonus mode - cycle %d ('?' for help)\n> ", vm->cycle);
		if (!m_build_args(get_next_line(0), &args))
			exit(1);
		if (args.argc == 0)
			continue ;
		i = 0;
		while (m_cmds[i].name)
		{
			if (strcmp(args.argv[0], m_cmds[i].name) == 0)
				m_cmds[i].func(vm, &args);
			i++;
		}
	}
	exit(0);
}


// void	m_run(t_vm *vm)
// {
// 	t_proc	*proc;

// 	while (1)
// 	{
// 		vm->cycle++;
// 		if (vm->dump_enabled && vm->cycle == vm->dump_cycle)
// 		{
// 			m_dump_memory(vm);
// 			exit(0);
// 		}
// 		proc = vm->procs;
// 		while (proc)
// 		{
// 			step_proc(vm, proc);
// 			proc = ft_list_get_next((void **)&vm->procs, (void *)proc);
// 		}
// 		if (vm->cycle % vm->cycle_to_die == 0)
// 		{
// 			log_msg(LOG_I, "Cycle %d: cycle to die check\n", vm->cycle);
// 			proc_check_deads(vm);
// 		}
// 	}
// }