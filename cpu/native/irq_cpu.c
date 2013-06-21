/**
 * Native CPU irq.h implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup native_cpu
 * @ingroup irq
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */
#include <err.h>
#include <string.h>
#include <unistd.h>

// __USE_GNU for gregs[REG_EIP] access under Linux
#define __USE_GNU
#include <signal.h>
#undef __USE_GNU

#include "irq.h"
#include "cpu.h"

#include "debug.h"

static int native_interrupts_enabled;

int _native_sigpend;
int _native_in_isr;
int _native_in_syscall;
static ucontext_t native_isr_context;
static sigset_t native_sig_set;
static char __isr_stack[SIGSTKSZ];
extern volatile tcb_t *active_thread;

unsigned int _native_saved_eip;
ucontext_t *_native_cur_ctx, *_native_isr_ctx;
int _native_in_isr;

static int pipefd[2];

struct int_handler_t {
    void (*func)(void);
};
static struct int_handler_t native_irq_handlers[255];
char sigalt_stk[SIGSTKSZ];

void print_thread_sigmask(ucontext_t *cp)
{
    sigset_t *p = &cp->uc_sigmask;

    if(sigemptyset(p) == -1) {
        err(1, "print_thread_sigmask: sigemptyset");
    }

    for(int i = 1; i < (NSIG); i++) {
        if(native_irq_handlers[i].func != NULL) {
            printf("%s: %s\n",
                   strsignal(i),
                   (sigismember(&native_sig_set, i) ? "blocked" : "unblocked")
                  );
        }

        if(sigismember(p, i)) {
            printf("%s: pending\n", strsignal(i));
        }
    }
}

void print_sigmasks(void)
{
    ucontext_t *p;
    //tcb_t *cb = NULL;

    for(int i = 0; i < MAXTHREADS; i++) {
        if(sched_threads[i] != NULL) {
            printf("%s:\n", sched_threads[i]->name);
            //print_thread_sigmask(sched_threads[i]->sp);
            p = (ucontext_t *)(sched_threads[i]->stack_start);
            print_thread_sigmask(p);
            puts("");
        }
    }
}

void native_print_signals()
{
    sigset_t p, q;
    puts("native signals:\n");

    if(sigemptyset(&p) == -1) {
        err(1, "native_print_signals: sigemptyset");
    }

    if(sigpending(&p) == -1) {
        err(1, "native_print_signals: sigpending");
    }

    if(sigprocmask(SIG_SETMASK, NULL, &q) == -1) {
        err(1, "native_print_signals(): sigprocmask");
    }

    for(int i = 1; i < (NSIG); i++) {
        if(native_irq_handlers[i].func != NULL || i == SIGUSR1) {
            printf("%s: %s in active thread\n",
                   strsignal(i),
                   (sigismember(&native_sig_set, i) ? "blocked" : "unblocked")
                  );
        }

        if(sigismember(&p, i)) {
            printf("%s: pending\n", strsignal(i));
        }

        if(sigismember(&q, i)) {
            printf("%s: blocked in this context\n", strsignal(i));
        }
    }
}

/**
 * block signals
 */
unsigned disableIRQ(void)
{
    unsigned int prev_state;
    sigset_t mask;

    _native_in_syscall = 1;
    DEBUG("disableIRQ()\n");

    if(sigfillset(&mask) == -1) {
        err(1, "disableIRQ(): sigfillset");
    }

    if(native_interrupts_enabled == 1) {
        DEBUG("sigprocmask(..native_sig_set)\n");

        if(sigprocmask(SIG_SETMASK, &mask, &native_sig_set) == -1) {
            err(1, "disableIRQ(): sigprocmask");
        }
    }
    else {
        DEBUG("sigprocmask()\n");

        if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
            err(1, "disableIRQ(): sigprocmask()");
        }
    }

    prev_state = native_interrupts_enabled;
    native_interrupts_enabled = 0;

    if(_native_sigpend > 0) {
        DEBUG("\n\n\t\treturn from syscall, calling native_irq_handler\n\n");
        _native_in_syscall = 0;
        printf("calling swapcontext()\n");
        swapcontext(_native_cur_ctx, _native_isr_ctx);
    }
    else {
        _native_in_syscall = 0;
    }

    DEBUG("disableIRQ(): return\n");

    return prev_state;
}

/**
 * unblock signals
 */
unsigned enableIRQ(void)
{
    unsigned int prev_state;

    _native_in_syscall = 1;
    DEBUG("enableIRQ()\n");

    if(sigprocmask(SIG_SETMASK, &native_sig_set, NULL) == -1) {
        err(1, "enableIRQ(): sigprocmask()");
    }

    prev_state = native_interrupts_enabled;
    native_interrupts_enabled = 1;

    //print_sigmasks();
    //native_print_signals();
    if(_native_sigpend > 0) {
        DEBUG("\n\n\t\treturn from syscall, calling native_irq_handler\n\n");
        _native_in_syscall = 0;
        printf("calling swapcontext()\n");
        swapcontext(_native_cur_ctx, _native_isr_ctx);
    }
    else {
        _native_in_syscall = 0;
    }

    DEBUG("enableIRQ(): return\n");

    return prev_state;
}

void restoreIRQ(unsigned state)
{
    DEBUG("restoreIRQ()\n");

    if(state == 1) {
        enableIRQ();
    }
    else {
        disableIRQ();
    }

    return;
}

int inISR(void)
{
    DEBUG("inISR(): %i\n", _native_in_isr);
    return _native_in_isr;
}


void dINT(void)
{
    disableIRQ();
}

void eINT(void)
{
    enableIRQ();
}

int _native_popsig(void)
{
    int nread, nleft, i;
    int sig;

    nleft = sizeof(int);
    i = 0;

    while((nleft > 0) && ((nread = read(pipefd[0], &sig + i, nleft))  != -1)) {
        i += nread;
        nleft -= nread;
    }

    if(nread == -1) {
        err(1, "_native_popsig(): read()");
    }

    return sig;
}

/**
 * call signal handlers,
 * restore user context
 */
void native_irq_handler()
{
    int sig;

    DEBUG("\n\n\t\tnative_irq_handler\n\n");

    while(_native_sigpend > 0) {

        sig = _native_popsig();
        _native_sigpend--;

        if(native_irq_handlers[sig].func != NULL) {
            DEBUG("calling interrupt handler for %i\n", sig);
            native_irq_handlers[sig].func();
        }
        else if(sig == SIGUSR1) {
            DEBUG("ignoring SIGUSR1\n");
        }
        else {
            printf("XXX: no handler for signal %i\n", sig);
            errx(1, "XXX: this should not have happened!\n");
        }
    }

    DEBUG("native_irq_handler(): return");
    _native_in_isr = 0;
    cpu_switch_context_exit();
}


/**
 * save signal, return to _native_sig_leave_tramp if possible
 */
void native_isr_entry(int sig, siginfo_t *info, void *context)
{
    DEBUG("\n\n\t\tnative_isr_entry\n\n");

    if(native_interrupts_enabled == 0) {
        errx(1, "interrupts are off, but I caught a signal.");
    }

    /* save the signal */
    if(write(pipefd[1], &sig, sizeof(int)) == -1) {
        err(1, "native_isr_entry(): write()");
    }

    _native_sigpend++;
    /* indicate irs status */

    makecontext(&native_isr_context, native_irq_handler, 0);
    _native_cur_ctx = (ucontext_t *)active_thread->sp;

    if(_native_in_syscall == 0) {
        _native_in_isr = 1;
        DEBUG("\n\n\t\treturn to _native_sig_leave_tramp\n\n");
#ifdef __MACH__
        _native_saved_eip = ((ucontext_t *)context)->uc_mcontext->__ss.__eip;
        ((ucontext_t *)context)->uc_mcontext->__ss.__eip = (unsigned int)&_native_sig_leave_tramp;
#elif BSD
        _native_saved_eip = ((struct sigcontext *)context)->sc_eip;
        ((struct sigcontext *)context)->sc_eip = (unsigned int)&_native_sig_leave_tramp;
#else
        _native_saved_eip = ((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP];
        ((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP] = (unsigned int)&_native_sig_leave_tramp;
#endif
        // TODO: change sigmask?
    }
    else {
        DEBUG("\n\n\t\treturn to syscall\n\n");
    }
}

/**
 * register signal/interrupt handler for signal sig
 *
 * TODO: check sa_flags for appropriateness
 * TODO: use appropriate data structure for signal
 *       handlers.
 */
int register_interrupt(int sig, void *handler)
{
    struct sigaction sa;
    DEBUG("XXX: register_interrupt()\n");

    if(sigdelset(&native_sig_set, sig)) {
        err(1, "register_interrupt: sigdelset");
    }

    native_irq_handlers[sig].func = handler;

    sa.sa_sigaction = (void *) native_isr_entry;
    /* sa.sa_handler = (void*) native_isr_entry; */

    if(sigemptyset(&sa.sa_mask) == -1) {
        err(1, "register_interrupt: sigemptyset");
    }

    sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;

    if(sigaction(sig, &sa, NULL)) {
        err(1, "register_interrupt: sigaction");
    }

    return 0;
}

/**
 * empty signal mask
 *
 * TODO: see register_interrupt
 * TODO: ignore signal
 */
int unregister_interrupt(int sig)
{
    struct sigaction sa;
    DEBUG("XXX: unregister_interrupt()\n");

    if(sigaddset(&native_sig_set, sig) == -1) {
        err(1, "unregister_interrupt: sigaddset");
    }

    native_irq_handlers[sig].func = NULL;

    /* sa.sa_sigaction = SIG_IGN; */
    sa.sa_handler = SIG_IGN;

    if(sigemptyset(&sa.sa_mask) == -1) {
        err(1, "unregister_interrupt: sigemptyset");
    }

    sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;

    if(sigaction(sig, &sa, NULL)) {
        err(1, "unregister_interrupt: sigaction");
    }

    return 0;
}


/**
 * register internal signal handler,
 * initalize local variables
 *
 * TODO: see register_interrupt
 */
void native_interrupt_init(void)
{
    struct sigaction sa;
    DEBUG("XXX: native_interrupt_init()\n");

    native_interrupts_enabled = 1;
    _native_sigpend = 0;

    for(int i = 0; i < 255; i++) {
        native_irq_handlers[i].func = NULL;
    }

    sa.sa_sigaction = (void *) native_isr_entry;

    if(sigemptyset(&sa.sa_mask) == -1) {
        err(1, "native_interrupt_init: sigemptyset");
    }

    sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;

    /*
    if (sigemptyset(&native_sig_set) == -1) {
        err(1, "native_interrupt_init: sigemptyset");
    }
    */
    if(sigprocmask(SIG_SETMASK, NULL, &native_sig_set) == -1) {
        err(1, "native_interrupt_init(): sigprocmask");
    }

    if(sigdelset(&native_sig_set, SIGUSR1) == -1) {
        err(1, "native_interrupt_init: sigdelset");
    }

    if(sigaction(SIGUSR1, &sa, NULL)) {
        err(1, "native_interrupt_init: sigaction");
    }

    if(getcontext(&native_isr_context) == -1) {
        err(1, "native_isr_entry(): getcontext()");
    }

    if(sigfillset(&(native_isr_context.uc_sigmask)) == -1) {
        err(1, "native_isr_entry(): sigfillset()");
    }

    native_isr_context.uc_stack.ss_sp = __isr_stack;
    native_isr_context.uc_stack.ss_size = SIGSTKSZ;
    native_isr_context.uc_stack.ss_flags = 0;
    _native_isr_ctx = &native_isr_context;

    static stack_t sigstk;
    sigstk.ss_sp = sigalt_stk;
    sigstk.ss_size = SIGSTKSZ;
    sigstk.ss_flags = 0;

    if(sigaltstack(&sigstk, NULL) < 0) {
        err(1, "main: sigaltstack");
    }

    makecontext(&native_isr_context, native_irq_handler, 0);

    _native_in_syscall = 0;

    if(pipe(pipefd) == -1) {
        err(1, "native_interrupt_init(): pipe()");
    }

    puts("RIOT native interrupts/signals initialized.");
}
/** @} */
