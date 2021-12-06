/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef UWB_CORE_H
#define UWB_CORE_H

#include <stdint.h>
#include "event.h"
#if IS_USED(MODULE_UWB_CORE_EVENT_THREAD)
#include "event/thread.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The event queue if uwb_core_event_thread is used
 */
#ifndef UWB_CORE_EVENT_THREAD_QUEUE
#define UWB_CORE_EVENT_THREAD_QUEUE     EVENT_PRIO_MEDIUM
#endif

/**
 * @brief   Priority used for uwb-core event queue
 */
#ifndef UWB_CORE_PRIO
#define UWB_CORE_PRIO                   (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Stacksize used for uwb-core event queue
 */
#ifndef UWB_CORE_STACKSIZE
#define UWB_CORE_STACKSIZE              (THREAD_STACKSIZE_LARGE)
#endif

/**
 * @brief   Setup and run uwb-core thread
 */
void uwb_core_riot_init(void);

/**
 * @brief   Retrieves the default event queue.
 *
 * if uwb_core_event_thread is used then the event_thread module queue will be
 * used. Otherwise uwb-core will start and handle its own event queue.
 *
 * @return  the default event queue.
 */
event_queue_t *uwb_core_get_eventq(void);

#ifdef __cplusplus
}
#endif

#endif /* UWB_CORE_H */
/** @} */
