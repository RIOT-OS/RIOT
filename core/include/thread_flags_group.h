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
 *    tfg_entry_t entry;
 *    thread_flags_group_join(&group, &entry);
 *
 *    while (!some_condition_is_met()) {
 *        thread_flags_wait_any(SOME_FLAG);
 *    }
 *
 *    thread_flags_group_leave(&group, &entry);
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

#include "thread_flags.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Thread flags group entry.
 */
typedef struct {
    list_node_t node;   /**< linked list node */
    thread_t *thread;   /**< thread joining the group */
} tfg_entry_t;

/**
 * @brief Thread flags group.
 */
typedef struct {
    list_node_t list;   /**< linked list head */
} tfg_t;

/**
 * @brief Initialize a thread flags group.
 */
#define THREAD_FLAGS_GROUP_INIT { 0 }

/**
 * @brief Join a thread flags group.
 *
 * After joining the group, the thread may call any thread_flags_wait_*()
 * method as usual. The thread will remain in the group until @ref
 * thread_flags_group_leave() is called.
 *
 * @param group     The thread flags group to join.
 * @param entry     Thread flags group entry.
 */
void thread_flags_group_join(tfg_t *group, tfg_entry_t *entry);

/**
 * @brief Leave a thread flags group.
 *
 * After leaving the group, the thread will no longer be signaled by @ref
 * thread_flags_group_set().
 *
 * @param group     The thread flags group to leave.
 * @param entry     Thread flags group entry.
 */
void thread_flags_group_leave(tfg_t *group, tfg_entry_t *entry);

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
