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
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef TCB_H_
#define TCB_H_

#include <stdint.h>
#include "queue.h"
#include "clist.h"
#include "cib.h"
#include "msg.h"

/** The runlevels have a running number, starting at 0, but they are shifted 1 bit
 *  as they also code whether the thread is on the run queue. */
#define STATUS_RUNLEVEL_SHIFT   (1)
/** invalid status, may be used as return value to signal an error */
#define STATUS_NOT_FOUND        (0x0000)
/** scheduled to run */
#define STATUS_ON_RUNQUEUE      (0x0001)
/** currently running */
#define STATUS_RUNNING         ((0x0000 << STATUS_RUNLEVEL_SHIFT) | STATUS_ON_RUNQUEUE)
/** waiting to be scheduled to run */
#define STATUS_PENDING         ((0x0001 << STATUS_RUNLEVEL_SHIFT) | STATUS_ON_RUNQUEUE)
/** has terminated */
#define STATUS_STOPPED          (0x0002 << STATUS_RUNLEVEL_SHIFT)
/** sleeping */
#define STATUS_SLEEPING         (0x0003 << STATUS_RUNLEVEL_SHIFT)
/** waiting for a locked mutex */
#define STATUS_MUTEX_BLOCKED    (0x0004 << STATUS_RUNLEVEL_SHIFT)
/** waiting for a message */
#define STATUS_RECEIVE_BLOCKED  (0x0005 << STATUS_RUNLEVEL_SHIFT)
/** waiting for message to be delivered */
#define STATUS_SEND_BLOCKED     (0x0006 << STATUS_RUNLEVEL_SHIFT)
/** waiting for a message response */
#define STATUS_REPLY_BLOCKED    (0x0007 << STATUS_RUNLEVEL_SHIFT)
/**< waiting for a timer to fire (deprecated) */
#define STATUS_TIMER_WAITING    (0x0008 << STATUS_RUNLEVEL_SHIFT)
/** The bits that determine the runlevel. Use thread_runlevel() to read and
 *  thread_runlevel_set() to write. */
#define STATUS_RUNLEVEL_MASK   ((0x0010 << STATUS_RUNLEVEL_SHIFT) - 1)

typedef struct tcb_t {
    char *sp;
    uint16_t status;

    uint16_t pid;
    uint16_t priority;

    clist_node_t rq_entry;

    void *wait_data;
    queue_node_t msg_waiters;

    cib_t msg_queue;
    msg_t *msg_array;

    const char *name;
    char *stack_start;
    int stack_size;
} tcb_t;

/**
 * @brief   returns the runlevel of a process.
 */
static inline unsigned thread_runlevel(const struct tcb_t *tcb)
{
    return tcb->status & STATUS_RUNLEVEL_MASK;
}

/**
 * @brief   sets the runlevel of a process.
 */
static inline void thread_runlevel_set(struct tcb_t *tcb, unsigned new_runlevel)
{
    tcb->status = (tcb->status & ~STATUS_RUNLEVEL_MASK) | new_runlevel;
}

/** @} */
#endif /* TCB_H_ */
