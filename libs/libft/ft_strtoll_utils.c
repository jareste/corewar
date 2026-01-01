/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:26:26 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/08 23:53:24 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

const char	*skip_0x(const char *s, int base)
{
	if (base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		return (s + 2);
	return (s);
}

unsigned long long	get_lim(int neg)
{
	if (neg)
		return ((unsigned long long)LLONG_MAX + 1ULL);
	return ((unsigned long long)LLONG_MAX);
}

void	set_endptr(char **endptr, const char *nptr,
		const char *s, int any)
{
	if (endptr == NULL)
		return ;
	if (any == 0)
		*endptr = (char *)nptr;
	else
		*endptr = (char *)s;
}

unsigned long long	acc_step(unsigned long long acc,
		unsigned long long base, int digit)
{
	return (acc * base + (unsigned long long)digit);
}

int	parse_digits(const char **ps, unsigned long long *acc,
		unsigned long long lim, int base)
{
	const char			*s;
	unsigned long long	cutoff;
	int					cutlim;
	int					any;
	int					digit;

	s = *ps;
	cutoff = lim / (unsigned long long)base;
	cutlim = (int)(lim % (unsigned long long)base);
	any = 0;
	while (*s)
	{
		digit = digit_value(*s);
		if (digit < 0 || digit >= base)
			break ;
		if (any >= 0 && (*acc > cutoff || (*acc == cutoff && digit > cutlim)))
		{
			any = -1;
			*acc = lim;
		}
		else if (any >= 0)
		{
			any = 1;
			*acc = acc_step(*acc, (unsigned long long)base, digit);
		}
		s++;
	}
	*ps = s;
	return (any);
}
