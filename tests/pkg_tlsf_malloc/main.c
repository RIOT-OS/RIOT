/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       TLSF initialization tests.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * Verify that TLSF is correctly and automatically initialized when used as
 * the default global allocator. Failure to do so will result in segfaults /
 * hardfaults, even if the application does not use malloc (this is because
 * the C library does buffer allocations internally).
 *
 * This is currently failing as the initialization is not being done.
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tlsf.h"
#include "tlsf-malloc.h"

extern char _eheap[];

#define BLOCKSIZE 42

static void walker(void* ptr, size_t size, int used, void* _accumulator)
{
	size_t *accumulator = _accumulator;
    *accumulator += size;
	printf("\t%p %s size: %u\n", ptr, used ? "used" : "free", (unsigned int)size);
}

int main(void)
{
    size_t accumulator = 0;
    void *some_block;
    size_t requested_size = 0;

    printf("Using TLSF-malloc, global pool at %p\n", (void*)_tlsf_get_global_control());

    #ifndef BOARD_NATIVE
        requested_size = _eheap - (char*)_tlsf_get_global_control();
    #else
        requested_size = TLSF_NATIVE_HEAPSIZE;
    #endif

    /* increase the chances of a crash */
    some_block = calloc(1, BLOCKSIZE);

    assert(some_block != NULL);

    puts("TLSF heap report:");

    tlsf_walk_pool(tlsf_get_pool(_tlsf_get_global_control()), walker, &accumulator);

    printf("According to TLSF heap has %u bytes\n", accumulator);
    printf("The size should be %u bytes.\n", requested_size);

    printf("Overhead: %d bytes\n", requested_size-accumulator);

    free(some_block);

    return 0;
}
