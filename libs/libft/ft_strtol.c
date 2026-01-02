/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtol.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:26:26 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/08 23:53:24 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

static unsigned long	get_lim(int neg)
{
	if (neg)
		return ((unsigned long)LONG_MAX + 1UL);
	return ((unsigned long)LONG_MAX);
}

static void	m_set_any_acc(int *any, int any_val,
	unsigned long *acc, unsigned long lim)
{
	*any = any_val;
	*acc = lim;
}

static int	parse_digits(const char **ps, unsigned long *acc,
		unsigned long lim, int base)
{
	const char		*s;
	unsigned long	cutoff;
	int				cutlim;
	int				any;
	int				digit;

	s = *ps;
	cutoff = lim / (unsigned long)base;
	cutlim = (int)(lim % (unsigned long)base);
	any = 0;
	while (*s)
	{
		digit = digit_value(*s);
		if (digit < 0 || digit >= base)
			break ;
		if (any >= 0 && (*acc > cutoff || (*acc == cutoff && digit > cutlim)))
			m_set_any_acc(&any, -1, acc, lim);
		else if (any >= 0)
			m_set_any_acc(&any, 1, acc, (*acc * (t_ul)base + (t_ul)digit));
		s++;
	}
	*ps = s;
	return (any);
}

long	ft_strtol(const char *restrict nptr, char **restrict endptr, int base)
{
	const char		*s;
	unsigned long	acc;
	unsigned long	lim;
	int				any;
	int				neg;

	s = skip_spaces_sign(nptr, &neg);
	base = detect_base(s, base);
	s = skip_0x(s, base);
	acc = 0;
	lim = get_lim(neg);
	any = parse_digits(&s, &acc, lim, base);
	set_endptr(endptr, nptr, s, any);
	if (any == -1)
	{
		if (neg)
			return (LONG_MIN);
		return (LONG_MAX);
	}
	if (neg)
		return (-(long)acc);
	return ((long)acc);
}
