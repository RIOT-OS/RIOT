/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of struct timeval for the atmega
 *
 */
struct timeval {
    time_t         tv_sec;      /**< seconds */
    suseconds_t    tv_usec;     /**< microseconds */
};

#ifdef __cplusplus
}
#endif
