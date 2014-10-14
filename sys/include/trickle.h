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
 * @file    trickle.h
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

#define TRICKLE_INTERVAL_STACKSIZE (KERNEL_CONF_STACKSIZE_MAIN)

typedef struct {
    void (*func)(void *);
    void *args;
} trickle_callback_t;

typedef struct {
    uint8_t k;
    uint32_t Imin;
    uint8_t Imax;
    uint32_t I;
    uint32_t t;
    uint16_t c;
    uint8_t done    :1;
    uint8_t reset   :1;
    uint8_t padding :6;
    timex_t wakeup_time;
    vtimer_t wakeup_timer;
    kernel_pid_t thread_pid;
    trickle_callback_t callback;
    char *thread_buf;
} trickle_t;

void reset_trickletimer(trickle_t *trickle);
void start_trickle(trickle_t *trickle, uint8_t DIOINtMin, uint8_t DIOIntDoubl, uint8_t DIORedundancyConstatnt);
void stop_trickle(trickle_t *trickle);
void trickle_increment_counter(trickle_t *trickle);

#ifdef __cplusplus
}
#endif

#endif /* _TRICKLE_H */
/** @} */
