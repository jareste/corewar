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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "libft.h"
#include "../corewar.h"
#include "ft_printf.h"
#include "../process/process.h"
#include "../decode/decode.h"
#include "parse_internal.h"

static int	load_champ_with_id(t_vm *vm, char *path, int id)
{
	int		slot;
	t_champ	*champ;

	if (vm->champ_count >= MAX_PLAYERS)
	{
		ft_dprintf(2, "Too many champions\n");
		return (-1);
	}
	if (id < 1 || id > MAX_PLAYERS)
	{
		ft_dprintf(2, "Invalid champ id: %d\n", id);
		return (-1);
	}
	if (id_used(vm, id))
	{
		ft_dprintf(2, "Duplicate champ id: %d\n", id);
		return (-1);
	}
	slot = find_free_slot(vm);
	champ = &vm->champs[slot];
	decode_file(path, champ);
	champ->id = id;
	vm->champ_count++;
	load_champ_code(vm, champ);
	return (0);
}

static int	handle_dump(t_vm *vm, char **av, int *i, int ac)
{
	int	n;

	if (*i + 1 >= ac)
	{
		ft_dprintf(2, "-dump needs a number\n");
		exit(1);
	}
	if (vm->dump_enabled)
	{
		ft_dprintf(2, "-dump specified twice\n");
		exit(1);
	}
	if (parse_int_strict(av[*i + 1], &n) != 0 || n < 0)
	{
		ft_dprintf(2, "Invalid -dump value: %s\n", av[*i + 1]);
		exit(1);
	}
	vm->dump_enabled = 1;
	vm->dump_cycle = n;
	*i += 2;
	return (0);
}

static int	handle_n(t_vm *vm, char **av, int *i, int ac)
{
	int	id;

	if (*i + 2 >= ac)
	{
		ft_dprintf(2, "-n needs: <id> <file.cor>\n");
		exit(1);
	}
	if (parse_int_strict(av[*i + 1], &id) != 0)
	{
		ft_dprintf(2, "Invalid id for -n: %s\n", av[*i + 1]);
		exit(1);
	}
	if (!is_cor_file(av[*i + 2]))
	{
		ft_dprintf(2, "Invalid champ file: %s\n", av[*i + 2]);
		exit(1);
	}
	if (load_champ_with_id(vm, av[*i + 2], id) != 0)
		exit(1);
	*i += 3;
	return (0);
}

static int	handle_cor(t_vm *vm, char *path)
{
	int	id;

	if (!is_cor_file(path))
	{
		ft_dprintf(2, "Invalid file: %s\n", path);
		exit(1);
	}
	id = find_free_id(vm);
	if (id < 0)
	{
		ft_dprintf(2, "No free champion id left\n");
		exit(1);
	}
	return (load_champ_with_id(vm, path, id));
}

int	parse_av(t_vm *vm, char **av, int ac)
{
	int	i;

	i = 0;
	vm->champ_count = 0;
	vm->dump_enabled = 0;
	vm->dump_cycle = 0;
	while (i < ac)
	{
		if (ft_strncmp(av[i], "-dump", 5) == 0 && av[i][5] == '\0')
			handle_dump(vm, av, &i, ac);
		else if (ft_strncmp(av[i], "-n", 2) == 0 && av[i][2] == '\0')
			handle_n(vm, av, &i, ac);
		else
		{
			handle_cor(vm, av[i]);
			i++;
		}
	}
	if (vm->champ_count == 0)
	{
		ft_dprintf(2, "No champions provided\n");
		return (-1);
	}
	return (0);
}
