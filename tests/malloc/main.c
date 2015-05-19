/*
 * Copyright (C) 2013 Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Simple malloc/free test
 *
 *
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 1024

struct node {
    struct node *next;
    void *ptr;
};

int total = 0;

void fill_memory(struct node *head)
{
    while (head && (head->ptr = malloc(CHUNK_SIZE))) {
        printf("Allocated %d Bytes at 0x%p, total %d\n", CHUNK_SIZE, head->ptr, total += CHUNK_SIZE);
        memset(head->ptr, '@', CHUNK_SIZE);
        head = head->next = malloc(sizeof(struct node));
        head->ptr =  0;
        head->next = 0;
        total += sizeof(struct node);
    }
}

void free_memory(struct node *head)
{
    struct node *old_head;

    while (head) {
        if (head->ptr) {
            printf("Free %d Bytes at 0x%p, total %d\n", CHUNK_SIZE, head->ptr, total -= CHUNK_SIZE);
            free(head->ptr);
        }

        if (head->next) {
            old_head = head;
            head = head->next;
            free(old_head);
        }
        else {
            free(head);
            head = 0;
        }

        total -= sizeof(struct node);
    }
}

int main(void)
{
    while (1) {
        struct node *head = malloc(sizeof(struct node));
        total += sizeof(struct node);

        fill_memory(head);
        free_memory(head);
    }

    return 0;
}
