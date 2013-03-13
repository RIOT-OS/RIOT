/**
 * Native CPU irq.h implementation
 *
 * uses POSIX real-time extension signals to create interrupts
 * TODO: needs to be rewritten for better portability
 * 
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup native_cpu
 * @ingroup irq
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */
#include <signal.h>
#include <err.h>

#include <irq.h>
#include "cpu.h"

#include "debug.h"

static int native_interrupts_enabled;
static int native_in_irs;

static int last_sig;
static ucontext_t native_context;
static sigset_t native_sig_set;
static char __isr_stack[SIGSTKSZ];
extern volatile tcb_t *active_thread;

struct int_handler_t {
    void (*func)(void);
};
static struct int_handler_t native_irq_handlers[255];

/**
 * block signals
 */
unsigned disableIRQ(void)
{
    unsigned int prev_state;
    sigset_t mask;

    DEBUG("disableIRQ()\n");

    if (sigfillset(&mask) == -1) {
        err(1, "disableIRQ(): sigfillset");
    }
    if (native_interrupts_enabled == 1) {
        if (sigprocmask(SIG_SETMASK, &mask, &native_sig_set) == -1) {
            err(1, "disableIRQ(): sigprocmask");
        }
    }
    else {
        if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
            err(1, "disableIRQ(): sigprocmask()");
        }
    }
    prev_state = native_interrupts_enabled;
    native_interrupts_enabled = 0;

    return prev_state;
}

/**
 * unblock signals
 */
unsigned enableIRQ(void)
{
    unsigned int prev_state;

    DEBUG("enableIRQ()\n");

    if (sigprocmask(SIG_SETMASK, &native_sig_set, NULL) == -1) {
        err(1, "enableIRQ(): sigprocmask()");
    }
    prev_state = native_interrupts_enabled;
    native_interrupts_enabled = 1;

    return prev_state;
}

void restoreIRQ(unsigned state)
{
    DEBUG("restoreIRQ()\n");

    if (state == 1) {
        enableIRQ();
    }
    else {
        disableIRQ();
    }
    return;
}

int inISR(void)
{
    DEBUG("inISR()\n");
    return native_in_irs;
}


void dINT(void)
{
    disableIRQ();
}

void eINT(void)
{
    enableIRQ();
}

/**
 * call signal handler,
 * restore user context
 */
void native_irq_handler()
{
    if (native_irq_handlers[last_sig].func != NULL) {
        DEBUG("calling interrupt handler for %i\n", last_sig);
        native_irq_handlers[last_sig].func();
    }
    else {
        printf("XXX: no handler for signal %i\n", last_sig);
        errx(1, "XXX: this should not have happened!\n");
    }
    native_in_irs = 0;
    cpu_switch_context_exit();
}


/**
 * load isr context, save signal
 */
void native_isr_entry(int sig, siginfo_t *info, void *context)
{
    /* 
     * This is how it goes:
     * We create a new context "R" for the RIOT interrupt service
     * routine.
     * We save the current (signalhandler) context "S" to the active
     * threads context. 
     * We then jump into the R context.
     * Later, when jumping back into "S", we start out in the signal
     * handler context only to immediately return into the context we
     * originally left. This step is done by the kernel for us.
     *
     * So the thing to wrap your head around is that the active thread
     * remains within in the signal handler context (which is pushed
     * onto the active threads own stack by swapcontext) until the
     * thread is activated again, at which point the kernel handles
     * the context switch back onto the interrupted context for us.
     * */

    /* save the signal */
    last_sig = sig;
    /* indicate irs status */
    native_in_irs = 1;
    
    if (getcontext(&native_context) == -1) {
        err(1, "native_isr_entry(): getcontext()");
    }

    native_context.uc_stack.ss_sp = __isr_stack;
    native_context.uc_stack.ss_size = SIGSTKSZ;
    native_context.uc_stack.ss_flags = 0;

    /* XXX: disable interrupts
     * -> sigfillset(&(native_context.uc_sigmask));
     * else: */
    //sigemptyset(&(native_context.uc_sigmask));
    if (sigfillset(&(native_context.uc_sigmask)) == -1) {
        err(1, "native_isr_entry(): sigfillset()");
    }

    makecontext(&native_context, native_irq_handler, 0);

    if ((swapcontext((ucontext_t*)active_thread->sp, &native_context)) == -1) {
        err(1, "swapcontext failed");
    }
    else {
        DEBUG("returning to interrupted thread\n");
    }
    native_in_irs = 0;
    enableIRQ();
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

    if (sigaddset(&native_sig_set, sig)) {
        err(1, "register_interrupt: sigaddset");
    }

    native_irq_handlers[sig].func = handler;

    sa.sa_sigaction = (void*) native_isr_entry;

    if (sigemptyset(&sa.sa_mask) == -1) {
        err(1, "register_interrupt: sigemptyset");
    }

    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigaction(sig, &sa, NULL)) {
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

    if (sigdelset(&native_sig_set, sig) == -1) {
        err(1, "unregister_interrupt: sigdelset");
    }
    native_irq_handlers[sig].func = NULL;

    sa.sa_sigaction = SIG_IGN;
    if (sigemptyset(&sa.sa_mask) == -1) {
        err(1, "unregister_interrupt: sigemptyset");
    }
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigaction(sig, &sa, NULL)) {
        err(1, "register_interrupt: sigaction");
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

    for (int i = 0; i<255; i++) {
        native_irq_handlers[i].func = NULL;
    }

    sa.sa_sigaction = (void*) native_isr_entry;
    if (sigemptyset(&sa.sa_mask) == -1) {
        err(1, "native_interrupt_init: sigemptyset");
    }
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigemptyset(&native_sig_set) == -1) {
        err(1, "native_interrupt_init: sigemptyset");
    }
    if (sigaddset(&native_sig_set, SIGUSR1) == -1) {
        err(1, "native_interrupt_init: sigaddset");
    }

    if (sigaction(SIGUSR1, &sa, NULL)) {
        err(1, "native_interrupt_init: sigaction");
    }

    puts("RIOT native interrupts/signals initialized.");
}
/** @} */
