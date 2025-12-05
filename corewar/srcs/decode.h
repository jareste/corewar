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

#ifndef DECODE_H
# define DECODE_H

# include <stdint.h>
# include "corewar.h"

int		decode_file(const char *filename, t_champ *champ);

int		read_n_bytes(uint8_t *mem, int addr, int n);
void	write_n_bytes(uint8_t *mem, int addr, int value, int n);
int		mod_addr(int addr);
int		get_arg_val(t_vm *vm, t_proc *p, t_arg *a, int apply_idx_mod);

#endif /* DECODE_H */
