/******************************************************************************
Copyright 2010, Freie Universitaet Berlin (FUB).
Copyright 2013, INRIA.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
*******************************************************************************/

/**
 * @ingroup rtc
 * @file        cc430-rtc.c
 * @brief       CC430 real time clock implementation
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <string.h>
#include <legacymsp430.h>
#include "irq.h"
#include "cpu.h"
#include "cc430-rtc.h"

//static volatile time_t epoch;
static struct tm time_to_set;
static int set_time = 0;
kernel_pid_t rtc_second_pid = KERNEL_PID_UNDEF;

/*---------------------------------------------------------------------------*/
void rtc_init(void)
{
    /* Set to calendar mode */
    RTCCTL1 |= RTCMODE_H;
}

/*---------------------------------------------------------------------------*/
void rtc_enable(void)
{
    /* Set RTC operational */
    RTCCTL1 &= ~RTCHOLD_H;
}
/*---------------------------------------------------------------------------*/
void rtc_disable(void)
{
    /* Stop RTC */
    RTCCTL1 |= RTCHOLD_H;
}
/*---------------------------------------------------------------------------*/
void rtc_set_localtime(struct tm *localt)
{
    if (localt == NULL) {
        return;
    }

    /* copy time to be set */
    memcpy(&time_to_set, localt, sizeof(struct tm));
    set_time = 1;
}

/*---------------------------------------------------------------------------
void rtc_set(time_t time) {
    struct tm* localt;
    localt = localtime(&time);                      // convert seconds to broken-down time
    rtc_set_localtime(localt);
    epoch = time - localt->tm_sec - localt->tm_min * 60;
}
*/

/*---------------------------------------------------------------------------
time_t rtc_time(void) {
    time_t sec;
    struct tm t;
    rtc_get_localtime(&t);
    sec = mktime(&t);
    return sec;
}
*/
/*---------------------------------------------------------------------------*/
void rtc_get_localtime(struct tm *localt)
{
    uint8_t success = 0;
    uint8_t i;
    uint16_t tmpyear;

    if (localt == NULL) {
        return;
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
}

/*---------------------------------------------------------------------------*/
void rtc_set_alarm(struct tm *localt, rtc_alarm_mask_t mask)
{
    if (mask & RTC_ALARM_MIN) {
        RTCAMIN = localt->tm_min;
        RTCAMIN |= BIT7;
    }

    if (mask & RTC_ALARM_HOUR) {
        RTCAHOUR = localt->tm_hour;
        RTCAHOUR |= BIT7;
    }

    if (mask & RTC_ALARM_DOW) {
        RTCADOW = localt->tm_wday;
        RTCADOW |= BIT7;
    }

    if (mask & RTC_ALARM_DOM) {
        RTCADAY = localt->tm_mday;
        RTCADAY |= BIT7;
    }

    RTCCTL0 |= RTCAIE;
}

/*---------------------------------------------------------------------------*/
void rtc_remove_alarm(void)
{
    /* reset all AE bits */
    RTCAHOUR &= ~BIT7;
    RTCAMIN  &= ~BIT7;
    RTCADAY  &= ~BIT7;
    RTCADOW  &= ~BIT7;

    /* reset alarm interrupt enable */
    RTCCTL0 &= ~RTCAIE;
}
/*---------------------------------------------------------------------------*/
interrupt(RTC_VECTOR) __attribute__((naked)) rtc_isr(void)
{
    __enter_isr();

    /* RTC is save to write for up to one second now */
    if (RTCIV == RTC_RTCRDYIFG) {
        /* disable interrupt */
        //RTCCTL0 &= ~RTCRDYIE;

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
            m.type = RTC_SECOND;
            msg_send_int(&m, rtc_second_pid);
        }
    }
    /* RTC alarm */
    else if (RTCIV == RTC_RTCAIFG) {
    }

    __exit_isr();
}
