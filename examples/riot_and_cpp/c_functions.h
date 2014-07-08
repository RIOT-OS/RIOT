/*
 * Copyright (C) 2014 Ho Chi Minh city University of Technology (HCMUT)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file        c_functions.h
 * @brief       Definitions for some c functions.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 */

#include <stdio.h>

#ifndef C_FUNCTIONS_H_
#define C_FUNCTIONS_H_

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
 * @param[in]   day
 * @param[in]   month
 * @param[in]   year
 *
 * @return      day of week (0 means Sunday, ... 6 means Saturday)
 */
int day_of_week(int day, int month, int year);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // _C_FUNCTIONS_H
