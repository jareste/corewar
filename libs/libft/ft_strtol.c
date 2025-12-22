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

long	ft_strtol(const char *restrict nptr, char **restrict endptr,
				int base)
{
	const char		*s;
	unsigned long	acc;
	unsigned long	lim;
	unsigned long	cutoff;
	int				cutlim;
	int				any;
	int				neg;
	int				digit;

	s = nptr;
	while (ft_isspace((unsigned char)*s))
		s++;
	neg = 0;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			neg = 1;
		s++;
	}
	if (base == 0)
	{
		if (*s == '0')
		{
			if ((s[1] == 'x' || s[1] == 'X') && ft_isxdigit((unsigned char)s[2]))
				base = 16;
			else
				base = 8;
		}
		else
			base = 10;
	}
	if (base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;
	acc = 0;
	lim = neg ? (unsigned long)LONG_MAX + 1UL : (unsigned long)LONG_MAX;
	cutoff = lim / (unsigned long)base;
	cutlim = (int)(lim % (unsigned long)base);
	any = 0;
	while (*s)
	{
		if (*s >= '0' && *s <= '9')
			digit = *s - '0';
		else if (*s >= 'a' && *s <= 'z')
			digit = *s - 'a' + 10;
		else if (*s >= 'A' && *s <= 'Z')
			digit = *s - 'A' + 10;
		else
			break;
		if (digit >= base)
			break;
		if (any >= 0)
		{
			if (acc > cutoff || (acc == cutoff && digit > cutlim))
			{
				any = -1;
				acc = lim;
			}
			else
			{
				any = 1;
				acc = acc * (unsigned long)base + (unsigned long)digit;
			}
		}
		s++;
	}
	if (endptr != NULL)
	{
		if (any == 0)
			*endptr = (char *)nptr;
		else
			*endptr = (char *)s;
	}
	if (any == -1)
		return (neg ? LONG_MIN : LONG_MAX);
	if (neg)
		return (-(long)acc);
	return ((long)acc);
}
