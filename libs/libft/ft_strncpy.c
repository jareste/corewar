/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 21:11:13 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/09 00:13:36 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strncpy(char *s1, const char *s2, size_t n)
{
	size_t size;
	
	size = ft_strnlen(s2, n);
	if (size != n)
		ft_memset(s1 + size, '\0', n - size);
	return (ft_memcpy(s1, s2, size));
}
