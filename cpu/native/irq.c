/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief   Native CPU irq.h implementation
 * @ingroup cpu_native
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#include <err.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util/valgrind.h"

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
volatile int _native_pending_syscalls;

char _isr_stack[THREAD_STACKSIZE_DEFAULT];
static ucontext_t _native_isr_context_storage;
ucontext_t *_native_isr_context = &_native_isr_context_storage;
ucontext_t *_native_current_context = NULL;

volatile uintptr_t _native_user_fptr;

static sigset_t _native_sig_set;
static sigset_t _native_sig_set_dint;
volatile int _native_pending_signals;
int _signal_pipe_fd[2];

static _native_callback_t _native_irq_handlers[255];

static inline void _set_sigmask(ucontext_t *ctx)
{
    ctx->uc_sigmask = _native_sig_set_dint;
    _native_interrupts_enabled = false;
}

void *thread_isr_stack_pointer(void)
{
    return _native_isr_context->uc_stack.ss_sp;
}

void *thread_isr_stack_start(void)
{
    return _isr_stack;
}

void print_thread_sigmask(ucontext_t *cp)
{
    sigset_t *p = &cp->uc_sigmask;

    if (sigemptyset(p) == -1) {
        err(EXIT_FAILURE, "print_thread_sigmask: sigemptyset");
    }

    for (int i = 1; i < (NSIG); i++) {
        if (_native_irq_handlers[i] != NULL) {
            printf("%s: %s\n",
                   strsignal(i),
                   (sigismember(&_native_sig_set, i) ? "blocked" : "unblocked")
                  );
        }

        if (sigismember(p, i)) {
            printf("%s: pending\n", strsignal(i));
        }
    }
}

#ifdef DEVELHELP
void print_sigmasks(void)
{
    for (int i = 0; i < MAXTHREADS; i++) {
        if (sched_threads[i] != NULL) {
            ucontext_t *p;
            printf("%s:\n", sched_threads[i]->name);
            //print_thread_sigmask(sched_threads[i]->sp);
            /* Use intermediate cast to uintptr_t to silence -Wcast-align.
             * stacks are manually word aligned in thread_static_init() */
            p = (ucontext_t *)(uintptr_t)(sched_threads[i]->stack_start);
            print_thread_sigmask(p);
            puts("");
        }
    }
}
#endif

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

    if (sigprocmask(SIG_SETMASK, NULL, &q) == -1) {
        err(EXIT_FAILURE, "native_print_signals: sigprocmask");
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

    if (sigprocmask(SIG_SETMASK, &_native_sig_set_dint, NULL) == -1) {
        err(EXIT_FAILURE, "irq_disable: sigprocmask");
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

    /* Mark the IRQ as enabled first since sigprocmask could call the handler
     * before returning to userspace.
     */

    prev_state = _native_interrupts_enabled;
    _native_interrupts_enabled = true;

    if (sigprocmask(SIG_SETMASK, &_native_sig_set, NULL) == -1) {
        err(EXIT_FAILURE, "irq_enable: sigprocmask");
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

void _native_call_sig_handlers_and_switch(void)
{
    DEBUG_IRQ("\n\n\t\tcall sig handlers + switch\n\n");

    while (_native_pending_signals > 0) {
        int sig = _native_pop_sig();
        _native_pending_signals--;

        if (_native_irq_handlers[sig]) {
            DEBUG_IRQ("call sig handlers + switch: calling interrupt handler for %i\n", sig);
            _native_irq_handlers[sig]();
        }
        else if (sig == SIGUSR1) {
            warnx("call sig handlers + switch: ignoring SIGUSR1");
        }
        else {
            errx(EXIT_FAILURE, "XXX: no handler for signal %i\nXXX: this should not have happened!\n", sig);
        }
    }

    DEBUG_IRQ("call sig handlers + switch: return\n");

    /* Leave ISR context */
    cpu_switch_context_exit();
}

void native_signal_action(int sig, siginfo_t *info, void *context)
{
    (void) info; /* unused at the moment */

    /* save the signal */
    if (real_write(_signal_pipe_fd[1], &sig, sizeof(int)) == -1) {
        err(EXIT_FAILURE, "native_signal_action: real_write()");
    }
    _native_pending_signals++;

    if (context == NULL) {
        errx(EXIT_FAILURE, "native_signal_action: context is null - unhandled");
    }
    if (thread_get_active() == NULL) {
        _native_in_isr++;
        warnx("native_signal_action: thread_get_active() is null - unhandled");
        _native_in_isr--;
        return;
    }

    /* XXX: Workaround safety check - whenever this happens it really
     * indicates a bug in irq_disable */
    if (!_native_interrupts_enabled) {
        return;
    }
    if (_native_in_isr != 0) {
        return;
    }

    if (_native_pending_syscalls != 0) {
        DEBUG_IRQ("\n\n\t\tnative_signal_action: return to syscall\n\n");
        return;
    }

    /* We will switch to the ISR context with ISR stack */
    _native_isr_context_make(_native_call_sig_handlers_and_switch);

    /* Current user thread context */
    _native_current_context = _native_user_context();

    DEBUG_IRQ("\n\n\t\tnative_signal_action: return to _native_sig_leave_tramp\n\n");
    /* disable interrupts in context */
    _set_sigmask((ucontext_t *)context);
    _native_in_isr = 1;

    /* Get PC/LR. This is where we will resume execution on the userspace thread.
     *
     * arm64e: We want the presigned version. This enables us to authenticate the pointer
     * in _native_sig_leave_tramp. */
    _native_user_fptr = (uintptr_t)_context_get_fptr((ucontext_t *)context, /* presigned: */ true);

    /* Now we want to go to _native_sig_leave_tramp before resuming execution at _native_user_fptr.
     *
     * arm64: We supply an unsigned pointer, hence presigned=false.
     * This instructs _context_set_fptr to sign the pointer which is necessary given how
     * setcontext is implemented on macOS. */
    _context_set_fptr(context, (uintptr_t)_native_sig_leave_tramp, /* presigned: */ false);
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

    /* Disable other signal during execution of the handler for this signal. */
    memcpy(&sa.sa_mask,  &_native_sig_set_dint, sizeof(sa.sa_mask));

    /* restart interrupted systems call and custom signal stack */
    sa.sa_flags = SA_RESTART | SA_ONSTACK;

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

    (void) VALGRIND_STACK_REGISTER(__isr_stack, __isr_stack + sizeof(__isr_stack));
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   (void *)__isr_stack, (void*)(__isr_stack + sizeof(__isr_stack)));

    _native_pending_signals = 0;
    memset(_native_irq_handlers, 0, sizeof(_native_irq_handlers));

    sa.sa_sigaction = native_signal_action;

    if (sigfillset(&sa.sa_mask) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigfillset");
    }

    sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;

    /* We want to white list authorized signals */
    if (sigfillset(&_native_sig_set) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: sigprocmask");
    }
    /* we need to disable all signals during our signal handler as it
     * can not cope with interrupted signals ... */
    if (sigfillset(&_native_sig_set_dint) == -1) {
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

    if (getcontext(_native_isr_context) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: getcontext");
    }

    _native_isr_context_make(_native_call_sig_handlers_and_switch);

    static stack_t sigstk;
    sigstk.ss_sp = malloc(SIGSTKSZ);
    expect(sigstk.ss_sp != NULL);
    sigstk.ss_size = SIGSTKSZ;
    sigstk.ss_flags = 0;

    if (sigaltstack(&sigstk, NULL) < 0) {
        err(EXIT_FAILURE, "native_interrupt_init: sigaltstack");
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
