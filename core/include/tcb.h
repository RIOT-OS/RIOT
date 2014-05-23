/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_thread
 * @{
 *
 * @file        tcb.h
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

#ifdef MODULE_MSG_QUEUE
#include "msg_queue.h"
#endif

/**
 * @brief Thread status list
 * @{
 */
/**
 * @brief Blocked states.
 * @{
 */
#define STATUS_STOPPED          0               /**< has terminated                     */
#define STATUS_SLEEPING         1               /**< sleeping                           */
#define STATUS_MUTEX_BLOCKED    2               /**< waiting for a locked mutex         */
#define STATUS_RECEIVE_BLOCKED  3               /**< waiting for a message              */
#define STATUS_SEND_BLOCKED     4               /**< waiting for message to be delivered*/
#define STATUS_REPLY_BLOCKED    5               /**< waiting for a message response     */
/** @} */

/**
 * @brief These have to be on a run queue.
 * @{*/
#define STATUS_ON_RUNQUEUE      STATUS_RUNNING  /**< to check if on run queue:
                                                 `st >= STATUS_ON_RUNQUEUE`             */
#define STATUS_RUNNING          6               /**< currently running                  */
#define STATUS_PENDING          7               /**< waiting to be scheduled to run     */
/** @} */
/** @} */

/**
 * @brief @c tcb_t holds thread's context data.
 */
typedef struct tcb_t {
    char *sp;                   /**< thread's stack pointer         */
    uint16_t status;            /**< thread's status                */

    kernel_pid_t pid;           /**< thread's process id            */
    uint16_t priority;          /**< thread's priority              */

    clist_node_t rq_entry;      /**< run queue entry                */

    void *wait_data;            /**< holding messages               */
    priority_queue_t msg_waiters;   /**< threads waiting on message     */

#ifdef MODULE_MSG_QUEUE
    msg_queue_t *msg_queue;     /**< ptr to thread's msg queue      */
#endif

    const char *name;           /**< thread's name                  */
    char *stack_start;          /**< thread's stack start address   */

#ifdef DEVELHELP
    int stack_size;             /**< thread's stack size            */
#endif
} tcb_t;

#endif /* TCB_H_ */
/** @} */
