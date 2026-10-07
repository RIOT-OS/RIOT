/*
 * SPDX-FileCopyrightText: 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief   Native CPU irq.h implementation
 * @ingroup cpu_native
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

#include "irq.h"
#include "cpu.h"
#include "periph/pm.h"

#include "native_internal.h"
#include "test_utils/expect.h"

#define ENABLE_DEBUG 0
#include "debug.h"
#define DEBUG_IRQ(...) DEBUG("[native] IRQ: " __VA_ARGS__)

volatile bool _native_interrupts_enabled = false;
volatile int _native_in_isr;
__thread volatile int _native_pending_syscalls;

sigset_t _native_sig_set;
static sigset_t _native_sig_set_dint;
static sigset_t _native_sig_set_all;
volatile int _native_pending_signals;
int _signal_pipe_fd[2];

static _native_callback_t _native_irq_handlers[255];

void *thread_isr_stack_pointer(void)
{
    /* interrupts are executed on the stack of the interrupted thread */
    return NULL;
}

void *thread_isr_stack_start(void)
{
    return NULL;
}

void native_print_signals(void)
{
    sigset_t p, q;
    puts("native signals:\n");

    if (sigemptyset(&p) == -1) {
        err(EXIT_FAILURE, "native_print_signals: sigemptyset");
    }

    if (sigpending(&p) == -1) {
        err(EXIT_FAILURE, "native_print_signals: sigpending");
    }

    if (pthread_sigmask(SIG_SETMASK, NULL, &q) != 0) {
        errx(EXIT_FAILURE, "native_print_signals: pthread_sigmask");
    }

    for (int i = 1; i < (NSIG); i++) {
        if (_native_irq_handlers[i] != NULL || i == SIGUSR1) {
            printf("%s: %s in active thread\n",
                   strsignal(i),
                   (sigismember(&_native_sig_set, i) ? "blocked" : "unblocked")
                  );
        }

        if (sigismember(&p, i)) {
            printf("%s: pending\n", strsignal(i));
        }

        if (sigismember(&q, i)) {
            printf("%s: blocked in this context\n", strsignal(i));
        }
    }
}

/**
 * block signals
 */
unsigned irq_disable(void)
{
    unsigned int prev_state;

    _native_syscall_enter();
    DEBUG_IRQ("irq_disable(): _native_in_isr == %i\n", _native_in_isr);

    if (_native_in_isr == 1) {
        DEBUG_IRQ("irq_disable + _native_in_isr\n");
    }

    if (pthread_sigmask(SIG_SETMASK, &_native_sig_set_dint, NULL) != 0) {
        errx(EXIT_FAILURE, "irq_disable: pthread_sigmask");
    }

    prev_state = _native_interrupts_enabled;
    _native_interrupts_enabled = false;

    DEBUG_IRQ("irq_disable(): return\n");
    _native_syscall_leave();

    return prev_state;
}

/**
 * unblock signals
 */
unsigned irq_enable(void)
{
    unsigned int prev_state;

    if (_native_in_isr == 1) {
#     ifdef DEVELHELP
        real_write(STDERR_FILENO, "irq_enable + _native_in_isr\n", 27);
#     else
        DEBUG_IRQ("irq_enable + _native_in_isr\n");
#     endif
    }

    _native_syscall_enter();
    DEBUG_IRQ("irq_enable()\n");

    /* Mark the IRQ as enabled first since pthread_sigmask could call the handler
     * before returning to userspace.
     */

    prev_state = _native_interrupts_enabled;
    _native_interrupts_enabled = true;

    if (pthread_sigmask(SIG_SETMASK, &_native_sig_set, NULL) != 0) {
        errx(EXIT_FAILURE, "irq_enable: pthread_sigmask");
    }

    _native_syscall_leave();

    if (_native_in_isr == 0 && sched_context_switch_request) {
        DEBUG_IRQ("irq_enable() deferred thread_yield_higher()\n");
        thread_yield_higher();
    }

    DEBUG_IRQ("irq_enable(): return\n");

    return prev_state;
}

void irq_restore(unsigned state)
{
    DEBUG_IRQ("irq_restore()\n");

    if (state == 1) {
        irq_enable();
    }
    else {
        irq_disable();
    }

    return;
}

bool irq_is_enabled(void)
{
    return _native_interrupts_enabled;
}

bool irq_is_in(void)
{
    DEBUG_IRQ("irq_is_in: %i\n", _native_in_isr);
    return _native_in_isr;
}

static int _native_pop_sig(void)
{
    int nread, nleft, i;
    int sig = 0;

    nleft = sizeof(int);
    i = 0;

    while ((nleft > 0) && ((nread = real_read(_signal_pipe_fd[0], ((uint8_t*)&sig) + i, nleft))  != -1)) {
        i += nread;
        nleft -= nread;
    }

    if (nread == -1) {
        err(EXIT_FAILURE, "_native_pop_sig: real_read");
    }

    return sig;
}

static void _native_call_sig_handlers(void)
{
    DEBUG_IRQ("\n\n\t\tcall sig handlers\n\n");

    while (_native_pending_signals > 0) {
        int sig = _native_pop_sig();
        _native_pending_signals--;

        if (_native_irq_handlers[sig]) {
            DEBUG_IRQ("call sig handlers: calling interrupt handler for %i\n", sig);
            _native_irq_handlers[sig]();
        }
        else if (sig == SIGUSR1) {
            warnx("call sig handlers: ignoring SIGUSR1");
        }
        else {
            errx(EXIT_FAILURE, "XXX: no handler for signal %i\nXXX: this should not have happened!\n", sig);
        }
    }

    DEBUG_IRQ("call sig handlers: return\n");
}

void _native_isr_run(bool in_signal_handler)
{
    /* signals are already blocked within the signal handler */
    if (!in_signal_handler
        && pthread_sigmask(SIG_SETMASK, &_native_sig_set_all, NULL) != 0) {
        errx(EXIT_FAILURE, "_native_isr_run: pthread_sigmask");
    }

    _native_interrupts_enabled = false;
    _native_in_isr = 1;

    _native_call_sig_handlers();

    if (IS_USED(MODULE_CORE_THREAD) && sched_context_switch_request) {
        sched_run();
        /* returns once this thread is scheduled again */
        _native_switch_to_active();
    }

    _native_in_isr = 0;
    _native_interrupts_enabled = true;

    if (!in_signal_handler
        && pthread_sigmask(SIG_SETMASK, &_native_sig_set, NULL) != 0) {
        errx(EXIT_FAILURE, "_native_isr_run: pthread_sigmask");
    }
}

void native_signal_action(int sig, siginfo_t *info, void *context)
{
    (void) info; /* unused at the moment */
    int saved_errno = errno;

    /* save the signal */
    if (real_write(_signal_pipe_fd[1], &sig, sizeof(int)) == -1) {
        err(EXIT_FAILURE, "native_signal_action: real_write()");
    }
    _native_pending_signals++;

    /* Only handle the interrupt right away if it is allowed to interrupt
     * the current thread, otherwise it will be handled on irq_enable() or
     * when the pending system call returns. */
    if (!_native_interrupts_enabled || _native_in_isr != 0) {
        goto out;
    }

    if (_native_pending_syscalls != 0) {
        DEBUG_IRQ("\n\n\t\tnative_signal_action: return to syscall\n\n");
        goto out;
    }

    if (!_native_is_cpu_owner()) {
        goto out;
    }

    /* Execute the ISR on the current host thread. If this results in a
     * context switch, this blocks until the interrupted thread is scheduled
     * again. */
    _native_isr_run(true);

    /* Return from the signal handler with interrupts enabled. Update the
     * mask as the set of enabled interrupts might have changed meanwhile.
     * Don't assign the whole sigset_t: The libc type may be larger than the
     * one in the kernel's signal frame, which is followed by the FPU state. */
    sigset_t *mask = &((ucontext_t *)context)->uc_sigmask;
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(&_native_sig_set, i)) {
            sigaddset(mask, i);
        }
        else {
            sigdelset(mask, i);
        }
    }

out:
    errno = saved_errno;
}

static void _set_signal_handler(int sig, bool add)
{
    struct sigaction sa;
    int ret;

    /* update the signal mask so irq_enable()/irq_disable() will be aware */
    if (add) {
        _native_syscall_enter();
        ret = sigdelset(&_native_sig_set, sig);
        _native_syscall_leave();
    } else {
        _native_syscall_enter();
        ret = sigaddset(&_native_sig_set, sig);
        _native_syscall_leave();
    }

    if (ret == -1) {
        err(EXIT_FAILURE, "set_signal_handler: sigdelset");
    }

    memset(&sa, 0, sizeof(sa));

    /* Disable all signals during execution of the handler. */
    sigfillset(&sa.sa_mask);

    /* restart interrupted systems call */
    sa.sa_flags = SA_RESTART;

    if (add) {
        sa.sa_flags |= SA_SIGINFO; /* sa.sa_sigaction is used */
        sa.sa_sigaction = native_signal_action;
    } else
    {
        sa.sa_handler = SIG_IGN;
    }

    _native_syscall_enter();
    if (sigaction(sig, &sa, NULL)) {
        err(EXIT_FAILURE, "set_signal_handler: sigaction");
    }
    _native_syscall_leave();
}

/* TODO: use appropriate data structure for signal handlers. */
int native_register_interrupt(int sig, _native_callback_t handler)
{
    DEBUG_IRQ("native_register_interrupt\n");

    unsigned state = irq_disable();

    _native_irq_handlers[sig] = handler;
    _set_signal_handler(sig, true);

    irq_restore(state);

    return 0;
}

int native_unregister_interrupt(int sig)
{
    /* empty signal mask */
    DEBUG_IRQ("native_unregister_interrupt\n");

    unsigned state = irq_disable();

    _set_signal_handler(sig, false);
    _native_irq_handlers[sig] = NULL;

    irq_restore(state);

    return 0;
}

static void native_shutdown(int sig, siginfo_t *info, void *context)
{
    (void)sig;
    (void)info;
    (void)context;

    pm_off();
}

void native_interrupt_init(void)
{
    /* register internal signal handler, initialize local variables
     * TODO: see native_register_interrupt */
    struct sigaction sa;
    DEBUG_IRQ("native_interrupt_init\n");

    _native_pending_signals = 0;
    memset(_native_irq_handlers, 0, sizeof(_native_irq_handlers));

    sa.sa_sigaction = native_signal_action;

    if (sigfillset(&sa.sa_mask) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigfillset");
    }

    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    /* We want to white list authorized signals */
    if (sigfillset(&_native_sig_set) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigprocmask");
    }
    /* we need to disable all signals during our signal handler as it
     * can not cope with interrupted signals ... */
    if (sigfillset(&_native_sig_set_dint) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigfillset");
    }
    /* used while executing interrupts and while a thread is not running */
    if (sigfillset(&_native_sig_set_all) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigfillset");
    }

    /* SIGUSR1 is intended for debugging purposes and shall always be
     * enabled */
    if (sigdelset(&_native_sig_set, SIGUSR1) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigdelset");
    }
    if (sigdelset(&_native_sig_set_dint, SIGUSR1) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigdelset");
    }

    /* SIGUSR1 is handled like a regular interrupt */
    if (sigaction(SIGUSR1, &sa, NULL)) {
        err(EXIT_FAILURE, "native_interrupt_init: sigaction");
    }

    _native_pending_syscalls = 0;

    if (real_pipe(_signal_pipe_fd) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: pipe");
    }

    /* allow for ctrl+c to shut down gracefully always */
    //native_register_interrupt(SIGINT, native_shutdown);
    sa.sa_sigaction = native_shutdown;
    if (sigdelset(&_native_sig_set, SIGINT) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigdelset");
    }
    if (sigdelset(&_native_sig_set_dint, SIGINT) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigdelset");
    }
    if (sigaction(SIGINT, &sa, NULL)) {
        err(EXIT_FAILURE, "native_interrupt_init: sigaction");
    }

    puts("RIOT native interrupts/signals initialized.");
}
