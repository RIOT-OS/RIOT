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
    printf("\t%p %s size: %u (%p)\n", ptr, used ? "used" : "free", (unsigned int)size, ptr);

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
