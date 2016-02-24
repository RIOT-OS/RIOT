/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and thread_conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_thread
 * @{
 *
 * @file
 * @brief       Thread context block definition
 *
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef TCB_H_
#define TCB_H_

#include <stdint.h>
#include "priority_queue.h"
#include "clist.h"
#include "cib.h"
#include "thread_flags.h"
#include "msg.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Thread status list
 * @{
 */
/**
 * @brief Blocked states.
 * @{
 */
#define STATUS_STOPPED              0   /**< has terminated                     */
#define STATUS_SLEEPING             1   /**< sleeping                           */
#define STATUS_MUTEX_BLOCKED        2   /**< waiting for a locked mutex         */
#define STATUS_RECEIVE_BLOCKED      3   /**< waiting for a message              */
#define STATUS_SEND_BLOCKED         4   /**< waiting for message to be delivered*/
#define STATUS_REPLY_BLOCKED        5   /**< waiting for a message response     */
#define STATUS_FLAG_BLOCKED_ANY     6   /**< waiting for any flag from flag_mask*/
#define STATUS_FLAG_BLOCKED_ALL     7   /**< waiting for all flags in flag_mask */
/** @} */

/**
 * @brief These have to be on a run queue.
 * @{*/
#define STATUS_ON_RUNQUEUE      STATUS_RUNNING  /**< to check if on run queue:
                                                 `st >= STATUS_ON_RUNQUEUE`         */
#define STATUS_RUNNING          9               /**< currently running              */
#define STATUS_PENDING          10              /**< waiting to be scheduled to run */
/** @} */
/** @} */

/**
 * @brief @c tcb_t holds thread's context data.
 */
struct tcb {
    char *sp;                   /**< thread's stack pointer         */
    uint16_t status;            /**< thread's status                */

    kernel_pid_t pid;           /**< thread's process id            */
    uint16_t priority;          /**< thread's priority              */

    thread_flags_t flags;       /**< currently set flags            */

    clist_node_t rq_entry;      /**< run queue entry                */

    void *wait_data;            /**< holding messages               */
    priority_queue_t msg_waiters;   /**< threads waiting on message     */

    cib_t msg_queue;            /**< message queue                  */
    msg_t *msg_array;           /**< memory holding messages        */

#if defined DEVELHELP || defined(SCHED_TEST_STACK)
    char *stack_start;          /**< thread's stack start address   */
#endif
#ifdef DEVELHELP
    const char *name;           /**< thread's name                  */
    int stack_size;             /**< thread's stack size            */
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* TCB_H_ */
/** @} */
