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
#include "log.h"
#include "../encode/encode.h"
#include "parse_internal.h"

static int	m_skip_signs_spaces(const char **s, int *sign)
{
	int	ret;
	int	my_sign;

	*sign = +1;
	while (**s && ft_isspace((unsigned char)**s))
		(*s)++;
	if (**s == '+' || **s == '-')
	{
		if (**s == '-')
			*sign = -1;
		else
			*sign = +1;
		(*s)++;
		my_sign = *sign;
		ret = m_skip_signs_spaces(s, sign);
		*sign *= my_sign;
		return (ret);
	}
	if (*(*s) == '\0')
		return (-1);
	return (0);
}

static int	m_parse_label_term(const char **s, t_label *label_list,
	int *term)
{
	t_label	*lab;
	char	name[128];
	int		len;

	log_msg(LOG_D, "  Parsing label term in expr\n");
	(*s)++;
	len = 0;
	while (**s && (ft_strchr(LABEL_CHARS, **s) != NULL)
		&& (len < ((int) sizeof(name) - 1)))
	{
		name[len] = **s;
		len++;
		(*s)++;
	}
	name[len] = '\0';
	lab = find_label(label_list, name);
	if (!lab)
	{
		log_msg(LOG_E, "Error: Undefined label '%s' in expr\n", name);
		return (-1);
	}
	*term = lab->offset;
	return (0);
}

static int	m_parse_non_label_term(const char **s, int *term)
{
	char	*endptr;
	int32_t	v;

	endptr = (char *)*s;
	endptr += m_parse_num32(*s, (int32_t *)&v);
	if (endptr == *s || endptr < *s)
	{
		log_msg(LOG_E, "Error: Bad number in expr\n");
		return (-1);
	}
	*term = v;
	*s = endptr;
	return (0);
}

static int	m_eval_expr_adjust_acc(t_instr *instr, int arg_num,
			int32_t *out, int32_t acc)
{
	if ((m_is_pc_relative_op(instr->op)
			|| (instr->args[arg_num].type == ARG_IND))
		&& ft_strchr(instr->args[arg_num].expr, LABEL_CHAR))
	{
		acc -= instr->offset;
	}
	*out = (int32_t)acc;
	return (0);
}

int	eval_expr(t_instr *inst, int arg_num, int32_t *out, t_label *ll)
{
	const char	*s;
	int32_t		acc;
	int			sign;
	int32_t		term;
	int			ret;

	acc = 0;
	s = inst->args[arg_num].expr;
	if (*s == DIRECT_CHAR)
		s++;
	while (*s)
	{
		if (m_skip_signs_spaces(&s, &sign) == -1)
			break ;
		term = 0;
		if (*s == LABEL_CHAR)
			ret = m_parse_label_term(&s, ll, (int *)&term);
		else
			ret = m_parse_non_label_term(&s, (int *)&term);
		if (ret != 0)
			return (-1);
		acc += sign * term;
	}
	return (m_eval_expr_adjust_acc(inst, arg_num, out, (int32_t)acc));
}
