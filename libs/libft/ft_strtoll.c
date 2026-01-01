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

int	digit_value(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'z')
		return (c - 'a' + 10);
	if (c >= 'A' && c <= 'Z')
		return (c - 'A' + 10);
	return (-1);
}

static const char	*skip_spaces_sign(const char *s, int *neg)
{
	while (ft_isspace((unsigned char)*s))
		s++;
	*neg = 0;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			*neg = 1;
		s++;
	}
	return (s);
}

static int	detect_base(const char *s, int base)
{
	if (base != 0)
		return (base);
	if (*s == '0')
	{
		if ((s[1] == 'x' || s[1] == 'X')
			&& ft_isxdigit((unsigned char)s[2]))
			return (16);
		return (8);
	}
	return (10);
}

long long	ft_strtoll(const char *restrict nptr,
		char **restrict endptr, int base)
{
	const char			*s;
	unsigned long long	acc;
	unsigned long long	lim;
	int					any;
	int					neg;

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
			return (LLONG_MIN);
		return (LLONG_MAX);
	}
	if (neg)
		return (-(long long)acc);
	return ((long long)acc);
}
