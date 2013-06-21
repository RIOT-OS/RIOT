/**
 * @ingroup kernel
 * @{
 * @file        sched.h
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stddef.h>
#include <bitarithm.h>
#include "tcb.h"

#define MAXTHREADS 32

#if ARCH_32_BIT
#define SCHED_PRIO_LEVELS 32
#else
#define SCHED_PRIO_LEVELS 16
#endif

void sched_init(void);
void sched_run(void);

void sched_set_status(tcb_t *process, unsigned int status);
void sched_switch(uint16_t current_prio, uint16_t other_prio, int in_isr);
void cpu_switch_context_exit(void);

extern volatile unsigned int sched_context_switch_request;

extern volatile tcb_t *sched_threads[MAXTHREADS];
extern volatile tcb_t *active_thread;

extern volatile int num_tasks;
extern volatile int thread_pid;

//#define SCHEDSTATISTICS
#if SCHEDSTATISTICS

typedef struct {
    unsigned int laststart;
    unsigned int schedules;
    unsigned int runtime;
} schedstat;

extern schedstat pidlist[MAXTHREADS];
#endif

/** @} */
#endif // _SCHEDULER_H
