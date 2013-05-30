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

#ifdef __MACH__
#define _XOPEN_SOURCE
#endif
#include <ucontext.h>
#ifdef __MACH__
#undef _XOPEN_SOURCE
#endif

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

extern void _native_sig_leave_tramp(void);
extern ucontext_t *_native_cur_ctx, *_native_isr_ctx;
extern unsigned int _native_saved_eip;
extern int _native_in_isr;
extern int _native_in_syscall;
extern int _native_sigpend;
/** @} */
#endif //_CPU_H
