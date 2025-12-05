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

#ifndef LOG_H
# define LOG_H

# include <stdio.h>
# include <stdbool.h>

# define LOG_FILE "log.txt"

typedef enum e_log_level
{
	LOG_E,
	LOG_B,
	LOG_W,
	LOG_I,
	LOG_D,
}	t_log_level;

typedef struct s_log_config
{
	t_log_level	log_level;
	char		*log_file_path;
	bool		log_erase;
}	t_log_config;

int		log_init(void);

void	log_close(void);

void	log_msg(t_log_level level, const char *fmt, ...);
void	log_msg_time(t_log_level level, const char *fmt, ...);

#endif /* LOG_H */
