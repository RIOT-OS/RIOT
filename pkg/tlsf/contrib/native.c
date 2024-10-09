/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief   Definitions to use tlsf as malloc on native.
 * @author  Juan I Carrano
 *
 * This assumes glibc is being used.
 * see: https://www.gnu.org/software/libc/manual/html_node/Replacing-malloc.html
 *
 */

#include <string.h>
#include <errno.h>

#include "irq.h"
#include "tlsf.h"
#include "tlsf-malloc.h"
#include "tlsf-malloc-internal.h"

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

extern tlsf_t tlsf_malloc_gheap;

/**
 * Allocate a block of size "bytes"
 */
ATTR_MALLOC void *malloc(size_t bytes)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_malloc(tlsf_malloc_gheap, bytes);

    if (result == NULL) {
        errno = ENOMEM;
    }

    irq_restore(old_state);
    return result;
}

/**
 * Allocate and clear a block of size "bytes*count"
 */
ATTR_CALLOC void *calloc(size_t count, size_t bytes)
{
    void *result = malloc(count * bytes);

    if (result != NULL) {
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
    void *result = tlsf_memalign(tlsf_malloc_gheap, align, bytes);

    if (result == NULL) {
        errno = ENOMEM;
    }

    irq_restore(old_state);
    return result;
}

/**
 * Deallocate and reallocate with a different size.
 */
ATTR_REALLOC void *realloc(void *ptr, size_t size)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_realloc(tlsf_malloc_gheap, ptr, size);

    if (result == NULL) {
        errno = ENOMEM;
    }

    irq_restore(old_state);
    return result;
}

/**
 * Deallocate a block of data.
 */
void free(void *ptr)
{
    unsigned old_state = irq_disable();

    tlsf_free(tlsf_malloc_gheap, ptr);
    irq_restore(old_state);
}
