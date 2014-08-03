/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file	slist.h
 * @brief	simple single linked list implementation with iterators
 *
 *			A list entry is a struct of an arbitrary type but with the only constraint that
 *			it's first entry is a pointer of the same type as the struct with the name next.
 *
 *			A list is defined by it's first entry, called head. Since the first entry may
 *			change, a pointer to the first entry is used to refer to the list.
 *
 *			The list automatically allocates and deallocates memory when list elements
 *			are added or removed
 *
 * @author	Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 */


#ifndef SIMPLE_LIST_H_
#define SIMPLE_LIST_H_
#include <stddef.h>
#include <stdlib.h>

struct simple_list_elem;

/**
 * @brief	allocates memory for a new list entry and appends it before the head. The new entry is the new head.
 *
 * @param	head 	pointer to the list
 * @return	the new list entry, NULL if no new list entry could be allocated
 */
#define simple_list_add_head(head)	__simple_list_add_head((struct simple_list_elem**) (head), calloc(1, sizeof **(head)))


/**
 * @brief	appends a preallocated element to the top of the list. The new entry is the new head.
 *
 * @param	head 	pointer to the list
 * @param	node	preallocated list element
 * @return	the new list entry (node)
 */
#define simple_list_set_head(head, node)	__simple_list_add_head((struct simple_list_elem**) head, node)

/**
* @brief	allocates memory for a new list entry and appends it at the end of the list.
*
* @param	head pointer to pointer to the first list element
* @param	head 	pointer to the list
* @return	the new list entry, NULL if no new list entry could be allocated
*/
#define simple_list_add_tail(head)	__simple_list_add_tail((struct simple_list_elem**) (head), calloc(1, sizeof **(head)))

/**
 * @brief	appends a preallocated element to the end of the list.
 *
 * @param	head 	pointer to the list
 * @param	node	preallocated list element
 * @return	the new list entry (node)
 */
#define simple_list_set_tail(head, node)	__simple_list_add_tail((struct simple_list_elem**) (head), (node))

/**
* @brief	allocates memory for a new list entry and adds it before an existing entry.
*			The new entry is added before the existing element where old_entry->value > value
*			If no such entry could be found, the new entry will be added at the end of the list
*
* @param	head 	pointer to the list
* @param	value	value to compare list entries
*					has to be the same name as the element in the list entry structure that is be used for comparison
* @return	pointer to the new element, NULL if no new list element could be allocated
*/
#define simple_list_add_before(head, value) *(head) == 0 ? simple_list_add_head((head)) : \
	__simple_list_add_before((struct simple_list_elem**) (head), calloc(1, sizeof **(head)), value, (void*) &(*(head))->value - (void*) *(head))

/**
* @brief	adds an preallocated list element before an existing one.
*			The new entry is added before the existing element where old_entry->value > value
*			If no such entry could be found, the new entry will be added at the end of the list *
* @param	head 	pointer to the list
* @param	value	value to compare list entries
*					has to be the same name as the element in the list entry structure that is be used for comparison
* @param	node	preallocated list element
* @return	the new list entry (node)
*/
#define simple_list_set_before(head, node, value) *(head) == 0 ? simple_list_set_head((head), (node)) : \
	__simple_list_add_before((struct simple_list_elem**) (head), (node), (value), (void*) &(*(head))->value - (void*) *(head))

/**
* @brief	searches for a list element by simple comparison of a struct value
*
* @param	head 	pointer to the list
* @param	value	the member value of a list entry that is to be found
*					has to be the same name as the value in the list element struct
* @return	pointer the list entry if found, otherwise NULL
*/
#define simple_list_find(head, value)	(head) == 0 ? NULL : \
	__simple_list_find((struct simple_list_elem*) (head), (value), (void*) &((head)->value) - (void*) (head), 0)

/**
* @brief	searches for a list element by comparing a buffer in the list element struct
*
* @param	head 	pointer to the list
* @param	value	pointer to the buffer that is to be found in the list
*					has to be the same name as the value in the list element struct
* @return	pointer the list entry if found, otherwise NULL
*/
#define simple_list_find_memcmp(head, value)	(head) == 0 ? NULL : \
	__simple_list_find((struct simple_list_elem*) (head), (value), (void*) &((head)->value) - (void*) (head), sizeof(*(value)))

/**
* @brief	searches for a list element by applying a comparator function to each list entry
*
* @param	head 	pointer to the list
* @param	value	input to the comparator function
* @param	comperator	a function that takes (value, node) and returns 0 if they match
* @return	pointer	the list entry if found, otherwise NULL
*/
#define simple_list_find_cmp(head, value, comperator)	(head) == 0 ? NULL : \
	__simple_list_find_cmp((struct simple_list_elem*) (head), (value), (void*) &((head)->value) - (void*) (head), (comperator))

/**
 * @brief	removes an entry from the list and frees it's memory
 *
 * @param	head 	pointer to the list
 * @param	node	entry to be removed
 * @returns a non-zero value if the element was found and removed
 */
#define simple_list_remove(head, node)	__simple_list_remove((struct simple_list_elem**) (head), (struct simple_list_elem*) (node), 0)

/**
 * @brief removes an entry from the list, doesn't free it's memory but returns the element
 *
 * @param	head 	pointer to the list
 * @param	node	entry to be extracted
 * @returns pointer to the element, NULL if it couldn't be found
 */
#define simple_list_extract(head, node)	__simple_list_remove((struct simple_list_elem**) (head), (struct simple_list_elem*) (node), 1)

/**
 * @brief	removes all entries from the list and frees their memory
 *
 * @param	head 	pointer to the list
 */
#define simple_list_clear(head)	__simple_list_clear((struct simple_list_elem**) (head))

/**
 * @brief	starts a loop to iterate over all list entries. Read-only list access only.
 *			needs to be provided with a local loop variable
 *
 * @param	head 	pointer to the list
 * @param	node	to the current entry (loop variable)
 */
#define simple_list_for_each(head, node)	for ((node) = (head); (node); (node) = (node)->next)

/**
 * @brief	starts a loop to iterate over all list elements with the possibility to remove elements
 			to remove an element, use simple_list_for_each_remove
 			needs to be provided with a local loop variable as well as two local auxiliary variables
 *
 * @param	head 	pointer to the list
 * @param	node	to the current entry (loop variable)
 * @param	prev 	internal variable, pointer to previous list entry - do not modify
 * @param	skipped	internal variable, integer - do not modify
 */
#define simple_list_for_each_safe(head, node, prev, skipped)	\
		for ((skipped) = 0, (prev) = 0, (node) = (head);\
			(node);					\
			(prev) = ((skipped) ? (prev) : (node)),	\
			(node) = ((skipped) ? (node) : (node)->next), (skipped) = 0)

/**
 * @brief	removes an element in a simple_list_for_each_safe context
 *
 * @param	head 	pointer to the list
 * @param	node 	pointer to the current entry (loop variable)
 * @param	prev 	internal variable, provided by simple_list_for_each_safe
 */
#define simple_list_for_each_remove(head, node, prev)	do {	\
	if (!prev) {				\
		(skipped) = 1;			\
		*(head) = (*(head))->next;	\
	} else					\
		(prev)->next = (node)->next;	\
	free(node);				\
	(node) = (prev) ? (prev) : *(head);	\
	} while (0)

void *__simple_list_add_head(struct simple_list_elem **head, void *mem);
void *__simple_list_add_tail(struct simple_list_elem **head, void *mem);
void *__simple_list_add_before(struct simple_list_elem **head, void *mem, int needle, int offset);
void *__simple_list_find(struct simple_list_elem *head, void *needle, int offset, size_t size);
void *__simple_list_find_cmp(struct simple_list_elem *head, void *needle, int offset, int compare(void *, void *));
void *__simple_list_remove(struct simple_list_elem **head, struct simple_list_elem *node, int keep);
void  __simple_list_clear(struct simple_list_elem **head);

#endif /* SIMPLE_LIST_H_ */
