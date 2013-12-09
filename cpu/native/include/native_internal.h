/**
 * Native CPU internal declarations
 */

/**
 * @ingroup arch
 * @defgroup    native_cpu  Native CPU
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef _NATIVE_INTERNAL_H
#define _NATIVE_INTERNAL_H

//#include <signal.h>

/**
 * internal functions
 */
void native_cpu_init(void);
void native_interrupt_init(void);
extern void native_hwtimer_pre_init();

void native_irq_handler();
extern void _native_sig_leave_tramp(void);

void _native_syscall_leave();
void _native_syscall_enter();

/**
 * external functions regularly wrapped in native for direct use
 */
extern ssize_t (*real_read)(int fd, void *buf, size_t count);
extern ssize_t (*real_write)(int fd, const void *buf, size_t count);

/**
 * data structures
 */
extern volatile int native_interrupts_enabled;
extern volatile unsigned int _native_saved_eip;
extern int _sig_pipefd[2];
extern volatile int _native_sigpend;
extern volatile int _native_in_isr;
extern volatile int _native_in_syscall;

extern char __isr_stack[SIGSTKSZ];
extern char __end_stack[SIGSTKSZ];
extern ucontext_t native_isr_context;
extern ucontext_t end_context;
extern ucontext_t *_native_cur_ctx, *_native_isr_ctx;

#ifdef MODULE_UART0
#include <sys/select.h>
extern fd_set _native_rfds;
#endif

/** @} */
#endif /* _NATIVE_INTERNAL_H */
