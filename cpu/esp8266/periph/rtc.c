/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_rtc
 * @{
 *
 * @file
 * @brief       Low-level RTC driver implementation using ESP8266 SDK
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "cpu.h"
#include "log.h"
#include "periph/rtc.h"

#include "esp_common.h"

#include "sdk/sdk.h"

void rtc_init(void)
{
    /* TODO implement */
    NOT_YET_IMPLEMENTED();
}

void rtc_poweron(void)
{
    /* TODO implement */
    NOT_YET_IMPLEMENTED();
}

void rtc_poweroff(void)
{
    /* TODO implement */
    NOT_YET_IMPLEMENTED();
}

int rtc_set_time(struct tm *ttime)
{
    (void) time;
    /* TODO implement */
    NOT_YET_IMPLEMENTED();
    return -1;
}

int rtc_get_time(struct tm *ttime)
{
    (void) time;
    /* TODO implement */
    NOT_YET_IMPLEMENTED();
    return -1;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    (void) time;
    (void) cb;
    (void) arg;

    /* TODO implement */
    NOT_YET_IMPLEMENTED();
    return -1;
}

int rtc_get_alarm(struct tm *time)
{
    (void) time;

    /* TODO implement */
    NOT_YET_IMPLEMENTED();
    return -1;
}

void rtc_clear_alarm(void)
{
    /* TODO implement */
    NOT_YET_IMPLEMENTED();
}
