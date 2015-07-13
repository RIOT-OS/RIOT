/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * Coroutine helper functions. The base of the multi-threading system.
 *
 * @ingroup x86-multithreading
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__UCONTEXT__HPP__
#define CPU__X86__UCONTEXT__HPP__

#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Common stacksize for a signal handler.
 *
 * Do not use this variable.
 * The standard wants us to define this variable, but you are better off the the RIOT specific macros.
 */
#define SIGSTKSZ (2048)

/**
 * @brief   General purpose registers of an x86 CPU.
 *
 * Used by ucontext_t.
 * Application developers should not use this type directly.
 */
struct x86_pushad {
    unsigned long ax, cx, dx, bx; /* field in ucontext_t: 3*4 -> 6*4 */
    unsigned long sp, bp, si, di; /* field in ucontext_t: 7*4 -> 10*4 */
} __attribute__((packed));

/**
 * @brief   Opaque memory needed to store the x87 FPU state.
 *
 * Used by x86_threading.c.
 * Application developers should not use this type directly.
 * There is only enough room for the basic x87 state, not the multimedia extensions.
 */
struct x86_fxsave {
    char opaque[512];
} __attribute__((packed, aligned(0x10)));

/**
 * @brief   Machine specific part of the corouting state.
 *
 * Used by ucontext_t.
 * Application developers should not use this type directly.
 */
typedef struct mcontext {
    unsigned long flags;         /* field in ucontext_t: 2*4 */
    struct x86_pushad registers; /* field in ucontext_t: 3*4 -> 10*4 */
    void *ip;                    /* field in ucontext_t: 11*4 */
} __attribute__((packed)) mcontext_t;

/**
 * @brief   Stack assigned to a coroutine.
 *
 * Used by ucontext_t.
 * Application developers need to set this values to make coroutines working.
 */
typedef struct stack {
    void *ss_sp; /* field in ucontext_t: 0*4 */
    //int ss_flags;
    size_t ss_size; /* field in ucontext_t: 1*4 */
} __attribute__((packed)) stack_info_t;

/**
 * @brief   Extra data to perform an `iret`.
 *
 * Used by x86_interrupts.c to continue a thread
 * if sched_context_switch_request was set set by an ISR.
 * Application developers should not use this type directly.
 */
struct x86_interrupted_interrupt {
    unsigned long ip;
    unsigned long flags;
} __attribute__((packed));

/**
 * @brief   Datatype to enable coroutines in userspace.
 *
 * This datatype stores the machine state of a suspended coroutine.
 */
typedef struct ucontext {
    stack_info_t uc_stack;    /* field in ucontext_t: 0*4 - 1*4 */
    mcontext_t uc_context;    /* field in ucontext_t: 2*4 -> 11*4 */
    struct ucontext *uc_link; /* field in ucontext_t: 12*4 */
    //sigset_t uc_sigmask;
    struct x86_interrupted_interrupt __intr;
    struct x86_fxsave __fxsave;
} __attribute__((packed)) ucontext_t;

/**
 * @brief   Store current coroutine state.
 *
 * With setcontext() you can jump to this point of execution again.
 * Take care about your stack, though.
 * After your thread of execution left the function calling getcontext() there be dragons.
 *
 * The FPU registers are not stored!
 */
int getcontext(ucontext_t *ucp);

/**
 * @brief   Restore a coroutine state.
 *
 * The state must be set up by calling getcontext() or makecontext() previously.
 * The FPU registers won't be restored.
 */
int setcontext(const ucontext_t *ucp) __attribute__((noreturn));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
typedef void (*makecontext_fun_t)();
#pragma GCC diagnostic pop

/**
 * @brief   Create a coroutine / trampoline.
 * @param[in]   func   Function to call.
 * @param[in]   argc   Number of arguments for func. Must not exceed 5.
 *
 * The function arguments shall be passed after argc.
 * The arguments have to be type punnable as an int,
 * this includes integral types up to 4 bytes,
 * but excludes int64_t, float and so on.
 *
 * Different from what the POSIX standard states,
 * you do not have to getcontext() on ucp before calling makecontext().
 * This function creates a whole new coroutine state / trampoline.
 */
void makecontext(ucontext_t *ucp, makecontext_fun_t func, int argc, ...);

/**
 * @brief   Swap coroutine.
 * @param[out]   oucp   Memory to preserve current coroutine state in.
 * @param[in]    ucp    Coroutine to execute next.
 *
 * This function is an atomic variant of getcontext() and setcontext().
 * Same restrictions apply.
 */
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
