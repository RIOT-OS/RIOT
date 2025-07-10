/*
 * SPDX-FileCopyrightText: 2014 Ho Chi Minh city University of Technology (HCMUT)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Definitions for some c functions.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 */

#include <stdio.h>

/*
 * all headers defining c functions must be marked as extern "C" when included
 * into c++ sources
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Print a hello message.
 */
int hello(void);

/**
 * @brief   Get day of week from given date (day, month, year >= 1/1/2000).
 *
 * @param[in]   day     day of the date
 * @param[in]   month   month of the date
 * @param[in]   year    year f the date
 *
 * @return      day of week (0 means Sunday, ... 6 means Saturday)
 */
int day_of_week(int day, int month, int year);

#ifdef __cplusplus
}
#endif

/** @} */
