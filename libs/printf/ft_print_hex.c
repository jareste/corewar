/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_hex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:15:25 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/15 16:20:33 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "../libft/libft.h"

void	put_hex_n(uint32_t v, int digits)
{
	static char	hex[] = "0123456789abcdef";
	char		buf[16];
	int			i;

	i = digits;
	while (i > 0)
	{
		i--;
		buf[i] = hex[v & 0xF];
		v >>= 4;
	}
	(void)!write(1, buf, digits);
}

void	put_hex_byte(uint8_t b)
{
	static char	hex[] = "0123456789abcdef";
	char		buf[2];

	buf[0] = hex[(b >> 4) & 0xF];
	buf[1] = hex[b & 0xF];
	(void)!write(1, buf, 2);
}

int	ft_print_hex(int fd, unsigned long int n, char format, int c_printed)
{
	char	*basel;
	char	*baseu;

	basel = "0123456789abcdef";
	baseu = "0123456789ABCDEF";
	if (n > 15)
		c_printed = ft_print_hex(fd, n / 16, format, c_printed);
	if (c_printed == -1)
		return (-1);
	if (format == 'x')
	{
		c_printed++;
		if (ft_print_char_fd(fd, basel[n % 16], 1) == -1)
			return (-1);
	}
	if (format == 'X')
	{
		c_printed++;
		if (ft_print_char_fd(fd, baseu[n % 16], 1) == -1)
			return (-1);
	}
	return (c_printed);
}
