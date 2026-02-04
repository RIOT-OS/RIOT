/*
 * Copyright (C) 2014-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup pkg_tlsf_malloc TLSF-based malloc.
 * @ingroup  pkg
 * @ingroup  sys_memory_management
 *
 * @brief    TLSF-based global memory allocator.
 *
 * This is a malloc/free implementation built on top of the TLSF allocator.
 * It defines a global tlsf_control block and performs allocations on that
 * block. This implementation replaces the system malloc
 *
 * Additionally, the calls to TLSF are wrapped in irq_disable()/irq_restore(),
 * to make it thread-safe.
 *
 * If this module is used as the system memory allocator, then the global memory
 * control block should be initialized as the first thing before the stdlib is
 * used. Boards should use tlsf_add_global_pool() at startup to add all the memory
 * regions they want to make available for dynamic allocation via malloc().
 *
 * @{
 * @file
 *
 * @brief   TLSF-based global memory allocator.
 * @author  René Kijewski
 * @author  Juan I Carrano
 *
 */

#include <stddef.h>

#include "tlsf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct to hold the total sizes of free and used blocks
 * Used for @ref tlsf_size_walker()
 */
typedef struct {
    unsigned free;          /**< total free size */
    unsigned used;          /**< total used size */
} tlsf_size_container_t;

/**
 * Walk the memory pool to print all block sizes and to calculate
 * the total amount of free and used block sizes.
 *
 * @note This function is passed to tlsf_walk_pool()
 *
 * @param   ptr        Pointer to the current block.
 * @param   size       Size of the current block at @p ptr.
 * @param   used       Shows whether the current block is used or free.
 * @param   user       Custom data expected to be of type ``pointer to tlsf_size_container_t``
 */
void tlsf_size_walker(void* ptr, size_t size, int used, void* user);

/**
 * Add an area of memory to the global allocator pool.
 *
 * The first time this function is called, it will automatically perform a
 * tlsf_create() on the global tlsf_control block.
 *
 * @warning If this module is used, then this function MUST be called at least
 *          once, before any allocations take place.
 *
 * @param   mem        Pointer to memory area. Should be aligned to 4 bytes.
 * @param   bytes      Size in bytes of the memory area.
 *
 * @return  0 on success, nonzero on failure.
 */
int tlsf_add_global_pool(void *mem, size_t bytes);

/**
 * Get a pointer to the global tlsf_control block.
 *
 * Use for debugging purposes only.
 */
tlsf_t _tlsf_get_global_control(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
