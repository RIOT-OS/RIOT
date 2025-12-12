/*
 * SPDX-FileCopyrightText: 2013 Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "buildinfo/cpu.h"
#include "architecture.h"
#include "test_utils/expect.h"

#ifndef CHUNK_SIZE
#ifdef CPU_NATIVE
#define CHUNK_SIZE          (1024 * 1024U)
#else
#define CHUNK_SIZE          (128U)
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

static uint32_t fill_memory(struct node *head)
{
    uint32_t allocations = 0;

    if (head) {
        head->next = NULL;
    }

    total = 0;
    while (head && (head->ptr = malloc(CHUNK_SIZE)) && total < MAX_MEM) {
        printf("Allocated %"PRIu32" Bytes at %p, total %"PRIu32"\n",
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

    return allocations;
}

static void free_memory(struct node *head)
{
    struct node *old_head;

    uint32_t freed = 0;

    while (head) {
        if (head->ptr) {
            if (total >= CHUNK_SIZE) {
                total -= CHUNK_SIZE;
                freed++;
            }
            printf("Free %"PRIu32" Bytes at %p, total %"PRIu32"\n",
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

    printf("Free count: %"PRIu32"\n", freed);
}

static void check_calloc_returns_null_on_overflow(void)
{
    /* modern compilers warn about nonsense calls to calloc, but this is exactly what we want to
     * test */
#pragma GCC diagnostic push
#if !defined(__clang__) && (__GNUC__ > 6)
#pragma GCC diagnostic ignored "-Walloc-size-larger-than="
#endif
    /* test if an overflow is correctly detected by calloc(): the size below overflows by 1 byte */
    /* cppcheck-suppress leakReturnValNotUsed; (should return NULL, so nothing to free anyway) */
    size_t nmemb = SIZE_MAX / 16 + 1;
    size_t size = 16;
    void *p = calloc(nmemb, size);

    /* When clang detects that the memory allocated is not actually used, it
     * will optimize out the call to `calloc()` and just assume that the
     * allocation succeeded. It then optimized out the test `NULL == p` and
     * assumes it to always be false. We just print the address to prevent
     * that from happening */
    printf("calloc(%" PRIuSIZE ", %" PRIuSIZE ") = %p\n", nmemb, size, p);
    expect(NULL == p);
#pragma GCC diagnostic pop
}

int main(void)
{
    uint32_t allocations = 0;
    check_calloc_returns_null_on_overflow();

    printf("CHUNK_SIZE: %"PRIu32"\n", (uint32_t)CHUNK_SIZE);
    printf("NUMBER_OF_TESTS: %d\n", NUMBER_OF_TESTS);

    uint8_t test = NUMBER_OF_TESTS;
    while (test--) {
        struct node *head = malloc(sizeof(struct node));
        total += sizeof(struct node);

        uint32_t new_allocations = fill_memory(head);
        free_memory(head);

        /* test if all memory was freed/can be allocated again */
        if (allocations == 0) {
            allocations = new_allocations;
        } else if (allocations != new_allocations) {
            puts("[FAILED]");
            return -1;
        }
    }

    puts("[SUCCESS]");

    return 0;
}
