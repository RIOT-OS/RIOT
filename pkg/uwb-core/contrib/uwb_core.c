/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core bootstrapping  functions
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "thread.h"
#include "event.h"
#include "uwb_core.h"

#if !IS_USED(MODULE_UWB_CORE_EVENT_THREAD)
static event_queue_t _queue;
static char _stack_uwb_core[UWB_CORE_STACKSIZE];
static void *_uwb_core_thread(void *arg)
{
    (void)arg;
    event_queue_init(&_queue);
    event_loop(&_queue);
    /* never reached */
    return NULL;
}
#endif

event_queue_t *uwb_core_get_eventq(void)
{
#if !IS_USED(MODULE_UWB_CORE_EVENT_THREAD)
    return &_queue;
#else
    return UWB_CORE_EVENT_THREAD_QUEUE;
#endif
}

void uwb_core_riot_init(void)
{
#if !IS_USED(MODULE_UWB_CORE_EVENT_THREAD)
    thread_create(_stack_uwb_core, sizeof(_stack_uwb_core),
                  UWB_CORE_PRIO,
                  0,
                  _uwb_core_thread, NULL,
                  "uwb_core_event");
#endif
}
