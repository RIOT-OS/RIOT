/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef LPC2387_RTC_H
#define LPC2387_RTC_H

/**
 * @defgroup    lpc2387_rtc     LPC2387 Real-Time-Clock
 * @ingroup     lpc2387
 *
 * \section lpc2387_rtc_newlib  Standard library support
 * Currently reading and setting time and date through standard C functions is implemented.
 * Standard C timers are not available.
 *
 * @{
 */

/**
 * @file
 * @brief       LPC2387 Real-Time-Clock
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @version     $Revision: 1998 $
 *
 * @note        $Id: lpc2387-rtc.h 1998 2010-03-16 13:05:41Z baar $
 */

#include <time.h>
#include <sys/time.h>
#include "periph/rtc.h"
#include "lpc2387.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- */
/**
 * @name LPC2387 RTC Compile-Time Configuration
 * @{
 */

/** @} */

/**
 * @brief   Mask for RTC alarms
 * @see     ::rtc_set_alarm, ::rtc_get_alarm
 */
enum rtc_alarm_mask {
    RTC_AMR_DISABLED = 0,       ///< Alarm disables
    RTC_AMR_SEC  =  AMRSEC,     ///< Alarm mask for Seconds
    RTC_AMR_MIN  =  AMRMIN,     ///< Alarm mask for Minutes
    RTC_AMR_HOUR =  AMRHOUR,    ///< Alarm mask for Hours
    RTC_AMR_DOM  =  AMRDOM,     ///< Alarm mask for Day of Month
    RTC_AMR_DOW  =  AMRDOW,     ///< Alarm mask for Day of Week
    RTC_AMR_DOY  =  AMRDOY,     ///< Alarm mask for Day of Year
    RTC_AMR_MON  =  AMRMON,     ///< Alarm mask for Month
    RTC_AMR_YEAR =  AMRYEAR,    ///< Alarm mask for Year
};

/**
 * @brief   Returns the current clock time
 * @param[out]  time        optional return value
 * @return  clock time in seconds
 */
time_t rtc_time(struct timeval *time);

/**
 * @brief Set clock to start of unix epoch
 */
void rtc_reset(void);

/**
 * @brief   Sets the current clock time
 * @param[in]   time        new time in seconds
 * @note    Any set alarm is shifted
 */
void rtc_set(time_t time);

/** @} */
#endif /* end __RTC_H */
