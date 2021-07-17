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
 * # About initialization
 *
 * Some system (standard library) routines trigger the allocation of buffers
 * which results in segmentation faults/ hard faults if using tlsf and the
 * heap is not yet allocated. This function can possibly be used super early
 * in the boot process, so the TLSF initialization must run earlier still.
 *
 * On native we define a static array and on embedded platforms we use the
 * _sheap and _eheap linker symbols and sbrk.
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

/* The C library runs the functions in this array before it initializes itself.
 * This section constitutes a cross-file array.
 * Note that in order for this to work this object file has to be added to
 * UNDEF (see ../Makefile.include) otherwise this symbol gets discarded.
 */
void (* const init_tlsf_malloc_p) (void) __attribute__((section (".preinit_array"))) =
    init_tlsf_malloc;

/**
 * @}
 */
