/**
 * Native CPU interface
 *
 * The native CPU uses system calls to simulate hardware access.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup arch
 * @defgroup    native_cpu  Native CPU
 * @ingroup		cpu
 * @brief		CPU abstraction for the native port
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef _CPU_H
#define _CPU_H

#include <sys/param.h>

/* enable signal handler register access on different platforms
 * check here for more:
 * http://sourceforge.net/p/predef/wiki/OperatingSystems/
 */
#ifdef BSD // BSD = (FreeBSD, Darwin, ...)
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#include <ucontext.h>
#undef _XOPEN_SOURCE
#else
#include <ucontext.h>
#endif
#elif defined(__linux__)
#ifndef _GNU_SOURCE
#define GNU_SOURCE
#include <ucontext.h>
#undef GNU_SOURCE
#else
#include <ucontext.h>
#endif
#endif // BSD/Linux

#include "kernel_internal.h"
#include "sched.h"

#include "cpu-conf.h"

/* TODO: choose better value? */
#define F_CPU 1000000

void dINT(void);
void eINT(void);

/**
 * register interrupt handler handler for interrupt sig
 */
int register_interrupt(int sig, void (*handler)(void));

/**
 * unregister interrupt handler for interrupt sig
 */
int unregister_interrupt(int sig);

/* this should be defined elsewhere */
void thread_yield(void);

/** @} */
#endif //_CPU_H
