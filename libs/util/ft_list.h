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

#ifndef FT_LIST_H
# define FT_LIST_H

typedef struct list_item_s
{
	struct list_item_s	*next;
	struct list_item_s	*prev;
}	t_list_item;

int		ft_list_add_last(void **head, void *node);
int		ft_list_add_first(void **head, void *node);
void	*ft_list_get_next(void **head, void *node);
void	*ft_list_get_prev(void **head, void *node);
int		ft_list_pop(void **head, void *node);
int		ft_list_pop_first(void **head);
int		ft_list_pop_last(void **head);
int		ft_list_get_size(void **head);
void	*ft_list_get_first(void **head);
void	*ft_list_get_last(void **head);
int		ft_list_find_node(void **head, void *node);

#endif /* FT_LIST_H */
