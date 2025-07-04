/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    core_thread_flags_group Thread Flags Group
 * @ingroup     core
 * @brief       Waiter groups for thread flags
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
 *    static thread_flags_group_t group = THREAD_FLAGS_GROUP_INIT;
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

#include <limits.h>

#include "atomic_utils.h"
#include "thread.h"
#include "thread_flags.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UINT_WIDTH is only provided starting with -std=c23 or newer. Until RIOT
 * requires C23 as C version, we need provide it by hand when missing. */
#ifndef UINT_WIDTH
/**
 * @brief Number of bits in unsigned int
 */
#  define UINT_WIDTH (sizeof(unsigned) * 8)
#endif

/**
 * @brief Thread flags group.
 */
typedef struct {
    /**
     * Members bitfield.
     */
    unsigned members[(MAXTHREADS / UINT_WIDTH) + !!(MAXTHREADS % UINT_WIDTH)];
} thread_flags_group_t;

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
 * @param[out] group The thread flags group to join.
 */
static inline void thread_flags_group_join(thread_flags_group_t *group)
{
    kernel_pid_t pid = thread_getpid();
    /* this also optimizes away the arithmetic below if MAXTHREADS <= UINT_WIDTH */
    assume(pid < MAXTHREADS);
    atomic_set_bit_unsigned(atomic_bit_unsigned(&group->members[pid / UINT_WIDTH],
                                                pid % UINT_WIDTH));
}

/**
 * @brief Leave a thread flags group.
 *
 * After leaving the group, the thread will no longer be signaled by @ref
 * thread_flags_group_set().
 *
 * @param[out] group    The thread flags group to leave.
 */
static inline void thread_flags_group_leave(thread_flags_group_t *group)
{
    kernel_pid_t pid = thread_getpid();
    /* this also optimizes away the arithmetic below if MAXTHREADS <= UINT_WIDTH */
    assume(pid < MAXTHREADS);
    atomic_clear_bit_unsigned(atomic_bit_unsigned(&group->members[pid / UINT_WIDTH],
                                                  pid % UINT_WIDTH));
}

/**
 * @brief Set thread flags for all threads in a group.
 *
 * @param[in] group The thread flags group to set flags to.
 * @param[in] mask  The flags to set.
 */
void thread_flags_group_set(thread_flags_group_t *group, thread_flags_t mask);

#ifdef __cplusplus
}
#endif
/** @} */
