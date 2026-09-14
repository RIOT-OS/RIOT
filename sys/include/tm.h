/*
 * SPDX-FileCopyrightText: 2014 René Kijewski
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  sys_timex
 * @{
 *
 * @file
 * @brief       Utility library for `struct tm`.
 */

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TM_WDAY_SUN (0) /**< Sunday in `struct tm::tm_wday`. */
#define TM_WDAY_MON (1) /**< Monday in `struct tm::tm_wday`. */
#define TM_WDAY_TUE (2) /**< Tuesday in `struct tm::tm_wday`. */
#define TM_WDAY_WED (3) /**< Wednesday in `struct tm::tm_wday`. */
#define TM_WDAY_THU (4) /**< Thursday in `struct tm::tm_wday`. */
#define TM_WDAY_FRI (5) /**< Friday in `struct tm::tm_wday`. */
#define TM_WDAY_SAT (6) /**< Saturday in `struct tm::tm_wday`. */

#define TM_MON_JAN ( 0) /**< January in `struct tm::tm_mon` */
#define TM_MON_FEB ( 1) /**< February in `struct tm::tm_mon` */
#define TM_MON_MAR ( 2) /**< March in `struct tm::tm_mon` */
#define TM_MON_APR ( 3) /**< April in `struct tm::tm_mon` */
#define TM_MON_MAY ( 4) /**< May in `struct tm::tm_mon` */
#define TM_MON_JUN ( 5) /**< June in `struct tm::tm_mon` */
#define TM_MON_JUL ( 6) /**< July in `struct tm::tm_mon` */
#define TM_MON_AUG ( 7) /**< August in `struct tm::tm_mon` */
#define TM_MON_SEP ( 8) /**< September in `struct tm::tm_mon` */
#define TM_MON_OCT ( 9) /**< October in `struct tm::tm_mon` */
#define TM_MON_NOV (10) /**< November in `struct tm::tm_mon` */
#define TM_MON_DEC (11) /**< December in `struct tm::tm_mon` */

/**
 * @brief           The number of days in common years.
 * @see             http://oeis.org/A008685
 */
extern const int8_t TM_MON_DAYS[12];

/**
 * @brief           The prefixsum of the number of days in common years.
 * @see             http://oeis.org/A061251
 */
extern const int16_t TM_MON_DAYS_ACCU[12];

/**
 * @brief           Tells if a given year is a leap year in the Gregorian calendar.
 * @param[in]       year   The year. Probably should be ≥ 1582, but needs to be ≥ 1.
 * @returns         `1` if it is a leap year, `0` if it is a common year.
 */
int tm_is_leap_year(unsigned year);

/**
 * @brief           Returns the congruent weekday of the Doomsday (March 0).
 * @details         Only applies for years in the Gregorian calendar.
 * @param[in]       year   The year. Probably should be ≥ 1582, but needs to be ≥ 1.
 * @returns         The result `% 7` is the weekday of the Doomsday of the given year.
 */
int tm_doomsday(int year);

/**
 * @brief           Calculates the day of the year and the weekday of a given date.
 * @details         Illegal dates are not caught.
 * @param[in]       year   The year. Probably should be ≥ 1582, but needs to be ≥ 1.
 * @param[in]       mon    The month, TM_MON_JAN to TM_MON_DEC.
 * @param[in]       mday   The day in the month, 1 to 31.
 * @param[out]      wday   Returns the day of the week.
 * @param[out]      yday   Returns the day of the year (Jan 1st is 0).
 */
#ifndef ATMEGA_INCOMPATIBLE_TIME_H
void tm_get_wyday(int year, int mon, int mday, int *wday, int *yday);
#else
void tm_get_wyday(int year, int mon, int mday, int8_t *wday, int16_t *yday);
#endif

/**
 * @brief               Fills in `struct tm::tm_wday` and `struct tm::tm_yday` given a date.
 * @details             `struct tm::tm_year`, `struct tm::tm_mon`, and `struct
 *                      tm::tm_mday` need to be set before you call this
 *                      function.
 * @param[in,out] tm    The datum to operate on.
 */
void tm_fill_derived_values(struct tm *tm);

/**
 * @brief           Tests if a date is valid.
 * @details         Dates before 1582-10-15 are invalid.
 * @param[in]       year   The year.
 * @param[in]       mon    The month.
 * @param[in]       mday   The day in the month.
 * @returns         0 if the date is invalid.
 */
int tm_is_valid_date(int year, int mon, int mday);

/**
 * @brief           Shallow test if a time is valid.
 * @details         This function accepts leap seconds at any given time, because the timezone is unknown.
 * @param[in]       hour   The hour.
 * @param[in]       min    The minutes.
 * @param[in]       sec    The seconds.
 * @returns         0 if the time is invalid.
 */
int tm_is_valid_time(int hour, int min, int sec);

#ifdef __cplusplus
}
#endif

/** @} */
