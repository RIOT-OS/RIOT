/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
* @file	list.c
* @author	Benjamin Valentin <benpicco@zedat.fu-berlin.de>
*/

#include <stdlib.h>
#include <string.h>

#include "list.h"

struct simple_list_elem {
    struct simple_list_elem *next;
};

void *__simple_list_add_tail(struct simple_list_elem **head, void *mem)
{
    struct simple_list_elem *_head = *head;

    /* check out-of-memory condition */
    if (mem == NULL) {
        return NULL;
    }

    if (!_head) {
        *head = mem;
        return *head;
    }

    while (_head->next) {
        _head = _head->next;
    }

    _head = _head->next = mem;
    return _head;
}

void *__simple_list_add_head(struct simple_list_elem **head, void *mem)
{
    struct simple_list_elem *_head = *head;

    /* check out-of-memory condition */
    if (mem == NULL) {
        return NULL;
    }

    *head = mem;
    (*head)->next = _head;

    return *head;
}

void *__simple_list_add_before(struct simple_list_elem **head, void *mem, int needle, int offset)
{
    struct simple_list_elem *_head = *head;
    struct simple_list_elem *prev = 0;

    /* check out-of-memory condition */
    if (mem == NULL) {
        return NULL;
    }

    if (!_head) {
        *head = mem;
        return *head;
    }

    while (_head) {
        int *buff = (void *) _head + offset;

        if (*buff > needle) {
            if (prev) {
                prev->next = mem;
                prev->next->next = _head;
                return prev->next;
            }

            prev = mem;
            prev->next = _head;
            *head = prev;
            return prev;
        }

        prev = _head;
        _head = _head->next;
    }

    _head = prev->next = mem;
    return _head;
}

void *__simple_list_find(struct simple_list_elem *head, void *needle, int offset, size_t size)
{
    while (head) {
        void **buff = (void *) head + offset;

        if (size == 0 && *buff == needle) {
            return head;
        }

        if (size > 0 && memcmp(*buff, needle, size) == 0) {
            return head;
        }

        head = head->next;
    }

    return 0;
}

void *__simple_list_find_cmp(struct simple_list_elem *head, void *needle, int offset, int compare(void *, void *))
{
    while (head) {
        void **buff = (void *) head + offset;

        if (compare(*buff, needle) == 0) {
            return head;
        }

        head = head->next;
    }

    return 0;
}

void *__simple_list_remove(struct simple_list_elem **head, struct simple_list_elem *node, int keep)
{
    struct simple_list_elem *_head = *head;
    struct simple_list_elem *prev = 0;

    while (_head && _head != node) {
        prev = _head;
        _head = _head->next;
    }

    /* not found */
    if (_head != node) {
        return NULL;
    }

    /* remove head */
    if (!prev) {
        *head = _head->next;
    }
    else {
        prev->next = node->next;
    }

    if (keep) {
        return node;
    }

    free(node);
    return (void *) 1;
}

void __simple_list_clear(struct simple_list_elem **head)
{
    struct simple_list_elem *tmp, *_head = *head;

    while (_head != NULL) {
        tmp = _head;
        _head = _head->next;
        free(tmp);
    }

    *head = NULL;
}
