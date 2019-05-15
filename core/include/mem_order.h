/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_mem_order Low-Level Memory Ordering Primitives
 * @ingroup     sys
 * @brief       Low-Level Memory Ordering Primitives
 *
 * Low-level primitives to enforce order of memory accesses. These are not
 * intended to be used by application code, please use the easier to use high
 * level synchronization mechanisms in @ref core_sync instead.
 *
 * @file
 * @{
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef MEM_ORDER_H
#define MEM_ORDER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Prevent the compiler from moving global memory accesses across this
 *          barrier, but **NOT** prevent the CPU from doing so
 * @see     memory_barrier
 * @see     compiler_barrier_data
 * @warning Application code should use high level synchronization primitives
 *          instead, see @ref core_sync
 *
 * Any read or store to global memory that is written in the code before this
 * function will not be moved after this barrier by the compiler. Likewise,
 * every read or store to global after this barrier will not be moved before
 * this barrier.
 */
#define compiler_barrier() __asm__ volatile("": : :"memory")

/**
 * @brief   Prevent the compiler from moving global memory accesses across this
 *          barrier and accesses to the local memory @p ptr points to, but
 *          do **NOT** prevent the CPU from doing so
 * @see     memory_barrier
 * @see     compiler_barrier
 * @warning Application code should use high level synchronization primitives
 *          instead, see @ref core_sync
 * @param   ptr     Pointer to local memory to also protect by the barrier
 *
 * This is identical to @ref compiler_barrier, but also accesses to the local
 * memory @p ptr points to are protected. In order to include multiple local
 * variables, repeat this macro for each local variable to protect. (You should
 * put **no** code between the repeated calls to @ref compiler_barrier_data in
 * order to pay the performance penalty of the global memory barrier only once.)
 */
#define compiler_barrier_data(ptr) __asm__ volatile("": : "r"(ptr) :"memory")

/**
 * @brief   Prevent both the compiler and the CPU from moving memory accesses
 *          across this barrier
 * @warning Application code should use high level synchronization primitives
 *          instead, see @ref core_sync
 */
static inline void memory_barrier(void)
{
    __sync_synchronize();
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MEM_ORDER_H */
