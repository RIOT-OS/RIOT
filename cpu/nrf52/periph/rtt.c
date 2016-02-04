/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral real-time timer interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/rtt.h"

/*
 * @brief Callback for the active alarm
 */
static rtt_cb_t alarm_cb;

/**
 * @brief Argument for the active alarm callback
 */
static void *alarm_arg;

/**
 * @brief Callback for the overflow event
 */
static rtt_cb_t overflow_cb;

/**
 * @brief Argument for the overflow callback
 */
static void *overflow_arg;

void rtt_init(void)
{
    /* configure interrupt */
    NVIC_EnableIRQ(RTT_IRQ);
    /* set prescaler */
    RTT_DEV->PRESCALER = RTT_PRESCALER;
    /* enable the low-frequency clock */
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    /* start the actual RTT thing */
    RTT_DEV->TASKS_START = 1;
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    overflow_cb = cb;
    overflow_arg = arg;
    RTT_DEV->INTENSET = RTC_INTENSET_OVRFLW_Msk;
}

void rtt_clear_overflow_cb(void)
{
    RTT_DEV->INTENCLR = RTC_INTENCLR_OVRFLW_Msk;
}

uint32_t rtt_get_counter(void)
{
    return RTT_DEV->COUNTER;
}

void rtt_set_counter(uint32_t counter)
{
    (void) counter;
    /* not supported by the nRF52 */
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    alarm_cb = cb;
    alarm_arg = arg;
    RTT_DEV->CC[0] = (alarm & RTT_MAX_VALUE);
    RTT_DEV->INTENSET = RTC_INTENSET_COMPARE0_Msk;
}

uint32_t rtt_get_alarm(void)
{
    return RTT_DEV->CC[0];
}

void rtt_clear_alarm(void)
{
    RTT_DEV->INTENCLR = RTC_INTENSET_COMPARE0_Msk;
}

void rtt_poweron(void)
{
    RTT_DEV->TASKS_START = 1;
}

void rtt_poweroff(void)
{
    RTT_DEV->TASKS_STOP = 1;
}

void RTT_ISR(void)
{
    if (RTT_DEV->EVENTS_COMPARE[0] == 1) {
        RTT_DEV->EVENTS_COMPARE[0] = 0;
        RTT_DEV->INTENCLR = RTC_INTENSET_COMPARE0_Msk;
        alarm_cb(alarm_arg);
    }

    if (RTT_DEV->EVENTS_OVRFLW == 1) {
        RTT_DEV->EVENTS_OVRFLW = 0;
        overflow_cb(overflow_arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
