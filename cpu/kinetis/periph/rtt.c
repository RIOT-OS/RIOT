/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_rtt
 *
 * @{
 *
 * @file
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "bit.h"
#include "periph/rtt.h"
#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static rtt_cb_t alarm_cb = NULL;       /**< callback called from RTC alarm */
static void *alarm_arg;                /**< argument passed to the callback */
static uint32_t alarm_value = 0;

static void _rtt_cb(void *arg, int channel)
{
    (void) arg;
    (void) channel;
    if (alarm_cb != NULL) {
        alarm_cb(alarm_arg);
    }
}

void rtt_init(void)
{
    timer_init(RTT_DEV, RTT_FREQUENCY, _rtt_cb, NULL);
    alarm_value = 0;
}

uint32_t rtt_get_counter(void)
{
    return timer_read(RTT_DEV);
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    unsigned state = irq_disable();
    alarm_arg = arg;
    alarm_cb = cb;
    alarm_value = alarm;
    timer_set_absolute(RTT_DEV, 0, alarm % RTT_MAX_VALUE);
    irq_restore(state);
}

uint32_t rtt_get_alarm(void)
{
    return alarm_value;
}

void rtt_clear_alarm(void)
{
    timer_clear(RTT_DEV, 0);
}

void rtt_poweron(void)
{
    timer_start(RTT_DEV);
}

void rtt_poweroff(void)
{
    timer_stop(RTT_DEV);
}
