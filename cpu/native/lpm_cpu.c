/**
 * Native CPU lpm.h implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpm.h"
#include "debug.h"

static enum lpm_mode native_lpm;

void lpm_init(void)
{
    DEBUG("lpm_init()\n");
    native_lpm = LPM_ON;
    return;
}

enum lpm_mode lpm_set(enum lpm_mode target)
{
    enum lpm_mode last_lpm;

    DEBUG("lpm_set(%i)\n", target);

    last_lpm = native_lpm;
    native_lpm = target;

    switch(native_lpm) /* @contiki :-p  */
    {
        case LPM_ON:
            break;

        case LPM_IDLE:
            DEBUG("lpm_set(): pause()\n");
            pause();
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
