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

#undef malloc
#undef realloc

void* ft_malloc(size_t size)
{
#undef malloc
    void *ptr = malloc(size);
    ft_assert(ptr != NULL, "malloc failed");
#define malloc(x) ft_malloc(x)
    return ptr;
}

void* ft_realloc(void *ptr, size_t size)
{
#undef realloc
    void* new_ptr = realloc(ptr, size);
    ft_assert(new_ptr != NULL, "realloc failed");
#define realloc(x, y) ft_realloc(x, y)
    return new_ptr;
}

char *ft_strdup(const char *s)
{
    size_t len = strlen(s);
    char *new_s = ft_malloc(len + 1);
    strcpy(new_s, s);
    new_s[len] = '\0';
    return new_s;
}

char    *ft_strndup(const char *s, size_t n)
{
    size_t len = strnlen(s, n);
    char *new_s = ft_malloc(len + 1);
    strncpy(new_s, s, len);
    new_s[len] = '\0';
    return new_s;
}

