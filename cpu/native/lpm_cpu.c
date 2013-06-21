/**
 * Native CPU lpm.h implementation
 *
 * Uses system calls to emulate CPU power modes.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup lpm
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef MODULE_UART0
#include <sys/select.h>
#include <errno.h>
#endif
#include <err.h>

#include "lpm.h"
#include "debug.h"
#include "cpu.h"
#ifdef MODULE_UART0
#include "board.h"
#endif

static enum lpm_mode native_lpm;

void lpm_init(void)
{
    DEBUG("lpm_init()\n");
    native_lpm = LPM_ON;
    return;
}

void _native_lpm_sleep()
{
#ifdef MODULE_UART0
    int retval;
    retval = select(1, &_native_uart_rfds, NULL, NULL, NULL);
    DEBUG("_native_lpm_sleep: retval: %i\n", retval);

    if(retval != -1) {
        /* uart ready, handle input */
        /* TODO: switch to ISR context */
        _native_handle_uart0_input();
    }
    else if(errno != EINTR) {
        /* select failed for reason other than signal */
        err(1, "lpm_set(): select()");
    }

    /* otherwise select was interrupted because of a signal, continue below */
#else
    pause();
#endif

    if(_native_sigpend > 0) {
        DEBUG("\n\n\t\treturn from syscall, calling native_irq_handler\n\n");
        _native_in_syscall = 0;
        _native_in_isr = 1;
        swapcontext(_native_cur_ctx, _native_isr_ctx);
    }
}

/**
 * LPM_IDLE uses sleep() to wait for interrupts
 * LPM_OFF exits process
 * other modes not supported at the moment
 */
enum lpm_mode lpm_set(enum lpm_mode target)
{
    enum lpm_mode last_lpm;

    //DEBUG("lpm_set(%i)\n", target);

    last_lpm = native_lpm;
    native_lpm = target;

    switch(native_lpm) { /* @contiki :-p  */
        case LPM_ON:
            break;

        case LPM_IDLE:
            //DEBUG("lpm_set(): pause()\n");

            _native_in_syscall = 1;
            //pause();
            _native_lpm_sleep();
            break;

        /* XXX: unfinished modes: */
        case LPM_SLEEP:
            /*TODO: implement*/
            printf("XXX: lpm_set(): LPM_SLEEP not implemented\n");
            //sigsuspend();

        case LPM_POWERDOWN:
            /*TODO: implement*/
            printf("XXX: lpm_set(): LPM_POWERDOWN not implemented\n");
            //sigsuspend();

        case LPM_OFF:
            printf("lpm_set(): exit()\n");
            exit(0);
            break;

        default:
            DEBUG("XXX: unsupported power mode: %i\n", native_lpm);
            exit(1);
    }

    return last_lpm;
}

void lpm_awake(void)
{
    DEBUG("XXX: lpm_awake()\n");
    native_lpm = LPM_ON;
    return;
}

void lpm_begin_awake(void)
{
    DEBUG("XXX: lpm_begin_awake()\n");
    return;
}

void lpm_end_awake(void)
{
    DEBUG("XXX: lpm_end_awake()\n");
    native_lpm = LPM_ON;
    return;
}

enum lpm_mode lpm_get(void)
{
    return native_lpm;
}
