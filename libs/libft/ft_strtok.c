/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 21:42:38 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/09 00:19:20 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtok(char *restrict str, const char *restrict delim)
{
	static char	*static_str = NULL;
	char		*start;
	char		*end;

	if (str)
		static_str = str;
	if (!static_str)
		return NULL;
	start = static_str + ft_strspn(static_str, delim);
	if (*start == '\0')
	{
		static_str = NULL;
		return NULL;
	}
	end = start + ft_strcspn(start, delim);
	if (*end == '\0')
	{
		static_str = NULL;
	}
	else
	{
		*end = '\0';
		static_str = end + 1;
	}
	return start;
}
