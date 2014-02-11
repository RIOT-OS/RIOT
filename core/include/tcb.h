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

/* uneven means has to be on runqueue */
#define STATUS_NOT_FOUND        (0x0000)                      /**< invalid status, may be used as
                                                                *  return value to signal an error
                                                                */
#define STATUS_ON_RUNQUEUE      (0x0001)                      /**< scheduled to run */
#define STATUS_RUNNING          (0x0002 + STATUS_ON_RUNQUEUE) /**< currently running */
#define STATUS_PENDING          (0x0004 + STATUS_ON_RUNQUEUE) /**< waiting to be scheduled to run */
#define STATUS_STOPPED          (0x0008)                      /**< has terminated */
#define STATUS_SLEEPING         (0x0010)                      /**< sleeping */
#define STATUS_MUTEX_BLOCKED    (0x0020)                      /**< waiting for a locked mutex */
#define STATUS_RECEIVE_BLOCKED  (0x0040)                      /**< waiting for a message */
#define STATUS_SEND_BLOCKED     (0x0080)                      /**< waiting for message to be
                                                                *  delivered */
#define STATUS_REPLY_BLOCKED    (0x0100)                      /**< waiting for a message response */
#define STATUS_TIMER_WAITING    (0x0200)                      /**< waiting for a timer to fire
                                                                *  (deprecated) */

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
