/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_evtimer Millisecond interval timers
 * @ingroup     sys
 * @brief       Provides timers for events up to 2**32 milliseconds in the future
 *
 * RIOT's main timer subsystem is xtimer, but for many applications xtimer's
 * 64bit absolute time values are wasteful or clumsy to use.
 *
 * Compared to xtimer, evtimer offers:
 *
 * - only relative 32bit millisecond timer values
 *   Events can be scheduled with a relative offset of up to ~49.7 days in the
 *   future.
 *   For time-critical stuff, use xtimer!
 *
 * - more flexible, "intrusive" timer type
 *   evtimer_event_t only contains the necessary fields, which can be extended
 *   as needed, and handlers define actions taken on timer triggers.
 *   Check out evtimer_msg_event as example.
 *
 * - uses xtimer as backend
 *
 * @{
 *
 * @file
 * @brief       evtimer API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef EVTIMER_H
#define EVTIMER_H

#include <stdint.h>

#include "msg.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** forward declaration */
typedef struct evtimer_event evtimer_event_t;

typedef struct {
    xtimer_t timer;
    evtimer_event_t *events;
} evtimer_t;

struct evtimer_event {
    evtimer_event_t *next;
    uint32_t offset;
};

typedef struct {
    evtimer_event_t event;
    msg_t msg;
} evtimer_msg_event_t;

void evtimer_init(evtimer_t *evtimer, void(*handler)(void*));
void evtimer_add(evtimer_t *evtimer, evtimer_event_t *event);
void evtimer_del(evtimer_t *evtimer, evtimer_event_t *event);
void evtimer_msg_handler(void *arg);
void evtimer_print(const evtimer_t *evtimer);

#ifdef __cplusplus
}
#endif

#endif /* EVTIMER_H */
/** @} */
