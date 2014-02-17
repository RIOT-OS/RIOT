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

/* thread status list */
/* blocked states: */
#define STATUS_STOPPED          0                      /**< has terminated */
#define STATUS_SLEEPING         1                      /**< sleeping */
#define STATUS_MUTEX_BLOCKED    2                      /**< waiting for a locked mutex */
#define STATUS_RECEIVE_BLOCKED  3                      /**< waiting for a message */
#define STATUS_SEND_BLOCKED     4                      /**< waiting for message to be
                                                           *  delivered */
#define STATUS_REPLY_BLOCKED    5                      /**< waiting for a message response */
#define STATUS_TIMER_WAITING    6                      /**< waiting for a timer to fire */

#define STATUS_ON_RUNQUEUE      7                      /**< */

/* these have to be on a run queue: */
#define STATUS_RUNNING          7                      /**< currently running */
#define STATUS_PENDING          8                      /**< waiting to be scheduled to run */



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

/** @} */
#endif /* TCB_H_ */
