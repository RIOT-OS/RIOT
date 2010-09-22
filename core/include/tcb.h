/**
 * @ingroup kernel
 * @{
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

/*
 * tcb.h
 *
 *  Created on: 19.08.2008
 *      Author: heiko, kaspar
 */

#ifndef TCB_H_
#define TCB_H_

#include <stdint.h>
#include "queue.h"
#include "clist.h"


/* uneven means has to be on runqueue */
#define STATUS_NOT_FOUND 0
#define STATUS_ON_RUNQUEUE 1
#define STATUS_RUNNING 2 +  STATUS_ON_RUNQUEUE
#define STATUS_PENDING 4 +  STATUS_ON_RUNQUEUE
#define STATUS_STOPPED 8
#define STATUS_SLEEPING 16
#define STATUS_MUTEX_BLOCKED 32
#define STATUS_RECEIVE_BLOCKED 64
#define STATUS_SEND_BLOCKED 128
#define STATUS_REPLY_BLOCKED 256

typedef struct tcb {
    char* sp;
    unsigned int status;

    uint16_t pid;
    uint16_t priority;

    void* wait_data;
    queue_node_t msg_queue;

    clist_node_t rq_entry;

    const char* name;
    char* stack_start;
    int stack_size;
} tcb;

/** @} */
#endif /* TCB_H_ */
