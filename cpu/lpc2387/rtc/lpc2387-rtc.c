/******************************************************************************
Copyright 2008-2010, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file is subject to the terms and conditions of the LGPLv2.
See the file LICENSE in the top level directory for more details.
*******************************************************************************/

/**
 * @file
 * @ingroup     lpc2387_rtc
 * @brief       LPC2387 Real-Time-Clock
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @version     $Revision: 2005 $
 *
 * @note        $Id: lpc2387-rtc.c 2005 2010-03-17 10:52:03Z baar $
 */

#include <sys/time.h>
#include <stdint.h>
#include <string.h>

/* cpu */
#include "VIC.h"
#include "lpc2387.h"
#include "lpc2387-rtc.h"
#include "lpm.h"

#define PREINT_RTC  0x000001C8  /* Prescaler value, integer portion, PCLK = 15Mhz */
#define PREFRAC_RTC 0x000061C0  /* Prescaler value, fraction portion, PCLK = 15Mhz */

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   epoch time in hour granularity
 */
static volatile time_t epoch;

/*---------------------------------------------------------------------------*/
/**
 * @brief   Sets the current time in broken down format directly from to RTC
 * @param[in]   localt      Pointer to structure with time to set
 */
void
rtc_set_localtime(struct tm *localt)
{
    if (localt == NULL) {
        return;
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
}
/*---------------------------------------------------------------------------*/
void rtc_set(time_t time)
{
    struct tm *localt;
    localt = localtime(&time);                      /* convert seconds to broken-down time */
    rtc_set_localtime(localt);
    epoch = time - localt->tm_sec - localt->tm_min * 60;
}
/*---------------------------------------------------------------------------*/
//* set clock to start of unix epoch */
void rtc_reset(void)
{
    rtc_set(0);
    epoch = 0;
}
/*---------------------------------------------------------------------------*/
void
rtc_set_alarm(struct tm *localt, enum rtc_alarm_mask mask)
{
    if (localt != NULL) {
        RTC_ALSEC = localt->tm_sec;
        RTC_ALMIN = localt->tm_min;
        RTC_ALHOUR = localt->tm_hour;
        RTC_ALDOM = localt->tm_mday;
        RTC_ALDOW = localt->tm_wday;
        RTC_ALDOY = localt->tm_yday;
        RTC_ALMON = localt->tm_mon + 1;
        RTC_ALYEAR = localt->tm_year;
        RTC_AMR = ~mask;                                            /* set wich alarm fields to check */
        DEBUG("alarm set %2lu.%2lu.%4lu  %2lu:%2lu:%2lu\n",
              RTC_ALDOM, RTC_ALMON, RTC_ALYEAR, RTC_ALHOUR, RTC_ALMIN, RTC_ALSEC);
    }
    else {
        RTC_AMR = 0xff;
    }
}
/*---------------------------------------------------------------------------*/
enum rtc_alarm_mask
rtc_get_alarm(struct tm *localt)
{
    if (localt != NULL) {
        localt->tm_sec = RTC_ALSEC;
        localt->tm_min = RTC_ALMIN;
        localt->tm_hour = RTC_ALHOUR;
        localt->tm_mday = RTC_ALDOM;
        localt->tm_wday = RTC_ALDOW;
        localt->tm_yday = RTC_ALDOY;
        localt->tm_mon = RTC_ALMON - 1;
        localt->tm_year = RTC_ALYEAR;
        localt->tm_isdst = -1; /* not available */
    }

    return (~RTC_AMR) & 0xff;                                       /* return which alarm fields are checked */
}
/*---------------------------------------------------------------------------*/
void RTC_IRQHandler(void) __attribute__((interrupt("IRQ")));
void RTC_IRQHandler(void)
{
    lpm_begin_awake();

    if (RTC_ILR & ILR_RTSSF) {
        /* sub second interrupt (does not need flag-clearing) */

    }
    else if (RTC_ILR & ILR_RTCCIF) {
        /* counter increase interrupt */
        RTC_ILR |= ILR_RTCCIF;
        epoch += 60 * 60;                   /* add 1 hour */

    }
    else if (RTC_ILR & ILR_RTCALF) {
        RTC_ILR |= ILR_RTCALF;
        RTC_AMR = 0xff;                     /* disable alarm irq */
        DEBUG("Ring\n");
        lpm_end_awake();
    }

    VICVectAddr = 0;                        /* Acknowledge Interrupt */
}

/*---------------------------------------------------------------------------*/
void rtc_enable(void)
{
    RTC_ILR = (ILR_RTSSF | ILR_RTCCIF | ILR_RTCALF);    /* clear interrupt flags */
    RTC_CCR |= CCR_CLKEN;                               /* enable clock */
    install_irq(RTC_INT, &RTC_IRQHandler, IRQP_RTC);    /* install interrupt handler */

    time_t now = rtc_time(NULL);
    epoch = now - (now % 3600);
}
/*---------------------------------------------------------------------------*/
void rtc_init(void)
{
    PCONP |= BIT9;
    RTC_AMR = 0xff;                         /* disable alarm irq */
    RTC_CIIR = IMHOUR;                      /* enable increase irq */
    RTC_CISS = 0;                           /* disable subsecond irq */

    INTWAKE |= BIT15;                       /* rtc irq wakes up mcu from power down */

    RTC_CCR = CCR_CLKSRC;                   /* Clock from external 32 kHz Osc. */

    /* initialize clock with valid unix compatible values
     * If RTC_YEAR contains an value larger unix time_t we must reset. */
    if (RTC_YEAR > 2037) {
        rtc_reset();
    }

    DEBUG("%2lu.%2lu.%4lu  %2lu:%2lu:%2lu epoch %lu\n",
          RTC_DOM, RTC_MONTH, RTC_YEAR, RTC_HOUR, RTC_MIN, RTC_SEC,
          epoch);
}
/*---------------------------------------------------------------------------*/
time_t rtc_time(struct timeval *time)
{
    uint32_t sec;
    uint32_t usec;
    uint32_t min;

    usec = (RTC_CTC >> 1);
    sec  = RTC_SEC;
    min = RTC_MIN;

    while (usec != (RTC_CTC >> 1)) {
        usec = (RTC_CTC >> 1);
        sec  = RTC_SEC;
        min = RTC_MIN;
    }

    sec += min * 60;                            /* add number of minutes */
    sec += epoch;                               /* add precalculated epoch in hour granularity */

    if (time != NULL) {
        usec = usec * 15625;
        usec >>= 9;
        time->tv_sec = sec;
        time->tv_usec =  usec;
    }

    return sec;
}
/*---------------------------------------------------------------------------*/
void rtc_disable(void)
{
    RTC_CCR &= ~CCR_CLKEN;  /* disable clock */
    install_irq(RTC_INT, NULL, 0);
    RTC_ILR = 0;
}
/*---------------------------------------------------------------------------*/
void
rtc_get_localtime(struct tm *localt)
{
    if (localt != NULL) {
        localt->tm_sec = RTC_SEC;
        localt->tm_min = RTC_MIN;
        localt->tm_hour = RTC_HOUR;
        localt->tm_mday = RTC_DOM;
        localt->tm_wday = RTC_DOW;
        localt->tm_yday = RTC_DOY;
        localt->tm_mon = RTC_MONTH - 1;
        localt->tm_year = RTC_YEAR;
        localt->tm_isdst = -1; /* not available */
    }
}
/*---------------------------------------------------------------------------*/
void gettimeofday_r(struct _reent *r, struct timeval *ptimeval, struct timezone *ptimezone)
{
    (void) ptimezone; /* unused */

    r->_errno = 0;

    if (ptimeval != NULL) {
        rtc_time(ptimeval);
    }
}
