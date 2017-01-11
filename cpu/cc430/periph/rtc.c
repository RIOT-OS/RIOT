/*
 * Copyright 2010, Freie Universitaet Berlin (FUB).
 * Copyright 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup driver_periph_rtc
 * @file
 * @brief       CC430 real time clock implementation
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <string.h>
#include <legacymsp430.h>
#include "irq.h"
#include "cpu.h"
#include "rtc.h"

/* Alarm callback */
static struct {
    rtc_alarm_cb_t cb;
    void *arg;
} _cc430_rtc_cb;

static struct tm time_to_set;
static int set_time = 0;
kernel_pid_t rtc_second_pid = KERNEL_PID_UNDEF;

static int  _cc430_rtc_init(rtc_t *rtc);
static int  _cc430_rtc_set_time(rtc_t *rtc, const struct tm *localt);
static int  _cc430_rtc_get_time(rtc_t *rtc, struct tm *localt);
static int  _cc430_rtc_set_alarm(rtc_t *rtc, const struct tm *localt, rtc_alarm_cb_t cb, void *arg);
static int  _cc430_rtc_get_alarm(rtc_t *rtc, struct tm *localt);
static void _cc430_rtc_clear_alarm(rtc_t *rtc);

static const rtc_ops_t _cc430_rtc_ops = {
    .init        = _cc430_rtc_init,
    .get_time    = _cc430_rtc_get_time,
    .set_time    = _cc430_rtc_set_time,
    .get_alarm   = _cc430_rtc_get_alarm,
    .set_alarm   = _cc430_rtc_set_alarm,
    .clear_alarm = _cc430_rtc_clear_alarm,
};

rtc_t cpu_rtc = {
    .rtc_op = &_cc430_rtc_ops,
    .name = "cpu",
};

int cc430_rtc_init(rtc_t *rtc)
{
    (void)rtc;
    /* Set to calendar mode */
    RTCCTL1 |= RTCMODE_H;
    /* Set RTC operational */
    RTCCTL1 &= ~RTCHOLD_H;
    return 0;
}

int cc430_rtc_set_time(rtc_t *rtc, const struct tm *localt)
{
    (void)rtc;
    if (localt == NULL) {
        return -1;
    }

    /* copy time to be set */
    memcpy(&time_to_set, localt, sizeof(struct tm));
    set_time = 1;
    return 0;
}

int cc430_rtc_get_time(rtc_t *rtc, struct tm *localt)
{
    (void)rtc;
    uint8_t success = 0;
    uint8_t i;
    uint16_t tmpyear;

    if (localt == NULL) {
        return -1;
    }

    while (!success) {
        for (i = 0; i < 8; i++) {
            /* try again when RTC is in transition */
            if (!(RTCCTL1 & RTCRDY_H)) {
                break;
            }

            switch(i) {
                case 0:
                    localt->tm_sec = RTCSEC;
                    break;

                case 1:
                    localt->tm_min = RTCMIN;
                    break;

                case 2:
                    localt->tm_hour = RTCHOUR;
                    break;

                case 3:
                    localt->tm_mday = RTCDAY;
                    break;

                case 4:
                    localt->tm_wday = RTCDOW;
                    break;

                case 5:
                    localt->tm_mon = RTCMON - 1;
                    break;

                case 6:
                    tmpyear = RTCYEARL;
                    tmpyear |= (RTCYEARH << 0x08);
                    localt->tm_year = tmpyear - 1900;
                    break;

                default:
                    success = 1;
                    break;
            }
        }
    }

    return 0;
}

int cc430_rtc_set_alarm(rtc_t *rtc, const struct tm *localt, rtc_alarm_cb_t cb, void *arg)
{
    (void)rtc;
    if (cb == NULL) {
        return -1;
    }

    if (localt == NULL) {
        return -2;
    }

    RTCAMIN = localt->tm_min;
    RTCAMIN |= BIT7;
    RTCAHOUR = localt->tm_hour;
    RTCAHOUR |= BIT7;
    RTCADOW = localt->tm_wday;
    RTCADOW |= BIT7;
    RTCADAY = localt->tm_mday;
    RTCADAY |= BIT7;

    RTCCTL0 |= RTCAIE;

    _cc430_rtc_cb.cb = cb;
    _cc430_rtc_cb.arg = arg;

    return 0;
}

int cc430_rtc_get_alarm(rtc_t *rtc, struct tm *localt)
{
    (void)rtc;
    if (localt == NULL) {
        return -1;
    }

    localt->tm_sec = -1;
    localt->tm_min = RTCAMIN;
    localt->tm_hour = RTCAHOUR;
    localt->tm_mday = -1;
    localt->tm_wday = RTCADOW;
    localt->tm_yday = -1;
    localt->tm_mon = - 1;
    localt->tm_year = -1;
    localt->tm_isdst = -1; /* not available */

    return 0;
}

void cc430_rtc_clear_alarm(rtc_t *rtc)
{
    (void)rtc;
    /* reset all AE bits */
    RTCAHOUR &= ~BIT7;
    RTCAMIN  &= ~BIT7;
    RTCADAY  &= ~BIT7;
    RTCADOW  &= ~BIT7;

    /* reset alarm interrupt enable */
    RTCCTL0 &= ~RTCAIE;
    _cc430_rtc_cb.cb = NULL;
    _cc430_rtc_cb.arg = NULL;
}

interrupt(RTC_VECTOR) __attribute__((naked)) rtc_isr(void)
{
    __enter_isr();

    /* RTC is save to write for up to one second now */
    if (RTCIV == RTC_RTCRDYIFG) {
        /* disable interrupt */
        /* RTCCTL0 &= ~RTCRDYIE; */

        if (set_time) {
            set_time = 0;
            /* set previous set time and reset it */
            RTCSEC = time_to_set.tm_sec;
            RTCMIN = time_to_set.tm_min;
            RTCHOUR = time_to_set.tm_hour;
            RTCDAY = time_to_set.tm_mday;
            RTCDOW = time_to_set.tm_wday;
            RTCMON = time_to_set.tm_mon + 1;
            RTCYEARL = (time_to_set.tm_year + 1900) & 0xFF;
            RTCYEARH = (time_to_set.tm_year + 1900) >> 0x08;
        }

        if (rtc_second_pid != KERNEL_PID_UNDEF) {
            static msg_t m;
            m.type = RTCSEC;
            msg_send_int(&m, rtc_second_pid);
        }
    }
    /* RTC alarm */
    else if (RTCIV == RTC_RTCAIFG) {
        if (_cc430_rtc_cb.cb) {
            _cc430_rtc_cb.cb(_cc430_rtc_cb.arg);
        }
    }

    __exit_isr();
}
