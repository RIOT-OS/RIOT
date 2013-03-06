/**
 * Native CPU rtc.h implementation
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

#include <time.h>

#include "debug.h"

#include <rtc.h>

static int native_rtc_enabled;

/**
 * @brief Initializes the RTC for calendar mode
 */
void rtc_init(void)
{
    native_rtc_enabled = 0;
    printf("native rtc initialized\n");
}

/**
 * @brief Starts the RTC
 */
void rtc_enable(void)
{
    DEBUG("rtc_enable\n");
    native_rtc_enabled = 1;
}

/**
 * @brief Stops the RTC
 */
void rtc_disable(void)
{
    DEBUG("rtc_disable()\n");
    native_rtc_enabled = 0;
}

/**
 * @brief	Sets the current time in broken down format directly from to RTC
 * @param[in]	localt		Pointer to structure with time to set
 */
void rtc_set_localtime(struct tm* localt)
{
    DEBUG("rtc_set_localtime()\n");
    printf("setting time not supported.");
}

/**
 * @brief	Returns the current time in broken down format directly from the RTC
 * @param[out]	localt		Pointer to structure to receive time
 */
void rtc_get_localtime(struct tm* localt)
{
    time_t t;
    if(native_rtc_enabled == 1) {
        t = time(NULL);
        if (localtime_r(&t, localt) == NULL) {
            err(1, "rtc_get_localtime: localtime_r");
        }
    }
}
