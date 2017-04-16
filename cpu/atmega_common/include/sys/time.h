/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef ATMEGA_TIME_H
#define ATMEGA_TIME_H

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

#endif /* ATMEGA_TIME_H */
