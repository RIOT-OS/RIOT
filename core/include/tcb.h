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
#define STATUS_NOT_FOUND        (0x0000)
#define STATUS_ON_RUNQUEUE      (0x0001)
#define STATUS_RUNNING          (0x0002 + STATUS_ON_RUNQUEUE)
#define STATUS_PENDING          (0x0004 + STATUS_ON_RUNQUEUE)
#define STATUS_STOPPED          (0x0008)
#define STATUS_SLEEPING         (0x0010)
#define STATUS_MUTEX_BLOCKED    (0x0020)
#define STATUS_RECEIVE_BLOCKED  (0x0040)
#define STATUS_SEND_BLOCKED     (0x0080)
#define STATUS_REPLY_BLOCKED    (0x0100)
#define STATUS_TIMER_WAITING    (0x0200)

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

    char *local_mem;

    const char *name;
    char *stack_start;
    int stack_size;
} tcb_t;

/** @} */
#endif /* TCB_H_ */
