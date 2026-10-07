/*
 * SPDX-FileCopyrightText: 2013-2014 Ludwig Knüpfer
 * SPDX-FileCopyrightText: 2025 carl-tud
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup cpu_native_stdio  STDIO for native
 * @ingroup  sys_stdio
 * @brief    Standard input/output backend for native
 * @{
 *
 * This will hook up RIOT's stdio to the host's stdio fds. It is the default
 * stdio implementation of the board `native`.
 *
 * @see @ref cpu_native

 * @}
 */

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  Native CPU internal symbols
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author carl-tud
 */

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <poll.h>

#include <stdbool.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/uio.h>
#include <dirent.h>

#include "cpu_conf.h"
#include "thread.h"
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "syscalls.h"

/* MARK: - Internal native CPU API */
/**
 * @name Internal native CPU API
 * @{
 */

/**
 * @brief Prototype for native's internal callbacks
 */
typedef void (*_native_callback_t)(void);

/**
 * @brief Initializes native CPU
 */
void native_cpu_init(void);
/** @} */

/* MARK: - Native Signal Handling */
/**
 * @name Native Signal Handling
 * @{
 */

/**
 * @brief A boolean variable indicating whether interrupts are currently permissible
 * @private
 */
extern volatile bool _native_interrupts_enabled;

/**
 * @brief Signal set during "IRQs enabled"
 * @internal
 */
extern sigset_t _native_sig_set;

/**
 * @brief Pipe yielding signals
 * @private
 *
 * The internal signal handler writes into the write end of this pipe
 */
extern int _signal_pipe_fd[2];

/**
 * @brief Number of currently pending signals
 * @private
 */
extern volatile int _native_pending_signals;

/**
 * @brief Registers signal handlers for the native CPU
 */
void native_interrupt_init(void);

/**
 * @brief Register interrupt handler handler for interrupt signal
 *
 * @param sig Signal number
 * @param handler Signal action function
 *
 * @returns 0 on success, negative number otherwise.
 */
int native_register_interrupt(int sig, _native_callback_t handler);

/**
 * @brief Unregister interrupt handler for interrupt signal
 *
 * @param sig Signal number
 *
 * @returns 0 on success, negative number otherwise.
 */
int native_unregister_interrupt(int sig);

/**
 * @brief Calls signal handlers for pending signals and performs a context switch if requested
 * @pre Interrupts are enabled, not in ISR context and the calling host thread is the CPU owner
 * @private
 *
 * Interrupts are disabled while the handlers are executed. If a context switch
 * is performed, this function only returns once the calling thread is scheduled
 * again. Interrupts are enabled again when this function returns.
 *
 * @param in_signal_handler `true` if called from within the native signal handler
 */
void _native_isr_run(bool in_signal_handler);
/** @} */

/* MARK: - System Calls */
/**
 * @name System Calls
 *
 * We wrap system calls and syscall-invoking library calls to ensure **no context switches occur during a system call**.
 * @{
 */
/**
 * @brief Number of currently pending system calls of the calling host thread
 * @private
 */
extern __thread volatile int _native_pending_syscalls;

/**
 * @brief Increment spending system call counter
 */
static inline void _native_pending_syscalls_up(void) {
    _native_pending_syscalls += 1;
}

/**
 * @brief Decrements pending system call counter
 */
static inline void _native_pending_syscalls_down(void) {
    _native_pending_syscalls -= 1;
}

/**
 * @brief Executes post-syscall logic
 * @private
 *
 * Decrements internal pending syscall counter.
 *
 * If there are other pending system calls and when not already in an ISR context, this function switches to the ISR context and
 * calls signals handlers. This is done through @ref `_native_call_sig_handlers_and_switch`. Returns to userspace
 * thereafter.
 */
void _native_syscall_leave(void);

/**
 * @brief Executes pre-syscall logic
 * @private
 *
 * Increments internal pending syscall counter.
 */
void _native_syscall_enter(void);

/**
 * @brief Registers system calls.
 *
 * Wraps syscall functions from the standard library.
 */
void _native_init_syscalls(void);
/** @} */

/* MARK: - Native Context Switching */
/**
 * @name Native Context Switching
 * @{
 */

/**
 * @brief A boolean variable indicating whether program execution currently takes place in an ISR context
 */
extern volatile int _native_in_isr;

/**
 * @brief Checks whether the calling host thread backs the active RIOT thread
 * @private
 *
 * Each RIOT thread is backed by a host thread, but only the host thread of the
 * active RIOT thread (the CPU owner) is allowed to run.
 */
bool _native_is_cpu_owner(void);

/**
 * @brief   Check if the caller is host internal code running outside of a
 *          RIOT thread context (e.g. the host libc managing host threads)
 *
 * Memory allocated by such code must not be tracked by malloc_monitor, as
 * the monitor requires a RIOT thread context.
 */
bool _native_host_internal(void);

/**
 * @brief Switches to the active thread as selected by the scheduler
 * @pre All signals are blocked and the calling host thread was the CPU owner
 * @private
 *
 * Hands the CPU over to the host thread of the thread returned by
 * @ref thread_get_active and blocks until the calling thread is scheduled
 * again. Returns immediately if the calling thread is still the active thread.
 */
void _native_switch_to_active(void);
/** @} */

/* MARK: - Native Process State */
/**
 * @name Native Process State
 * @{
 */
/**
 * @brief Program name
 * @private
 */
extern const char *_progname;

/**
 * @brief Program argument values
 * @private
 */
extern char **_native_argv;

/**
 * @brief Process Identifier
 * @private
 */
extern pid_t _native_pid;

/**
 * @brief Process Identifier / CPUID ???
 * @private
 */
extern pid_t _native_id;

/**
 * @brief Random number generator seed value
 * @private
 */
extern unsigned int _native_rng_seed;

/**
 * @brief Random number generator mode
 * @private
 *
 * - `0`: Use `/dev/random`
 * - `1`: Use `random(3)`
 */
extern int _native_rng_mode;
/** @} */

/* MARK: - Native Read/Write Methods */
/**
 * @name Native Read/Write Methods
 * @{
 */
/**
 * @brief Reads file, populates given buffer
 */
ssize_t _native_read(int fd, void *buf, size_t count);

/**
 * @brief Writes given data into file
 */
ssize_t _native_write(int fd, const void *buf, size_t count);

/**
 * @brief Performs a vectored write operation
 */
ssize_t _native_writev(int fildes, const struct iovec *iov, int iovcnt);
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
