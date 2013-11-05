#ifndef SIMPLE_LIST_H_
#define SIMPLE_LIST_H_
#include <stddef.h>
#include <stdlib.h>

struct simple_list_elem;

#define simple_list_add_head(head)	_simple_list_add_head((struct simple_list_elem**) head, calloc(1, sizeof **head))
#define simple_list_set_head(head, node)	_simple_list_add_head((struct simple_list_elem**) head, node)
#define simple_list_add_tail(head)	_simple_list_add_tail((struct simple_list_elem**) head, calloc(1, sizeof **head))
#define simple_list_set_tail(head, node)	_simple_list_add_tail((struct simple_list_elem**) head, node)
#define simple_list_add_before(head, value) *head == 0 ? simple_list_add_head(head) : \
	_simple_list_add_before((struct simple_list_elem**) head, calloc(1, sizeof **head), value, (void*) &(*(head))->value - (void*) *head)
#define simple_list_set_before(head, node, value) *head == 0 ? simple_list_set_head(head, node) : \
	_simple_list_add_before((struct simple_list_elem**) head, node, value, (void*) &(*(head))->value - (void*) *head)	
#define simple_list_find(head, value)	head == 0 ? NULL : \
	_simple_list_find((struct simple_list_elem*) head, value, (void*) &((head)->value) - (void*) head, 0)
#define simple_list_find_memcmp(head, value)	head == 0 ? NULL : \
	_simple_list_find((struct simple_list_elem*) head, value, (void*) &((head)->value) - (void*) head, sizeof(*value))
#define simple_list_find_cmp(head, value, comperator)	head == 0 ? NULL : \
	_simple_list_find_cmp((struct simple_list_elem*) head, value, (void*) &((head)->value) - (void*) head, comperator)
#define simple_list_remove(head, node)	_simple_list_remove((struct simple_list_elem**) head, (struct simple_list_elem*) node, 0)
#define simple_list_extract(head, node)	_simple_list_remove((struct simple_list_elem**) head, (struct simple_list_elem*) node, 1)
#define simple_list_clear(head)	_simple_list_clear((struct simple_list_elem**) head)

#define simple_list_for_each(head, node)	for (node = head; node; node = node->next)
#define simple_list_for_each_safe(head, node, prev, skipped)	\
		for (skipped = 0, prev = 0, node = head; node; prev = (skipped ? prev : node), node = (skipped ? node : node->next), skipped = 0)
#define simple_list_for_each_remove(head, node, prev)	{	\
	if (!prev) {					\
		skipped = 1;				\
		*head = (*head)->next;		\
	} else							\
		prev->next = node->next;	\
	free(node);						\
	node = prev ? prev : *head;		\
	}

void* _simple_list_add_head(struct simple_list_elem** head, void* mem);
void* _simple_list_add_tail(struct simple_list_elem** head, void* mem);
void* _simple_list_add_before(struct simple_list_elem** head, void* mem, int needle, int offset);
void* _simple_list_find(struct simple_list_elem* head, void* needle, int offset, size_t size);
void* _simple_list_find_cmp(struct simple_list_elem* head, void* needle, int offset, int compare(void*, void*));
void* _simple_list_remove(struct simple_list_elem** head, struct simple_list_elem* node, int keep);
void  _simple_list_clear(struct simple_list_elem** head);

#endif /* SIMPLE_LIST_H_ */
