/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 00:31:17 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/19 23:59:13 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GNL_H
# define GNL_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 500000000
# endif

# include <limits.h>
# include <unistd.h>
# include <stdlib.h>

char	*get_next_line(int fd);
char	*new_line(char *buffer);
char	*ft_find_line(int fd, char *buffer);
char	*gnl_ft_strjoin(char *s1, char *s2);
char	*gnl_ft_substr(char *s, unsigned int start, size_t len);
size_t	gnl_ft_strlen(const char *s);
char	*gnl_ft_strchr(char *s, int c);
char	*gnl_ft_free(char **buffer);
#endif
