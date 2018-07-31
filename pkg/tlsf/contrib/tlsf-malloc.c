/*
 * Copyright (C) 2014-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup  pkg_tlsf_malloc
 * @ingroup  pkg
 * @ingroup  sys
 * @{
 * @file
 *
 * @brief   TLSF-based global memory allocator.
 * @author  René Kijewski
 * @author  Juan I Carrano
 *
 */

#include <stdio.h>
#include <string.h>

#include "irq.h"
#include "tlsf.h"
#include "tlsf-malloc.h"

/**
 * Global memory heap (really a collection of pools, or areas)
 **/
static tlsf_t gheap = NULL;

/* TODO: Add defines for other compilers */
#if defined(__GNUC__) && !defined(__clang__)    /* Clang supports __GNUC__ but
                                                 * not the alloc_size()
                                                 * attribute */

#define ATTR_MALLOC  __attribute__((malloc, alloc_size(1)))
#define ATTR_CALLOC  __attribute__((malloc, alloc_size(1,2)))
#define ATTR_MALIGN  __attribute__((alloc_align(1), alloc_size(2), malloc))
#define ATTR_REALLOC  __attribute__((alloc_size(2)))

#else /* No GNU C -> no alias attribute */

#define ATTR_MALLOC
#define ATTR_CALLOC
#define ATTR_MALIGN
#define ATTR_REALLOC

#endif /* __GNUC__ */

int tlsf_add_global_pool(void *mem, size_t bytes)
{
    if (gheap == NULL) {
        gheap = tlsf_create_with_pool(mem, bytes);
        return gheap == NULL;
    }
    else {
        return tlsf_add_pool(gheap, mem, bytes) == NULL;
    }
}

tlsf_t *_tlsf_get_global_control(void)
{
    return gheap;
}

void tlsf_size_walker(void* ptr, size_t size, int used, void* user)
{
    printf("\t%p %s size: %u (%p)\n", ptr, used ? "used" : "free", (unsigned int)size, ptr);

    if (used) {
        ((tlsf_size_container_t *)user)->used += (unsigned int)size;
    }
    else {
        ((tlsf_size_container_t *)user)->free += (unsigned int)size;
    }
}

/**
 * Allocate a block of size "bytes"
 */
ATTR_MALLOC void *malloc(size_t bytes)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_malloc(gheap, bytes);

    irq_restore(old_state);
    return result;
}

/**
 * Allocate and clear a block of size "bytes*count"
 */
ATTR_CALLOC void *calloc(size_t count, size_t bytes)
{
    void *result = malloc(count * bytes);

    if (result) {
        memset(result, 0, count * bytes);
    }
    return result;
}

/**
 * Allocate an aligned memory block.
 */
ATTR_MALIGN void *memalign(size_t align, size_t bytes)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_memalign(gheap, align, bytes);

    irq_restore(old_state);
    return result;
}

/**
 * Deallocate and reallocate with a different size.
 */
ATTR_REALLOC void *realloc(void *ptr, size_t size)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_realloc(gheap, ptr, size);

    irq_restore(old_state);
    return result;
}


/**
 * Deallocate a block of data.
 */
void free(void *ptr)
{
    unsigned old_state = irq_disable();

    tlsf_free(gheap, ptr);
    irq_restore(old_state);
}

/**
 * @}
 */
