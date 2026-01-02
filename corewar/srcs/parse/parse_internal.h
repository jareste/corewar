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

#ifndef PARSE_INTERNAL_H
# define PARSE_INTERNAL_H

int		is_number_str(char *s);
int		is_cor_file(char *s);

int		parse_int_strict(char *s, int *out);
int		id_used(t_vm *vm, int id);
int		find_free_slot(t_vm *vm);
void	load_champ_code(t_vm *vm, t_champ *champ);
int		find_free_id(t_vm *vm);

#endif /* PARSE_INTERNAL_H */
