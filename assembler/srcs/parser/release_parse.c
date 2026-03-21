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

#include "../asm.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <libft.h>
#include "parse_internal.h"
#include "log.h"
#include "ft_malloc.h"
#include "../encode/encode.h"

void	release_parser_state(t_parser_state *p_st)
{
	t_label	*lab;
	t_label	*old_lab;
	t_instr	*inst;
	t_instr	*old_inst;

	lab = p_st->l_l;
	while (lab)
	{
		free(lab->name);
		old_lab = lab;
		lab = ft_list_get_next((void **)&p_st->l_l, (void *)lab);
		free(old_lab);
	}
	inst = p_st->i_l;
	while (inst)
	{
		old_inst = inst;
		inst = ft_list_get_next((void **)&p_st->i_l, (void *)inst);
		if (old_inst->raw)
			free(old_inst->raw);
		free(old_inst);
	}
}
