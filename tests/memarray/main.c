/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Simple memarray module tests
 *
 * @author      Tobias Heider <heidert@nm.ifi.lmu.de>
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memarray.h"

#define MESSAGE_SIZE         (8U)

#ifndef MAX_NUMBER_BLOCKS
#define MAX_NUMBER_BLOCKS    (10)
#endif

#ifndef NUMBER_OF_TESTS
#define NUMBER_OF_TESTS      (1)
#endif

#ifndef NUMBER_OF_LOOPS
#define NUMBER_OF_LOOPS      (1)
#endif


extern int _ps_handler(int argc, char **argv);

struct block_t {
    struct node *next;
    int number;
    /* static size for the components */
    unsigned char message[MESSAGE_SIZE];
};

struct block_t block_storage_data[MAX_NUMBER_BLOCKS];
struct block_t block_storage_data_extend[MAX_NUMBER_BLOCKS];
memarray_t block_storage;

int total = 0;

static void memory_block_init(void)
{
    memarray_init(&block_storage, block_storage_data, sizeof(struct block_t), MAX_NUMBER_BLOCKS);
}

void fill_memory(struct block_t *head)
{
    int aux = 0;

    while ((aux < MAX_NUMBER_BLOCKS) && (head)) {
        memset(head->message, '@', MESSAGE_SIZE - 1);
        head->message[MESSAGE_SIZE - 1] = 0;
        head->number = aux;

        printf("\t(%i, %s) Allocated %u Bytes at %p, total %d\n",
               head->number, head->message, (unsigned)sizeof(struct block_t),
               (void *)head, total);

        /* NOTE: If there is not space, memarray_alloc returns zero */
        head->next = memarray_alloc(&block_storage);
        head = (struct block_t *)head->next;

        total += sizeof(struct block_t);
        aux++;
    }
}

void free_memory(struct block_t *head)
{
    struct block_t *old;

    while (head) {
        total -= sizeof(struct block_t);
        printf("\tFree (%i) %u Bytes at %p, total %d\n", \
               head->number, (unsigned)sizeof(struct block_t),
               (void *)head, total);

        if (head->next) {
            old = head;
            head = (struct block_t *) head->next;
            memarray_free(&block_storage, old);
        }
        else {
            memarray_free(&block_storage, head);
            head = 0;
        }
    }
}

int main(void)
{
    printf("MAX_NUMBER_BLOCKS: %d\n", MAX_NUMBER_BLOCKS);
    printf("NUMBER_OF_LOOPS: %d\n", NUMBER_OF_LOOPS);
    printf("NUMBER_OF_TESTS: %d\n", NUMBER_OF_TESTS);

    memory_block_init();
    int count = 0;
    int loop = 0;

    printf("Starting (%d, %u)\n", MAX_NUMBER_BLOCKS, MESSAGE_SIZE);
    _ps_handler(0, NULL);

    printf("LOOP #%i:\n", loop + 1);
    while (count <  NUMBER_OF_TESTS) {
        struct block_t *head = (struct block_t *) memarray_alloc(&block_storage);

        printf("TEST #%i:\n", count + 1 );
        fill_memory(head);
        free_memory(head);

        count++;
    }

    count = 0;
    loop++;
    printf("LOOP #%i:\n", loop + 1);
    while (count < NUMBER_OF_TESTS) {
        memory_block_init();
        struct block_t *head = (struct block_t *) memarray_alloc(&block_storage);

        printf("TEST #%i:\n", count + 1 );
        fill_memory(head);
        free_memory(head);

        count++;
    }

    puts("Extend and reduce tests");

    printf("Memarray available: %u\n",
           (unsigned)memarray_available(&block_storage));

    /* Extend with second block */
    memarray_extend(&block_storage, block_storage_data_extend,
                    MAX_NUMBER_BLOCKS);
    printf("Memarray available: %u\n",
           (unsigned)memarray_available(&block_storage));

    /* remove the original block */
    int res = memarray_reduce(&block_storage, block_storage_data,
                              MAX_NUMBER_BLOCKS);
    printf("Memarray reduction: %d available: %u\n",
           res, (unsigned)memarray_available(&block_storage));

    /* try to remove original block a second time */
    res = memarray_reduce(&block_storage, block_storage_data,
                          MAX_NUMBER_BLOCKS);
    printf("Memarray reduction: %d available: %u\n",
           res, (unsigned)memarray_available(&block_storage));

    /* remove the extension block */
    res = memarray_reduce(&block_storage, block_storage_data_extend,
                          MAX_NUMBER_BLOCKS);
    printf("Memarray reduction: %d available: %u\n",
           res, (unsigned)memarray_available(&block_storage));

    /* extend again with the original block */
    memarray_extend(&block_storage, block_storage_data, MAX_NUMBER_BLOCKS);

    /* remove one element */
    memarray_alloc(&block_storage);
    printf("Memarray available: %u\n",
           (unsigned)memarray_available(&block_storage));

    /* try to reduce with a missing element */
    res = memarray_reduce(&block_storage, block_storage_data, MAX_NUMBER_BLOCKS);
    printf("Memarray reduction: %d available: %u\n",
           res, (unsigned)memarray_available(&block_storage));

    printf("Finishing\n");
    _ps_handler(0, NULL);

    return 0;
}
