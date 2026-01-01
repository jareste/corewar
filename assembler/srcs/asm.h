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

#ifndef ASM_H
# define ASM_H
# include "../../op/op.h"
# include <ft_malloc.h>
# include <ft_list.h>
# include <error_codes.h>

typedef enum e_arg_kind
{
	ARG_REG,
	ARG_DIR,
	ARG_IND,
	ARG_LABEL_DIR,
	ARG_LABEL_IND,
}	t_arg_type;

typedef struct s_arg
{
	t_arg_type	type;
	union
	{
		int		value;
		char	*label;
	} u_;
	char		*expr;

}	t_arg;

typedef struct s_instr
{
	t_list_item	l;
	t_op		*op;
	int			line_no;
	int			offset; /* offset from start of code */
	t_arg		args[3];
	int			arg_count;
	char		*label; /* label defined on this line (if any) */
	int			raw_len; /* length of raw data for non-opcode lines */
	char		*raw; /* raw data for non-opcode lines */
}	t_instr;

typedef struct s_label
{
	t_list_item	l;
	char		*name;
	int			offset; /* byte offset where label points */
	int			line_no;
}	t_label;

typedef struct s_parser_state
{
	t_label		*l_l;
	t_instr		*i_l;
	int			prog_size;
}	t_parser_state;

#endif
