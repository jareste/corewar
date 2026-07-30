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

static void	m_register_bonus_cmds2(t_cmd_ctx *ctx, char *name, t_cmd_func func)
{
	ctx->name = name;
	ctx->func = func;
	cmds(ctx);
}

void	register_bonus_cmds(void)
{
	t_cmd_ctx	ctx;

	memset(&ctx, 0, sizeof(ctx));
	m_register_bonus_cmds2(&ctx, "dump", dump_memory_b);
	m_register_bonus_cmds2(&ctx, "procs", dump_procs);
	m_register_bonus_cmds2(&ctx, "run", run_cycles);
	m_register_bonus_cmds2(&ctx, "pkill", kill_process);
	m_register_bonus_cmds2(&ctx, "write", write_mem);
	m_register_bonus_cmds2(&ctx, "q", quit);
	m_register_bonus_cmds2(&ctx, "quit", quit);
	m_register_bonus_cmds2(&ctx, "h", help);
	m_register_bonus_cmds2(&ctx, "help", help);
	m_register_bonus_cmds2(&ctx, "?", help);
}

static int	m_exec_cmd(t_cmd *cmds, t_cmd_ctx *ctx)
{
	int	i;

	if (!cmds || !ctx || !ctx->name)
		return (-1);
	i = 0;
	while (cmds[i].name)
	{
		if (strcmp(cmds[i].name, ctx->name) == 0)
		{
			cmds[i].func(ctx->vm, ctx->args);
			return (1);
		}
		i++;
	}
	log_msg(LOG_W, "Unknown command: %s\n", ctx->name);
	return (-1);
}

int	cmds(t_cmd_ctx *ctx)
{
	static t_cmd	*cmds = NULL;
	static int		cnt = 0;

	if (!cmds || ((cnt % 20 == 0 && ctx->func)))
	{
		ft_realloc((void **)&cmds, cnt, sizeof(t_cmd) * (cnt + 20));
		ft_memset(&cmds[cnt], 0, sizeof(t_cmd) * 20);
	}
	if (ctx->name && ctx->func)
	{
		cmds[cnt].name = ctx->name;
		cmds[cnt].func = ctx->func;
		cnt++;
		return (1);
	}
	else if (ctx->name)
		return (m_exec_cmd(cmds, ctx));
	ft_assert(0, "m_cmds called with invalid parameters");
	return (-1);
}
