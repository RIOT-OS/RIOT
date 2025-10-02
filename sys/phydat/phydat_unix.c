/*
 * Copyright (C) 2023 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "macros/math.h"
#include "container.h"
#include "phydat.h"

/**
 * Offsets of the first day of the month starting with January.
 * Months after February have a negative offset to efficiently handle leap years.
 */
static int16_t month_to_yday[] = { 0,      31, -306, -275, -245, -214,
                                   -184, -153, -122,  -92,  -61,  -31 };

static int16_t phydat_unscale(int16_t value, int16_t scale)
{
    static const int16_t tenmap[] = {10, 100, 1000, 10000 };

    if (value == 0) {
        return 0;
    }

    /* when out of range anyway, we just saturate to max */
    if (scale > (int16_t)ARRAY_SIZE(tenmap)) {
        return INT16_MAX;
    }

    /* same, but lower end of the range */
    if (scale < -(int16_t)ARRAY_SIZE(tenmap)) {
        return 0;
    }

    if (scale > 0) {
        size_t idx = scale - 1;
        if (__builtin_mul_overflow(value, tenmap[idx], &value)) {
            /* result would overflow, returning INT16_MAX instead */
            return INT16_MAX;
        }
        return value;
    }

    if (scale < 0) {
        size_t idx = (-scale) - 1;
        return DIV_ROUND(value, tenmap[idx]);
    }

    return value;
}

int64_t phydat_date_time_to_unix(phydat_t *date, phydat_t *time, int32_t offset_seconds)
{
    return phydat_unix(
        phydat_unscale(date->val[2], date->scale),
        phydat_unscale(date->val[1], date->scale),
        phydat_unscale(date->val[0], date->scale),
        phydat_unscale(time->val[2], time->scale),
        phydat_unscale(time->val[1], time->scale),
        phydat_unscale(time->val[0], time->scale),
        offset_seconds);
}

int64_t phydat_unix(int16_t year, int16_t month, int16_t day,
                    int16_t hour, int16_t minute, int16_t second,
                    int32_t offset_seconds)
{
    /* Make the year relative to 1900. */
    /* Add a year for months after Feb to deal with leap years. */
    year += (month > 2 ? 1 : 0) - 1900;

    /* Calculate the day of the year based on the month */
    day += month_to_yday[(month - 1) % 12] - 1;

    /* POSIX calculation of a UNIX timestamp. */
    /* See section 4.16 of The Open Group Base Specifications Issue 7. */
    int16_t leap_days = ((year - 69) / 4) - ((year - 1) / 100) + ((year + 299) / 400);

    return (int64_t)(day + leap_days) * 86400 +
           (int64_t)(year - 70) * 31536000 +
           (int64_t)(hour) * 3600 +
           (int64_t)(minute) * 60 +
           (int64_t)(second) -
           offset_seconds;
}
