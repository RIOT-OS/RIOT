/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @file
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#include "tm.h"

#include <stdint.h>

const int8_t TM_MON_DAYS[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

const int16_t TM_MON_DAYS_ACCU[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

int tm_is_leap_year(unsigned year)
{
    return ((year & 3) == 0) && ((year % 400 == 0) || (year % 100 != 0));
}

int tm_doomsday(int year)
{
    int result;
    result  = TM_WDAY_TUE;
    result += year;
    result += year >>= 2;
    result -= year /= 25;
    result += year >>= 2;
    return result;
}

void tm_get_wyday(int year, int mon, int mday, int *wday, int *yday)
{
    int is_leap_year = tm_is_leap_year(year);
    *yday = TM_MON_DAYS_ACCU[mon] + mday + (mon <= TM_MON_FEB ? 0 : is_leap_year) - 1;
    int jan1 = tm_doomsday(year) - 2 - is_leap_year;
    *wday = (jan1 + *yday) % 7;
}

void tm_fill_derived_values(struct tm *tm)
{
    tm_get_wyday(tm->tm_year + 1900, tm->tm_mon, tm->tm_mday, &tm->tm_wday, &tm->tm_yday);
}

int tm_is_valid_date(int year, int mon, int mday)
{
    if ((mon < TM_MON_JAN) || (mon > TM_MON_DEC)) {
        return 0;
    }
    if ((mday <= 0) || (mday > TM_MON_DAYS[mon])) {
        if ((mday != 29) || (mon != TM_MON_FEB) || !tm_is_leap_year(year)) {
            return 0;
        }
    }
    if (year <= 1582) {
        if (year < 1582) {
            return 0;
        }
        if ((mon < TM_MON_OCT) || ((mon == TM_MON_OCT) && (mday < 15))) {
            return 0;
        }
    }

    return 1;
}

int tm_is_valid_time(int hour, int min, int sec)
{
    return (hour >= 0) && (hour < 24) &&
           (min >= 0) && (min < 60) &&
           (sec >= 0) && (sec <= 60);
}
