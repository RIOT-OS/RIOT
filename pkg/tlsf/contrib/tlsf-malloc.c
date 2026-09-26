/*
 * SPDX-FileCopyrightText: 2014-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup  pkg_tlsf_malloc
 * @ingroup  pkg
 * @ingroup  sys
 * @{
 * @file
 *
 * @brief   TLSF-based global memory allocator.
 * @author  Juan I Carrano
 *
 */

#include <stdio.h>

#include "tlsf.h"
#include "tlsf-malloc.h"
#include "tlsf-malloc-internal.h"

/**
 * Global memory heap (really a collection of pools, or areas)
 **/
tlsf_t tlsf_malloc_gheap = NULL;

int tlsf_add_global_pool(void *mem, size_t bytes)
{
    if (tlsf_malloc_gheap == NULL) {
        tlsf_malloc_gheap = tlsf_create_with_pool(mem, bytes);
        return tlsf_malloc_gheap == NULL;
    }
    else {
        return tlsf_add_pool(tlsf_malloc_gheap, mem, bytes) == NULL;
    }
}

tlsf_t _tlsf_get_global_control(void)
{
    return tlsf_malloc_gheap;
}

void tlsf_size_walker(void* ptr, size_t size, int used, void* user)
{
    printf("\t%p %s size: %" PRIuSIZE " (%p)\n", ptr, used ? "used" : "free",
           size, ptr);

    if (used) {
        ((tlsf_size_container_t *)user)->used += (unsigned int)size;
    }
    else {
        ((tlsf_size_container_t *)user)->free += (unsigned int)size;
    }
}

/**
 * @}
 */
