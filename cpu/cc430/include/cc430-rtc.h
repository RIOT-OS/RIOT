/*
 * Copyright 2010, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CC430_RTC_H
#define CC430_RTC_H
#include "periph/rtc.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup     cpu_cc430
 */

/**
 * @file
 * @brief   CC430 Real Time Clock
 *
 * @author  Freie Universität Berlin, Computer Systems & Telematics, RIOT
 * @version $Revision $
 */

/**
 * @brief   Mask for RTC alarms
 * @see     ::rtc_set_alarm
 */
typedef enum {
    RTC_ALARM_DISABLED = 0x00,      /**< Alarm disables */
    RTC_ALARM_MIN      = 0x01,      /**< Alarm mask for Minutes */
    RTC_ALARM_HOUR     = 0x02,      /**< Alarm mask for Hours */
    RTC_ALARM_DOW      = 0x04,      /**< Alarm mask for Day of Week */
    RTC_ALARM_DOM      = 0x08       /**< Alarm mask for Day of Month */
} rtc_alarm_mask_t;

/**
 * @brief   Resets any set alarm
 */
void rtc_remove_alarm(void);

#ifdef __cplusplus
}
#endif

#endif /* CC430_RTC_H */
