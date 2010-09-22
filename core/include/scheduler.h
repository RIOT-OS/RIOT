/**
 * @ingroup kernel
 * @{
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stddef.h>
#include <bitarithm.h>
#include "tcb.h"

#define MAXTHREADS 32

#ifdef ARCH_32_BIT
#define SCHED_PRIO_LEVELS 32
#else
#define SCHED_PRIO_LEVELS 16
#endif

void scheduler_init();
void fk_schedule();

void sched_set_status(tcb *process, unsigned int status);

volatile unsigned int fk_context_switch_request;

volatile tcb *fk_threads[MAXTHREADS];
volatile tcb *fk_thread;

extern volatile int num_tasks;
volatile int fk_pid;

//#define SCHEDSTATISTICS
#if SCHEDSTATISTICS

    typedef struct schedstat {
        unsigned int laststart;
        unsigned int schedules;
        unsigned int runtime;
    }schedstat;

    extern schedstat pidlist[MAXTHREADS];
#endif

/** @} */
#endif // _SCHEDULER_H
