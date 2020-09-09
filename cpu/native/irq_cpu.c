/**
 * Native CPU irq.h implementation
 *
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup cpu_native
 * @{
 * @file
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#include <err.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef HAVE_VALGRIND_H
#include <valgrind.h>
#define VALGRIND_DEBUG DEBUG
#elif defined(HAVE_VALGRIND_VALGRIND_H)
#include <valgrind/valgrind.h>
#define VALGRIND_DEBUG DEBUG
#else
#define VALGRIND_STACK_REGISTER(...)
#define VALGRIND_DEBUG(...)
#endif

/* __USE_GNU for gregs[REG_EIP] access under Linux */
#define __USE_GNU
#include <signal.h>
#undef __USE_GNU

#include "irq.h"
#include "cpu.h"
#include "periph/pm.h"

#include "native_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

volatile int native_interrupts_enabled = 0;
volatile int _native_in_isr;
volatile int _native_in_syscall;

static sigset_t _native_sig_set, _native_sig_set_dint;

char __isr_stack[SIGSTKSZ];
ucontext_t native_isr_context;
ucontext_t *_native_cur_ctx, *_native_isr_ctx;

volatile unsigned int _native_saved_eip;
volatile int _native_sigpend;
int _sig_pipefd[2];

static _native_callback_t native_irq_handlers[255];
char sigalt_stk[SIGSTKSZ];

void *thread_isr_stack_pointer(void)
{
    return native_isr_context.uc_stack.ss_sp;
}

void *thread_isr_stack_start(void)
{
    return __isr_stack;
}

void print_thread_sigmask(ucontext_t *cp)
{
    sigset_t *p = &cp->uc_sigmask;

    if (sigemptyset(p) == -1) {
        err(EXIT_FAILURE, "print_thread_sigmask: sigemptyset");
    }

    for (int i = 1; i < (NSIG); i++) {
        if (native_irq_handlers[i] != NULL) {
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
            p = (ucontext_t *)(sched_threads[i]->stack_start);
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
        if (native_irq_handlers[i] != NULL || i == SIGUSR1) {
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
    DEBUG("irq_disable()\n");

    if (_native_in_isr == 1) {
        DEBUG("irq_disable + _native_in_isr\n");
    }

    if (sigprocmask(SIG_SETMASK, &_native_sig_set_dint, NULL) == -1) {
        err(EXIT_FAILURE, "irq_disable: sigprocmask");
    }

    prev_state = native_interrupts_enabled;
    native_interrupts_enabled = 0;

    DEBUG("irq_disable(): return\n");
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
#ifdef DEVELHELP
        real_write(STDERR_FILENO, "irq_enable + _native_in_isr\n", 27);
#else
        DEBUG("irq_enable + _native_in_isr\n");
#endif
    }

    _native_syscall_enter();
    DEBUG("irq_enable()\n");

    /* Mark the IRQ as enabled first since sigprocmask could call the handler
     * before returning to userspace.
     */

    prev_state = native_interrupts_enabled;
    native_interrupts_enabled = 1;

    if (sigprocmask(SIG_SETMASK, &_native_sig_set, NULL) == -1) {
        err(EXIT_FAILURE, "irq_enable: sigprocmask");
    }

    _native_syscall_leave();

    DEBUG("irq_enable(): return\n");

    return prev_state;
}

void irq_restore(unsigned state)
{
    DEBUG("irq_restore()\n");

    if (state == 1) {
        irq_enable();
    }
    else {
        irq_disable();
    }

    return;
}

int irq_is_in(void)
{
    DEBUG("irq_is_in: %i\n", _native_in_isr);
    return _native_in_isr;
}

int _native_popsig(void)
{
    int nread, nleft, i;
    int sig = 0;

    nleft = sizeof(int);
    i = 0;

    while ((nleft > 0) && ((nread = real_read(_sig_pipefd[0], ((uint8_t*)&sig) + i, nleft))  != -1)) {
        i += nread;
        nleft -= nread;
    }

    if (nread == -1) {
        err(EXIT_FAILURE, "_native_popsig: real_read");
    }

    return sig;
}

/**
 * call signal handlers,
 * restore user context
 */
void native_irq_handler(void)
{
    DEBUG("\n\n\t\tnative_irq_handler\n\n");

    while (_native_sigpend > 0) {
        int sig = _native_popsig();
        _native_sigpend--;

        if (native_irq_handlers[sig] != NULL) {
            DEBUG("native_irq_handler: calling interrupt handler for %i\n", sig);
            native_irq_handlers[sig]();
        }
        else if (sig == SIGUSR1) {
            warnx("native_irq_handler: ignoring SIGUSR1");
        }
        else {
            errx(EXIT_FAILURE, "XXX: no handler for signal %i\nXXX: this should not have happened!\n", sig);
        }
    }

    DEBUG("native_irq_handler: return\n");
    cpu_switch_context_exit();
}

void isr_set_sigmask(ucontext_t *ctx)
{
    ctx->uc_sigmask = _native_sig_set_dint;
    native_interrupts_enabled = 0;
}

/**
 * save signal, return to _native_sig_leave_tramp if possible
 */
void native_isr_entry(int sig, siginfo_t *info, void *context)
{
    (void) info; /* unused at the moment */
    //printf("\n\033[33m\n\t\tnative_isr_entry(%i)\n\n\033[0m", sig);

    /* save the signal */
    if (real_write(_sig_pipefd[1], &sig, sizeof(int)) == -1) {
        err(EXIT_FAILURE, "native_isr_entry: real_write()");
    }
    _native_sigpend++;
    //real_write(STDOUT_FILENO, "sigpend\n", 8);

    if (context == NULL) {
        errx(EXIT_FAILURE, "native_isr_entry: context is null - unhandled");
    }
    if (thread_get_active() == NULL) {
        _native_in_isr++;
        warnx("native_isr_entry: thread_get_active() is null - unhandled");
        _native_in_isr--;
        return;
    }

    /* XXX: Workaround safety check - whenever this happens it really
     * indicates a bug in irq_disable */
    if (native_interrupts_enabled == 0) {
        //printf("interrupts are off, but I caught a signal.\n");
        return;
    }
    if (_native_in_isr != 0) {
        //real_write(STDOUT_FILENO, "interrupts in ISR!!\n", 20);
        return;
    }

    if (_native_in_syscall != 0) {
        DEBUG("\n\n\t\tnative_isr_entry: return to syscall\n\n");
        return;
    }

    native_isr_context.uc_stack.ss_sp = __isr_stack;
    native_isr_context.uc_stack.ss_size = sizeof(__isr_stack);
    native_isr_context.uc_stack.ss_flags = 0;
    makecontext(&native_isr_context, native_irq_handler, 0);
    _native_cur_ctx = (ucontext_t *)thread_get_active()->sp;

    DEBUG("\n\n\t\tnative_isr_entry: return to _native_sig_leave_tramp\n\n");
    /* disable interrupts in context */
    isr_set_sigmask((ucontext_t *)context);
    _native_in_isr = 1;
    /*
     * For register access on new platforms see:
     * http://google-glog.googlecode.com/svn/trunk/m4/pc_from_ucontext.m4
     * (URL added on Fri Aug 29 17:17:45 CEST 2014)
     */
#ifdef __MACH__
    _native_saved_eip = ((ucontext_t *)context)->uc_mcontext->__ss.__eip;
    ((ucontext_t *)context)->uc_mcontext->__ss.__eip = (unsigned int)&_native_sig_leave_tramp;
#elif defined(__FreeBSD__)
    _native_saved_eip = ((struct sigcontext *)context)->sc_eip;
    ((struct sigcontext *)context)->sc_eip = (unsigned int)&_native_sig_leave_tramp;
#else /* Linux */
#if defined(__arm__)
    _native_saved_eip = ((ucontext_t *)context)->uc_mcontext.arm_pc;
    ((ucontext_t *)context)->uc_mcontext.arm_pc = (unsigned int)&_native_sig_leave_tramp;
#else /* Linux/x86 */
    //printf("\n\033[31mEIP:\t%p\ngo switching\n\n\033[0m", (void*)((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP]);
    _native_saved_eip = ((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP];
    ((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP] = (unsigned int)&_native_sig_leave_tramp;
#endif
#endif
}

/**
 * Add or remove handler for signal
 *
 * To be called with interrupts disabled
 *
 */
void set_signal_handler(int sig, bool add)
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
        sa.sa_sigaction = native_isr_entry;
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

/**
 * register signal/interrupt handler for signal sig
 *
 * TODO: use appropriate data structure for signal
 *       handlers.
 */
int register_interrupt(int sig, _native_callback_t handler)
{
    DEBUG("register_interrupt\n");

    unsigned state = irq_disable();

    native_irq_handlers[sig] = handler;
    set_signal_handler(sig, true);

    irq_restore(state);

    return 0;
}

/**
 * empty signal mask
 */
int unregister_interrupt(int sig)
{
    DEBUG("unregister_interrupt\n");

    unsigned state = irq_disable();

    set_signal_handler(sig, false);
    native_irq_handlers[sig] = NULL;

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

/**
 * register internal signal handler,
 * initialize local variables
 *
 * TODO: see register_interrupt
 */
void native_interrupt_init(void)
{
    struct sigaction sa;
    DEBUG("native_interrupt_init\n");

    VALGRIND_STACK_REGISTER(__isr_stack, __isr_stack + sizeof(__isr_stack));
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   (void *)__isr_stack, (void*)((int)__isr_stack + sizeof(__isr_stack)));

    _native_sigpend = 0;

    for (int i = 0; i < 255; i++) {
        native_irq_handlers[i] = NULL;
    }

    sa.sa_sigaction = native_isr_entry;

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

    if (getcontext(&native_isr_context) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: getcontext");
    }

    native_isr_context.uc_stack.ss_sp = __isr_stack;
    native_isr_context.uc_stack.ss_size = sizeof(__isr_stack);
    native_isr_context.uc_stack.ss_flags = 0;
    _native_isr_ctx = &native_isr_context;

    static stack_t sigstk;
    sigstk.ss_sp = sigalt_stk;
    sigstk.ss_size = sizeof(__isr_stack);
    sigstk.ss_flags = 0;

    if (sigaltstack(&sigstk, NULL) < 0) {
        err(EXIT_FAILURE, "native_interrupt_init: sigaltstack");
    }

    makecontext(&native_isr_context, native_irq_handler, 0);

    _native_in_syscall = 0;

    if (real_pipe(_sig_pipefd) == -1) {
        err(EXIT_FAILURE, "native_interrupt_init: pipe");
    }

    /* allow for ctrl+c to shut down gracefully always */
    //register_interrupt(SIGINT, native_shutdown);
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
/** @} */
