/**
 * Native CPU interface
 *
 * The native CPU uses system calls to simulate hardware access.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup arch
 * @defgroup    native_cpu  Native CPU
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef _CPU_H
#define _CPU_H

#include <ucontext.h>

#include "kernel_intern.h"
#include "sched.h"
#include "debug.h"

#include "cpu-conf.h"

/* TODO: choose better value? */
#define F_CPU 1000000

void dINT(void);
void eINT(void);

/**
 * register interrupt handler handler for interrupt sig
 */
int register_interrupt(int sig, void *handler);

/**
 * unregister interrupt handler for interrupt sig
 */
int unregister_interrupt(int sig);

/* this should be defined elsewhere */
void thread_yield(void);

extern volatile ucontext_t *interrupted_contexts[MAXTHREADS];
/** @} */
#endif //_CPU_H
