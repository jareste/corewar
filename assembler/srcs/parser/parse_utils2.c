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
#include "../encode/encode.h"

t_label	*m_new_label(const char *name)
{
	const char	*p;
	t_label		*label;

	if (!name || *name == '\0')
		return (NULL);
	p = name;
	while (*p)
	{
		if (ft_strchr(LABEL_CHARS, *p) == NULL)
			return (NULL);
		p++;
	}
	label = NEW(t_label, 1);
	label->name = ft_strdup(name);
	return (label);
}

t_op	*m_find_op(const char *name, size_t len)
{
	int	i;

	i = 0;
	while (i <= REG_NUMBER)
	{
		if (op_tab[i].name
			&& ft_strlen(op_tab[i].name) == len
			&& ft_strncmp(op_tab[i].name, name, len) == 0)
		{
			return (&op_tab[i]);
		}
		i++;
	}
	return (NULL);
}

int	m_parse_reg(const char *s, int *out)
{
	char	*endptr;
	long	val;

	errno = 0;
	val = strtol(s, &endptr, 10); /* todo implement it */
	log_msg(LOG_D, "Parsing int Errno=%d, endptr='%d' val=%d\n",
		errno, endptr == s, *endptr != '\0');
	if (errno != 0 || endptr == s || (*endptr != '\0'
			&& !ft_isspace((unsigned char)*endptr)))
		return (-1);
	if (val < INT_MIN || val > INT_MAX)
		return (-1);
	*out = (int)val;
	log_msg(LOG_D, "Parsed int: %d\n", *out);
	return (0);
}

int	m_parse_num32(const char *s, int32_t *out)
{
	char		*endptr;
	long long	val;

	if (ft_strncmp(s, "0x", 2) == 0 || ft_strncmp(s, "0X", 2) == 0)
	{
		s += 2;
		errno = 0;
		val = strtoll(s, &endptr, 16);  /* todo implement it */
	}
	else
	{
		errno = 0;
		val = strtoll(s, &endptr, 10);  /* todo implement it */
	}
	log_msg(LOG_D, "Parsing int32 Errno=%d, endptr='%d' val=%lld\n",
		errno, endptr == s, val);
	if (errno != 0 || endptr == s || (*endptr != '\0'
			&& !ft_isspace((unsigned char)*endptr)))
		return (-1);
	if (val < 0 && val < INT32_MIN)
		return (-1);
	if (val > (long long)UINT32_MAX)
		return (-1);
	*out = (int32_t)val;
	return (endptr - s);
}

int	m_mask_for_arg_type(t_arg_type t)
{
	if (t == ARG_REG)
		return (PARAM_REGISTER);
	else if (t == ARG_DIR || t == ARG_LABEL_DIR)
		return (PARAM_DIRECT);
	else if (t == ARG_IND || t == ARG_LABEL_IND)
		return (PARAM_INDIRECT);
	else
		return (PARAM_UNKNOWN);
}
