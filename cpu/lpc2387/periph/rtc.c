/*
 * Copyright 2008-2010, Freie Universitaet Berlin (FUB). All rights reserved.
 * Copyright 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <sys/time.h>
#include <stdint.h>
#include <string.h>

#include "kernel_types.h"

/* cpu */
#include "rtc.h"
#include "VIC.h"
#include "lpc2387.h"
#include "lpm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Alarm callback */
static struct {
    rtc_alarm_cb_t cb;
    void *arg;
} _lpc2387_rtc_cb;

/* internal function to set time based on time_t */
static void _rtc_set(time_t time);

void RTC_IRQHandler(void) __attribute__((interrupt("IRQ")));

static int  _lpc2387_rtc_init(rtc_t *rtc);
static int  _lpc2387_rtc_set_time(rtc_t *rtc, const struct tm *localt);
static int  _lpc2387_rtc_get_time(rtc_t *rtc, struct tm *localt);
static int  _lpc2387_rtc_set_alarm(rtc_t *rtc, const struct tm *localt, rtc_alarm_cb_t cb, void *arg);
static int  _lpc2387_rtc_get_alarm(rtc_t *rtc, struct tm *localt);
static void _lpc2387_rtc_clear_alarm(rtc_t *rtc);

static const rtc_ops_t _lpc2387_rtc_ops = {
    .init        = _lpc2387_rtc_init,
    .get_time    = _lpc2387_rtc_get_time,
    .set_time    = _lpc2387_rtc_set_time,
    .get_alarm   = _lpc2387_rtc_get_alarm,
    .set_alarm   = _lpc2387_rtc_set_alarm,
    .clear_alarm = _lpc2387_rtc_clear_alarm,
};

rtc_t cpu_rtc = {
    .rtc_op = &_lpc2387_rtc_ops,
    .name = "cpu",
};

int _lpc2387_rtc_init(rtc_t *rtc)
{
    (void)rtc;
    PCONP |= BIT9;
    RTC_AMR = 0xff;                         /* disable alarm irq */
    RTC_CIIR = 0;                           /* disable increase irq */
    RTC_CISS = 0;                           /* disable subsecond irq */

    INTWAKE |= BIT15;                       /* rtc irq wakes up mcu from power down */

    RTC_CCR = CCR_CLKSRC;                   /* Clock from external 32 kHz Osc. */

    /* initialize clock with valid unix compatible values
     * If RTC_YEAR contains an value larger unix time_t we must reset. */
    if (RTC_YEAR > 2037) {
        _rtc_set(0);
    }

    DEBUG("%2lu.%2lu.%4lu  %2lu:%2lu:%2lu\n",
            RTC_DOM, RTC_MONTH, RTC_YEAR, RTC_HOUR, RTC_MIN, RTC_SEC);
    return 0;
}

/**
 * @brief   Sets the current time in broken down format directly from to RTC
 * @param[in]   localt      Pointer to structure with time to set
 */
int _lpc2387_rtc_set_time(rtc_t *rtc, const struct tm *localt)
{
    (void)rtc;
    if (localt == NULL) {
        return -1;
    }

    /* set clock */
    RTC_SEC = localt->tm_sec;
    RTC_MIN = localt->tm_min;
    RTC_HOUR = localt->tm_hour;
    RTC_DOM = localt->tm_mday;
    RTC_DOW = localt->tm_wday;
    RTC_DOY = localt->tm_yday;
    RTC_MONTH = localt->tm_mon + 1;
    RTC_YEAR = localt->tm_year;

    return 0;
}

int _lpc2387_rtc_get_time(rtc_t *rtc, struct tm *localt)
{
    (void)rtc;
    if (localt == NULL) {
        return -1;
    }

    localt->tm_sec = RTC_SEC;
    localt->tm_min = RTC_MIN;
    localt->tm_hour = RTC_HOUR;
    localt->tm_mday = RTC_DOM;
    localt->tm_wday = RTC_DOW;
    localt->tm_yday = RTC_DOY;
    localt->tm_mon = RTC_MONTH - 1;
    localt->tm_year = RTC_YEAR;
    localt->tm_isdst = -1; /* not available */

    return 0;
}


int _lpc2387_rtc_set_alarm(rtc_t *rtc, const struct tm *localt, rtc_alarm_cb_t cb, void *arg)
{
    (void)rtc;
    if (cb == NULL) {
        return -1;
    }
    if (localt == NULL) {
        return -2;
    }
    RTC_ALSEC = localt->tm_sec;
    RTC_ALMIN = localt->tm_min;
    RTC_ALHOUR = localt->tm_hour;
    RTC_ALDOM = localt->tm_mday;
    RTC_ALDOW = localt->tm_wday;
    RTC_ALDOY = localt->tm_yday;
    RTC_ALMON = localt->tm_mon + 1;
    RTC_ALYEAR = localt->tm_year;
    RTC_AMR = 0;                                            /* set wich alarm fields to check */
    DEBUG("alarm set %2lu.%2lu.%4lu  %2lu:%2lu:%2lu\n",
          RTC_ALDOM, RTC_ALMON, RTC_ALYEAR, RTC_ALHOUR, RTC_ALMIN, RTC_ALSEC);
    _lpc2387_rtc_cb.cb = cb;
    _lpc2387_rtc_cb.arg = arg;

    return 0;
}

int _lpc2387_rtc_get_alarm(rtc_t *rtc, struct tm *localt)
{
    (void)rtc;
    if (localt == NULL) {
        return -1;
    }

    localt->tm_sec = RTC_ALSEC;
    localt->tm_min = RTC_ALMIN;
    localt->tm_hour = RTC_ALHOUR;
    localt->tm_mday = RTC_ALDOM;
    localt->tm_wday = RTC_ALDOW;
    localt->tm_yday = RTC_ALDOY;
    localt->tm_mon = RTC_ALMON - 1;
    localt->tm_year = RTC_ALYEAR;
    localt->tm_isdst = -1; /* not available */

    return 0;
}

void _lpc2387_rtc_clear_alarm(rtc_t *rtc)
{
    (void)rtc;
    RTC_AMR = 0xff;
}

void RTC_IRQHandler(void)
{
    lpm_begin_awake();

    if (RTC_ILR & ILR_RTSSF) {
        /* sub second interrupt (does not need flag-clearing) */
    }
    else if (RTC_ILR & ILR_RTCCIF) {
        /* counter increase interrupt */
    }
    else if (RTC_ILR & ILR_RTCALF) {
        RTC_ILR |= ILR_RTCALF;
        RTC_AMR = 0xff;                     /* disable alarm irq */
        if (_lpc2387_rtc_cb.cb) {
            _lpc2387_rtc_cb.cb(_lpc2387_rtc_cb.arg);
        }
        DEBUG("Ring\n");
        lpm_end_awake();
    }

    VICVectAddr = 0;                        /* Acknowledge Interrupt */
}

static void _rtc_set(time_t time)
{
    struct tm *localt;
    localt = localtime(&time);        /* convert seconds to broken-down time */
    rtc_set_time(NULL, localt);
}
