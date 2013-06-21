/******************************************************************************
Copyright 2010, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @defgroup rtc Realtime Clock
 * @ingroup drivers
 * @{
 */

#ifndef RTC_H
#define RTC_H

#define RTC_SECOND 10001U

#include <time.h>

/**
 * @brief Initializes the RTC for calendar mode
 */
void rtc_init(void);

/**
 * @brief Starts the RTC
 */
void rtc_enable(void);

/**
 * @brief Stops the RTC
 */
void rtc_disable(void);

/**
 * @brief	Sets the current time in broken down format directly from to RTC
 * @param[in]	localt		Pointer to structure with time to set
 */
void rtc_set_localtime(struct tm *localt);

/**
 * @brief	Returns the current time in broken down format directly from the RTC
 * @param[out]	localt		Pointer to structure to receive time
 */
void rtc_get_localtime(struct tm *localt);

extern int rtc_second_pid;

#endif
/** @} */
