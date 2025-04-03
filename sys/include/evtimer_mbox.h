/*
 * Copyright (C) 2020 Simon Brummer <simon.brummer@posteo.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef EVTIMER_MBOX_H
#define EVTIMER_MBOX_H
/**
 * @addtogroup  sys_evtimer
 * @{
 *
 * @file
 * @brief       Message box based evtimer event.
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#include "assert.h"
#include "msg.h"
#include "mbox.h"
#include "evtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message box event definition.
 */
typedef struct {
    evtimer_event_t event;   /**< event base class */
    msg_t msg;               /**< message to store in mbox on event */
    mbox_t *mbox;            /**< mbox the IPC message shall be stored */
} evtimer_mbox_event_t;

/**
 * @brief   Adds mbox event to an event timer.
 *
 * @pre @p evtimer is not NULL.
 * @pre @p event is not NULL.
 * @pre @p mbox is not NULL.
 *
 * @param[in] evtimer   Timer to add @p event.
 * @param[in] event     Event to add.
 * @param[in] mbox      Mbox to store event->msg timer expiration.
 */
static inline void evtimer_add_mbox(evtimer_t *evtimer, evtimer_mbox_event_t *event, mbox_t *mbox)
{
    assert(evtimer);
    assert(event);
    assert(mbox);
    event->mbox = mbox;
    evtimer_add(evtimer, &(event->event));
}

/**
 * @brief   Event handler for mbox events.
 *
 * @pre @p event is not NULL.
 *
 * @param[in] event   The event to handle
 */
static inline void _evtimer_mbox_handler(evtimer_event_t *event)
{
    assert(event);
    evtimer_mbox_event_t *mbevent = (evtimer_mbox_event_t *)event;
    mbox_try_put(mbevent->mbox, &(mbevent->msg));
}

/**
 * @brief   Initializes event timer for mbox events.
 *
 * @pre @p evtimer is not NULL.
 *
 * @param[in] evtimer   An event timer
 */
static inline void evtimer_init_mbox(evtimer_t *evtimer)
{
    assert(evtimer);
    evtimer_init(evtimer, _evtimer_mbox_handler);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* EVTIMER_MBOX_H */
