/*
 * Trickle constants and prototypes
 *
 * Copyright (C) 2013, 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup sys_trickle Trickle Timer
 * @ingroup sys
 * @{
 */

/**
 * @file
 * @brief   Implementation of a generic Trickle Algorithm (RFC 6206)
 *
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#ifndef _TRICKLE_H
#define _TRICKLE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "vtimer.h"
#include "thread.h"

/** @brief a generic callback function with arguments that is called by trickle periodically */
typedef struct {
    void (*func)(void *);       /**< a generic callback function pointer */
    void *args;                 /**< a generic parameter for the callback function pointer */
} trickle_callback_t;

/** @brief all state variables for a trickle timer */
typedef struct {
    uint8_t k;                      /**< redundancy constant */
    uint32_t Imin;                  /**< minimum interval size */
    uint8_t Imax;                   /**< maximum interval size, described as a number of doublings */
    uint32_t I;                     /**< current interval size */
    uint32_t t;                     /**< time within the current interval */
    uint16_t c;                     /**< counter */
    kernel_pid_t pid;               /**< pid of trickles target thread */
    trickle_callback_t callback;    /**< the callback function and parameter that trickle is calling
                                         after each interval */
    uint16_t interval_msg_type;     /**< the msg_t.type that trickle should use after an interval */
    timex_t msg_interval_time;      /**< interval represented as timex_t */
    vtimer_t msg_interval_timer;    /**< vtimer to send a msg_t to the target thread for a new interval */
    uint16_t callback_msg_type;     /**< the msg_t.type that trickle should use after a callback */
    timex_t msg_callback_time;      /**< callback interval represented as timex_t */
    vtimer_t msg_callback_timer;    /**< vtimer to send a msg_t to the target thread for a callback */
} trickle_t;

/**
 * @brief resets the trickle timer
 *
 * @param[in] trickle   the trickle timer
 */
void trickle_reset_timer(trickle_t *trickle);

/**
 * @brief start the trickle timer
 *
 * @param[in] pid                   target thread
 * @param[in] trickle               trickle timer
 * @param[in] interval_msg_type     msg_t.type for interval messages
 * @param[in] callback_msg_type     msg_t.type for callback messages
 * @param[in] Imin                  minimum interval
 * @param[in] Imax                  maximum interval
 * @param[in] k                     redundancy constant
 */
void trickle_start(kernel_pid_t pid, trickle_t *trickle, uint16_t interval_msg_type,
        uint16_t callback_msg_type, uint32_t Imin, uint8_t Imax, uint8_t k);

/**
 * @brief stops the trickle timer
 *
 * @param[in] trickle   trickle timer
 */
void trickle_stop(trickle_t *trickle);

/**
 * @brief increments the counter by one
 *
 * @param[in] trickle   trickle timer
 */
void trickle_increment_counter(trickle_t *trickle);

/**
 * @brief is called after the interval is over and calculates the next interval
 *
 * @param[in] trickle   trickle timer
 */
void trickle_interval(trickle_t *trickle);

/**
 * @brief is called after the callback interval is over and calls the callback function
 *
 * @param[in] trickle   trickle timer
 */
void trickle_callback(trickle_t *trickle);

#ifdef __cplusplus
}
#endif

#endif /* _TRICKLE_H */
/** @} */
