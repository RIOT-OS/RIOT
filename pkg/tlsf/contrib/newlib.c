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
 * @brief   Reentrant definitions to replace newlib's malloc with TLSF.
 * @author  Juan I Carrano
 *
 * Newlib-nano implements malloc/free/etc in terms of the reentrant definitions
 * in _malloc_r/_free_r/etc so the latter are the one that have to be
 * overwritten.
 *
 */

#include <string.h>
#include <reent.h>
#include <errno.h>

#include "irq.h"
#include "tlsf.h"
#include "tlsf-malloc.h"
#include "tlsf-malloc-internal.h"

/* TODO: Add defines for other compilers */
#if defined(__GNUC__) && !defined(__clang__)    /* Clang supports __GNUC__ but
                                                 * not the alloc_size()
                                                 * attribute */

#define ATTR_MALLOCR  __attribute__((malloc, alloc_size(2)))
#define ATTR_CALLOCR  __attribute__((malloc, alloc_size(2,3)))
#define ATTR_MALIGNR  __attribute__((alloc_align(2), alloc_size(3), malloc))
#define ATTR_REALLOCR  __attribute__((alloc_size(3)))

#else /* No GNU C -> no alias attribute */

#define ATTR_MALLOCR
#define ATTR_CALLOCR
#define ATTR_MALIGNR
#define ATTR_REALLOCR

#endif /* __GNUC__ */

/**
 * Allocate a block of size "bytes"
 */
ATTR_MALLOCR void *_malloc_r(struct _reent *reent_ptr, size_t bytes)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_malloc(tlsf_malloc_gheap, bytes);

    if (result == NULL) {
        reent_ptr->_errno = ENOMEM;
    }

    irq_restore(old_state);
    return result;
}

/**
 * Allocate and clear a block of size "bytes*count"
 */
ATTR_CALLOCR void *_calloc_r(struct _reent *reent_ptr, size_t count, size_t bytes)
{
    size_t size_total;
    if (__builtin_mul_overflow(count, bytes, &size_total)) {
        return NULL;
    }
    void *result = _malloc_r(reent_ptr, size_total);

    if (result != NULL) {
        memset(result, 0, size_total);
    }
    return result;
}

/**
 * Allocate an aligned memory block.
 */
ATTR_MALIGNR void *_memalign_r(struct _reent *reent_ptr, size_t align, size_t bytes)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_memalign(tlsf_malloc_gheap, align, bytes);

    if (result == NULL) {
        reent_ptr->_errno = ENOMEM;
    }

    irq_restore(old_state);
    return result;
}

/**
 * Deallocate and reallocate with a different size.
 */
ATTR_REALLOCR void *_realloc_r(struct _reent *reent_ptr, void *ptr, size_t size)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_realloc(tlsf_malloc_gheap, ptr, size);

    if (result == NULL) {
        reent_ptr->_errno = ENOMEM;
    }

    irq_restore(old_state);
    return result;
}

/**
 * Deallocate a block of data.
 */
void _free_r(struct _reent *reent_ptr, void *ptr)
{
    unsigned old_state = irq_disable();
    (void)reent_ptr;

    tlsf_free(tlsf_malloc_gheap, ptr);
    irq_restore(old_state);
}

/**
 * @}
 */
