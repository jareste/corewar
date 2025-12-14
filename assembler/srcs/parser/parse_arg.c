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
#include <libft.h>
#include <stdbool.h>
#include "log.h"
#include "../encode/encode.h"
#include "parse_internal.h"

static int	m_parse_tok_reg(const char *s, size_t len, t_arg *out)
{
	int	reg;

	log_msg(LOG_D, "Parsing register argument: '%s'\n", s);
	log_msg(LOG_D, "Length: %zu\n", len);
	if (len < 2)
		return (-1);
	reg = 0;
	m_parse_reg(s + 1, &reg);
	log_msg(LOG_D, "Register number parsed: %d\n", reg);
	if (m_parse_reg(s + 1, &reg) != 0)
		return (-1);
	if (reg < 0 || reg > REG_NUMBER)
		return (-1);
	out->type = ARG_REG;
	out->u_.value = reg;
	return (0);
}

static int	m_parse_tok_direct(char *s, size_t len, t_arg *out)
{
	int32_t	val;
	char	*end;

	log_msg(LOG_D, "Parsing direct argument: '%s'\n", s);
	if (len < 2)
		return (-1);
	if (s[1] == LABEL_CHAR)
	{
		out->type = ARG_LABEL_DIR;
		end = s + 2 + ft_strcspn(s + 2, " \t\n\r\f\v");
		out->u_.label = ft_strndup(s + 2, end - (s + 2));
		if (!out->u_.label)
			return (-1);
		log_msg(LOG_D, "Direct label parsed: '%s' %zu\n",
			out->u_.label, ft_strlen(out->u_.label));
		return (0);
	}
	else
	{
		if (m_parse_num32(s + 1, &val) == -1)
			return (-1);
		out->type = ARG_DIR;
		out->u_.value = val;
		return (0);
	}
}

static int	m_parse_tok_indirect(char *s, size_t len, t_arg *out)
{
	int32_t	val;
	char	*end;

	(void)len;
	log_msg(LOG_D, "Parsing indirect argument: '%s'\n", s);
	if (s[0] == LABEL_CHAR)
	{
		out->type = ARG_LABEL_IND;
		end = s + 1 + ft_strcspn(s + 1, " \t\n\r\f\v");
		out->u_.label = ft_strndup(s + 1, end - (s + 1));
		if (!out->u_.label)
			return (-1);
		log_msg(LOG_D, "Indirect label parsed: '%s' %zu\n",
			out->u_.label, ft_strlen(out->u_.label));
		return (0);
	}
	else
	{
		if (m_parse_num32(s, &val) == -1)
			return (-1);
		out->type = ARG_IND;
		out->u_.value = val;
		return (0);
	}
}

int	m_parse_arg_token(const char *arg_str, t_arg *out)
{
	char	*s;
	size_t	len;

	s = m_skip_spaces((char *)arg_str);
	len = ft_strlen(s);
	if (len == 0)
		return (-1);
	if (s[0] == 'r')
		return (m_parse_tok_reg(s, len, out));
	else if (s[0] == DIRECT_CHAR)
		return (m_parse_tok_direct(s, len, out));
	else
		return (m_parse_tok_indirect(s, len, out));
}
