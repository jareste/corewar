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

static void	m_release_args(t_args *args)
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

static int	m_free_and_ret(void *mem, int rc)
{
	free(mem);
	return (rc);
}

int	m_build_args(char *line, t_args *args)
{
	char	*token;
	size_t	old_size;
	size_t	new_size;

	if (!line)
		return (m_free_and_ret(line, -1));
	m_release_args(args);
	token = strtok(line, " \t\r\n");
	if (!token)
		return (m_free_and_ret(line, 0));
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
	return (m_free_and_ret(line, 1));
}

void	handle_bonus(t_vm *vm)
{
	t_args		args;
	t_cmd_ctx	ctx;

	memset(&args, 0, sizeof(args));
	can_print_log(true);
	register_bonus_cmds();
	while (1)
	{
		ft_dprintf(1, "\nBonus mode - cycle %d ('?' for help)\n$ ", vm->cycle);
		if (!m_build_args(get_next_line(0), &args))
			exit(1);
		if (args.argc == 0)
			continue ;
		ctx.name = args.argv[0];
		ctx.func = NULL;
		ctx.vm = vm;
		ctx.args = &args;
		cmds(&ctx);
	}
	exit(0);
}
