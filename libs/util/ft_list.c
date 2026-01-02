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

#include "ft_list.h"
#include "error_codes.h"
#include <stddef.h>

int	ft_list_add_last(void **_head, void *_node)
{
	t_list_item	**head;
	t_list_item	*node;
	t_list_item	*last;

	head = (t_list_item **)_head;
	node = (t_list_item *)_node;
	if (!head || !node)
		return (INVALID_ARGS);
	if (!*head)
	{
		*head = node;
		node->next = node;
		node->prev = node;
		return (OK);
	}
	last = (*head)->prev;
	last->next = node;
	node->prev = last;
	node->next = *head;
	(*head)->prev = node;
	return (OK);
}

int	ft_list_add_first(void **_head, void *_node)
{
	int	ret;

	ret = ft_list_add_last(_head, _node);
	if (ret != OK)
		return (ret);
	*(t_list_item **)_head = (t_list_item *)_node;
	return (OK);
}

// int	ft_list_add_first(void **_head, void *_node)
// {
// 	t_list_item	**head;
// 	t_list_item	*node;
// 	t_list_item	*first;

// 	head = (t_list_item **)_head;
// 	node = (t_list_item *)_node;
// 	if (!head || !node)
// 		return (INVALID_ARGS);
// 	if (!*head)
// 	{
// 		*head = node;
// 		node->next = node;
// 		node->prev = node;
// 		return (OK);
// 	}
// 	first = *head;
// 	while (first->prev != *head)
// 		first = first->prev;
// 	first->prev = node;
// 	node->next = first;
// 	return (OK);
// }

void	*ft_list_get_next(void **_head, void *_node)
{
	t_list_item	**head;
	t_list_item	*node;

	head = (t_list_item **)_head;
	node = (t_list_item *)_node;
	if (!head || !node)
		return (NULL);
	if (!*head)
		return (NULL);
	if (node->next == *head)
		return (NULL);
	return (node->next);
}

void	*ft_list_get_prev(void **_head, void *_node)
{
	t_list_item	**head;
	t_list_item	*node;

	head = (t_list_item **)_head;
	node = (t_list_item *)_node;
	if (!head || !node)
		return (NULL);
	if (!*head)
		return (NULL);
	if (node->prev == *head)
		return (NULL);
	return (node->prev);
}

int	ft_list_pop(void **_head, void *_node)
{
	t_list_item	**head;
	t_list_item	*node;
	t_list_item	*prev;
	t_list_item	*next;

	head = (t_list_item **)_head;
	node = (t_list_item *)_node;
	if (!head || !node || !*head)
		return (INVALID_ARGS);
	prev = node->prev;
	next = node->next;
	if (node == node->next && node == node->prev)
		*head = NULL;
	else
	{
		prev->next = next;
		next->prev = prev;
		if (*head == node)
			*head = next;
	}
	node->next = NULL;
	node->prev = NULL;
	return (OK);
}
