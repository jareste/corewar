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

int	detect_base(const char *s, int base)
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

const char	*skip_spaces_sign(const char *s, int *neg)
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
