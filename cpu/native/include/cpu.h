/**
 * Native CPU interface
 *
 * The native CPU uses system calls to simulate hardware access.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup arch
 * @defgroup    native_cpu  Native CPU
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
#define _XOPEN_SOURCE
#elif defined(__linux__)
#ifndef __USE_GNU
#define __USE_GNU
#endif
#endif
#include <ucontext.h>
#ifdef BSD
#undef _XOPEN_SOURCE
#elif defined(__linux__)
#undef __USE_GNU
#endif

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

extern void _native_sig_leave_tramp(void);
extern ucontext_t *_native_cur_ctx, *_native_isr_ctx;
extern volatile unsigned int _native_saved_eip;
extern volatile int _native_in_isr;
extern volatile int _native_in_syscall;
extern volatile int _native_sigpend;
#ifdef MODULE_UART0
#include <sys/select.h>
extern fd_set _native_rfds;
#endif
/** @} */
#endif //_CPU_H
