/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtoul.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:26:26 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/08 23:53:24 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

static void	m_set_any_acc(int *any, int any_val,
	t_ul *acc, t_ul lim)
{
	*any = any_val;
	*acc = lim;
}

static int	parse_digits(const char **ps, unsigned long *acc, int base)
{
	const char		*s;
	unsigned long	cutoff;
	int				cutlim;
	int				any;
	int				digit;

	s = *ps;
	cutoff = ULONG_MAX / (unsigned long)base;
	cutlim = (int)(ULONG_MAX % (unsigned long)base);
	any = 0;
	while (*s)
	{
		digit = digit_value(*s);
		if (digit < 0 || digit >= base)
			break ;
		if (any >= 0 && (*acc > cutoff || (*acc == cutoff && digit > cutlim)))
			m_set_any_acc(&any, -1, acc, ULONG_MAX);
		else if (any >= 0)
			m_set_any_acc(&any, 1, acc, (*acc * (t_ul)base + (t_ul)digit));
		s++;
	}
	*ps = s;
	return (any);
}

unsigned long	ft_strtoul(const char *restrict nptr,
		char **restrict endptr, int base)
{
	const char		*s;
	unsigned long	acc;
	int				any;
	int				neg;

	s = skip_spaces_sign(nptr, &neg);
	base = detect_base(s, base);
	s = skip_0x(s, base);
	acc = 0;
	any = parse_digits(&s, &acc, base);
	set_endptr(endptr, nptr, s, any);
	if (neg)
		return ((unsigned long)(-(long)acc));
	return (acc);
}
