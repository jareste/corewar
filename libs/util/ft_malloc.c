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

#include "ft_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void	*ft_memset(void *b, int c, unsigned long long len);

void	*ft_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	ft_assert(ptr != NULL, "malloc failed");
	return (ptr);
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;

	ptr = malloc(count * size);
	ft_memset(ptr, 0, count * size);
	ft_assert(ptr != NULL, "calloc failed");
	return (ptr);
}

char	*ft_strdup(const char *s)
{
	size_t	len;
	char	*new_s;

	len = strlen(s);
	new_s = ft_malloc(len + 1);
	strcpy(new_s, s);
	new_s[len] = '\0';
	return (new_s);
}

char	*ft_strndup(const char *s, size_t n)
{
	size_t	len;
	char	*new_s;

	len = strnlen(s, n);
	new_s = ft_malloc(len + 1);
	strncpy(new_s, s, len);
	new_s[len] = '\0';
	return (new_s);
}

void	ft_assert(int expr, const char *message)
{
	if (!expr)
	{
		fprintf(stderr, "Assertion failed: %s\n", message);
		exit(EXIT_FAILURE);
	}
}
