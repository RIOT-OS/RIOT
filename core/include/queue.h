/**
 * @ingroup kernel
 * @{
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __QUEUE_H
#define __QUEUE_H

typedef struct queue_node_t {
    struct queue_node_t *next;
    unsigned int data;
    unsigned int priority;
} queue_node_t;

queue_node_t *queue_remove_head(queue_node_t *root);
void queue_add_tail(queue_node_t *root, queue_node_t *new_obj);
void queue_add_head(queue_node_t *root, queue_node_t *new_obj);
queue_node_t *queue_remove_head(queue_node_t *root);
void queue_priority_add(queue_node_t *root, queue_node_t *new_obj);
void queue_priority_add_generic(queue_node_t *root, queue_node_t *new_obj, int(*cmp)(queue_node_t *, queue_node_t *)) ;
void queue_remove(queue_node_t *root, queue_node_t *node);

/** @} */
#endif // __QUEUE_H
