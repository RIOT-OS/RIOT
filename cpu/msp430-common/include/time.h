/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_common
 * @{
 *
 * @file
 * @brief       time.h for msp430
 * @see http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/assert.h.html
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef MSPGCC_TIME_H
#define MSPGCC_TIME_H

#include <sys/types.h>
#include "msp430_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Datatype to represent time.
 */
struct tm {
    int tm_sec;     /**< Seconds after the minute [0, 60] */
    int tm_min;     /**< Minutes after the hour [0, 59] */
    int tm_hour;    /**< Hours since midnight [0, 23] */
    int tm_mday;    /**< Day of the month [1, 31] */
    int tm_mon;     /**< Months since January [0, 11] */
    int tm_year;    /**< Years since 1900 */
    int tm_wday;    /**< Days since Sunday [0, 6] */
    int tm_yday;    /**< Days since January 1st [0, 365] */
    int tm_isdst;   /**< Daylight saving time is in effect
                     *   (positive if true, 0 if not, negative if n/a) */
};

#ifdef __cplusplus
}
#endif

#endif
/** @} */
