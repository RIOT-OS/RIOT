/**
 * simple queue implementation
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <stddef.h>

#include "queue.h"

//#define ENABLE_DEBUG 0
#include "debug.h"

void queue_remove(queue_node_t *root, queue_node_t *node)
{
    while(root->next != NULL) {
        if(root->next == node) {
            root->next = node->next;
            node->next = NULL;
            return;
        }

        root = root->next;
    }
}

queue_node_t *queue_remove_head(queue_node_t *root)
{
    queue_node_t *head = root->next;

    if(head != NULL) {
        root->next = head->next;
    }

    return head;
}

void queue_add_tail(queue_node_t *node, queue_node_t *new_obj)
{
    while(node->next != NULL) {
        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}

void queue_add_head(queue_node_t *root, queue_node_t *new_obj)
{
    new_obj->next = root->next;
    root->next = new_obj;
}

void queue_priority_add(queue_node_t *root, queue_node_t *new_obj)
{
    queue_node_t *node = root;

    while(node->next != NULL) {
        if(node->next->priority > new_obj->priority) {
            new_obj->next = node->next;
            node->next = new_obj;
            return;
        }

        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}

void queue_priority_add_generic(queue_node_t *root, queue_node_t *new_obj, int (*cmp)(queue_node_t *, queue_node_t *))
{
    queue_node_t *node = root;

    while(node->next != NULL) {
        if(cmp(node->next, new_obj) < 0) {
            new_obj->next = node->next;
            node->next = new_obj;
            return;
        }

        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}


void queue_print(queue_node_t *node)
{
    printf("queue:\n");

    while(node->next != NULL) {
        node = node->next;
        printf("Data: %u Priority: %u\n", node->data, node->priority);
    }
}

void queue_print_node(queue_node_t *node)
{
    printf("Data: %u Priority: %u Next: %u\n", (unsigned int)node->data, node->priority, (unsigned int)node->next);
}

/*
int main() {
    queue_node_t root, a, b, c,d;

    memset(&d, '\0', sizeof(queue_node_t));
    memset(&a, '\0', sizeof(queue_node_t));
    memset(&b, '\0', sizeof(queue_node_t));
    memset(&c, '\0', sizeof(queue_node_t));

    root.data = 0;
    root.next = NULL;

    a.data = 1;
    a.priority = 1;
    b.data = 2;
    b.priority = 2;
    c.data = 0;
    c.priority = 5;
    d.data = 4;
    d.priority = 4;

    queue_add_tail(&root, &a);
    queue_add_tail(&root, &b);
    queue_add_tail(&root, &c);
    queue_add_tail(&root, &d);

    queue_print(&root);

    queue_remove(&root, &c);

    queue_print(&root);

    queue_priority_add(&root, &c);

    queue_print(&root);

    return 0;
}*/
