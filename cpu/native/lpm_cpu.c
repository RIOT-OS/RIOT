/**
 * Native CPU lpm.h implementation
 *
 * Uses system calls to emulate CPU power modes.
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include <err.h>

#include "lpm.h"
#include "debug.h"
#include "cpu.h"

#include "native_internal.h"

static enum lpm_mode native_lpm;

void lpm_init(void)
{
    DEBUG("lpm_init()\n");
    native_lpm = LPM_ON;
    return;
}

void _native_lpm_sleep(void)
{
    _native_in_syscall++; // no switching here
    real_pause();
    _native_in_syscall--;

    if (_native_sigpend > 0) {
        DEBUG("\n\n\t\treturn from syscall, calling native_irq_handler\n\n");
        _native_in_syscall++;
        _native_syscall_leave();
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
            real_exit(EXIT_SUCCESS);

        default:
            DEBUG("XXX: unsupported power mode: %i\n", native_lpm);
            real_exit(EXIT_FAILURE);
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
