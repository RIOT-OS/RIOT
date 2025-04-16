/*
 * Copyright (C) 2025 Mihai Renea <mihai.renea@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_thread_flags_group Thread Flags Group
 * @ingroup     core
 * @brief       Thread flags
 *
 *
 * This API is optional and must be enabled by adding "core_thread_flags_group"
 * to USEMODULE.
 *
 * A thread flags group allows setting thread flags for an arbitrary number of
 * threads (called waiters) at the same time. The waiters can join and leave
 * the group at any time. An additional advantage is that the signaler (the
 * "flags setter") is de-coupled from the list of waiters, i.e. it does not
 * need to know which specific thread must be signaled.
 *
 * Example (waiter):
 *
 *    static tfg_t group = THREAD_FLAGS_GROUP_INIT;
 *
 *    ...
 *
 *    thread_flags_group_join(&group);
 *
 *    while (!some_condition_is_met()) {
 *        thread_flags_wait_any(SOME_FLAG);
 *    }
 *
 *    thread_flags_group_leave(&group);
 *
 * Example (signaler):
 *
 *    fulfill_some_condition();
 *    thread_flags_group_set(&group, SOME_FLAG);
 *
 * @{
 *
 * @file
 * @brief       Thread Flags Group API
 *
 * @author      Mihai Renea <mihai.renea@ml-pa.com>
 */

#ifndef THREAD_FLAGS_GROUP_H
#define THREAD_FLAGS_GROUP_H

#include "atomic_utils.h"
#include "thread.h"
#include "thread_flags.h"
#include "limits.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @internal
 * @{
 */

/**
 * @name Atomic bit manipulation using native integer size
 *
 * With "native integer size" we mean whatever unsigned int is because this is
 * what the bit manipulation in bitarithm.h operates on.
 *
 * We can't `typedef unsigned tfg_int_t` because some architectures map uint32_t
 * to unsigned long, so passing tfg_int_t to the 32-bit atomic operations below
 * will issue a warning.
 *
 * @{
 */
#if UINT_MAX == UINT32_MAX
/**
 * @brief Integer type for the bitfield.
 */
typedef uint32_t tfg_int_t;
#define ATOMIC_SET_BIT_UINT(bit) atomic_set_bit_u32(bit)        /**< @brief set */
#define ATOMIC_CLEAR_BIT_UINT(bit) atomic_clear_bit_u32(bit)    /**< @brief clear */
#define ATOMIC_BIT_UINT(dest, bit) atomic_bit_u32(dest, bit)    /**< @brief get bit */

#elif UINT_MAX == UINT16_MAX
/**
 * @brief Integer type for the bitfield.
 */
typedef uint16_t tfg_int_t; /**< integer type for the bitfield */
#define ATOMIC_SET_BIT_UINT(bit) atomic_set_bit_u16(bit)        /**< @brief set */
#define ATOMIC_CLEAR_BIT_UINT(bit) atomic_clear_bit_u16(bit)    /**< @brief clear */
#define ATOMIC_BIT_UINT(dest, bit) atomic_bit_u16(dest, bit)    /**< @brief get bit */

#else
/* Technically, 8-bit-sized unsigned int is possible (e.g. by setting AVR's
 * -mint8 flag) but that seems to break RIOT in other places.
 */
static_assert(0, "unsigned size unsupported!");
#endif

#define TFG_INT_BITS (sizeof(tfg_int_t) * 8) /**< Number of bits in tfg_int_t */
/** @} */

/**
 * @internal
 * @}
 */

/**
 * @brief Thread flags group.
 */
typedef struct {
    /**
     * Members bitfield.
     */
    tfg_int_t members[MAXTHREADS / TFG_INT_BITS + !!(MAXTHREADS % TFG_INT_BITS)];
} tfg_t;

/**
 * @brief Initialize a thread flags group.
 */
#define THREAD_FLAGS_GROUP_INIT { .members = { 0 } }

/**
 * @brief Join a thread flags group.
 *
 * After joining the group, the thread may call any thread_flags_wait_*()
 * method as usual. The thread will remain in the group until @ref
 * thread_flags_group_leave() is called.
 *
 * @param group     The thread flags group to join.
 */
static inline void thread_flags_group_join(tfg_t *group)
{
    kernel_pid_t pid = thread_getpid();
    /* this also optimizes away the arithmetic below if MAXTHREADS <= TFG_INT_BITS */
    assume(pid < MAXTHREADS);
    ATOMIC_SET_BIT_UINT(ATOMIC_BIT_UINT(&group->members[pid / TFG_INT_BITS],
                                        pid % TFG_INT_BITS));
}

/**
 * @brief Leave a thread flags group.
 *
 * After leaving the group, the thread will no longer be signaled by @ref
 * thread_flags_group_set().
 *
 * @param group     The thread flags group to leave.
 */
static inline void thread_flags_group_leave(tfg_t *group)
{
    kernel_pid_t pid = thread_getpid();
    /* this also optimizes away the arithmetic below if MAXTHREADS <= TFG_INT_BITS */
    assume(pid < MAXTHREADS);
    ATOMIC_CLEAR_BIT_UINT(ATOMIC_BIT_UINT(&group->members[pid / TFG_INT_BITS],
                                          pid % TFG_INT_BITS));
}

/**
 * @brief Set thread flags for all threads in a group.
 *
 * @param group     The thread flags group to set flags to.
 * @param mask      The flags to set.
 */
void thread_flags_group_set(tfg_t *group, thread_flags_t mask);

#ifdef __cplusplus
}
#endif

#endif /* THREAD_FLAGS_GROUP_H */
/** @} */
