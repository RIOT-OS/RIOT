#ifndef CPUCONF_H_
#define CPUCONF_H_
#include <signal.h>

/* TODO: choose more sensibly? */
#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   8192
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      2048
#define NATIVE_ISR_STACKSIZE            8192

#define _SIG_UNDEF          SIGRTMIN + 0
#define _SIG_TIMER          SIGRTMIN + 10 // check hwtimer_cpu.h for compliance
#define _SIG_LTC4150        SIGRTMIN + 20

/* TODO: check for overflow (SIGRTMAX) */

#endif /* CPUCONF_H_ */
