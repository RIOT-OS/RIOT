/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython time module port-specific helpers for RIOT
 *
 * Included by extmod/modtime.c via MICROPY_PY_TIME_INCLUDEFILE when
 * MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME and MICROPY_PY_TIME_TIME_TIME_NS
 * are enabled. Uses POSIX time() function to get the current time in seconds.
 *
 * @}
 */

#include <time.h>

#include "py/obj.h"
#include "shared/timeutils/timeutils.h"

/**
 * @brief   Get the current time in seconds since the epoch and convert it to a
 * struct_time.
 *
 * @param[out]  tm      Pointer to a timeutils_struct_time_t structure to store
 * the result.
 */
static void mp_time_localtime_get(timeutils_struct_time_t *tm)
{
    time_t seconds = time(NULL);

    timeutils_seconds_since_epoch_to_struct_time((mp_timestamp_t)seconds, tm);
}

/**
 * @brief   Get the current time in seconds since the epoch as a MicroPython
 *          object.
 *
 * @return  A MicroPython integer object representing the current time in
 *          seconds
 */
static mp_obj_t mp_time_time_get(void)
{
    return mp_obj_new_int_from_ull((mp_uint_t)time(NULL));
}
