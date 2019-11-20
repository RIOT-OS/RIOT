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
#include <inttypes.h>

#ifndef CHUNK_SIZE
#ifdef BOARD_NATIVE
#define CHUNK_SIZE          (1024 * 1024U)
#else
#define CHUNK_SIZE          (1024U)
#endif
#endif

#ifndef NUMBER_OF_TESTS
#define NUMBER_OF_TESTS     3
#endif

#ifndef MAX_MEM
#define MAX_MEM             (256 * 1024UL * 1024UL)
#endif

struct node {
    struct node *next;
    void *ptr;
};

static uint32_t total = 0;

static void fill_memory(struct node *head)
{
    uint32_t allocations = 0;

    if (head) {
        head->next = NULL;
    }

    total = 0;
    while (head && (head->ptr = malloc(CHUNK_SIZE)) && total < MAX_MEM) {
        printf("Allocated %"PRIu32" Bytes at 0x%p, total %"PRIu32"\n",
               (uint32_t)CHUNK_SIZE, head->ptr, total += CHUNK_SIZE);
        memset(head->ptr, '@', CHUNK_SIZE);
        head = head->next = malloc(sizeof(struct node));
        if (head) {
            total += sizeof(struct node);
            head->ptr  = 0;
            head->next = 0;
        }
        allocations++;
    }

    printf("Allocations count: %"PRIu32"\n", allocations);
}

static void free_memory(struct node *head)
{
    struct node *old_head;

    while (head) {
        if (head->ptr) {
            if (total > CHUNK_SIZE) {
                total -= CHUNK_SIZE;
            }
            printf("Free %"PRIu32" Bytes at 0x%p, total %"PRIu32"\n",
                   (uint32_t)CHUNK_SIZE, head->ptr, total);
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
    printf("CHUNK_SIZE: %"PRIu32"\n", (uint32_t)CHUNK_SIZE);
    printf("NUMBER_OF_TESTS: %d\n", NUMBER_OF_TESTS);

    uint8_t test = NUMBER_OF_TESTS;
    while (test--) {
        struct node *head = malloc(sizeof(struct node));
        total += sizeof(struct node);

        fill_memory(head);
        free_memory(head);
    }

    puts("[SUCCESS]");

    return 0;
}
