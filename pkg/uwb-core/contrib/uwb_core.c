/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "event/callback.h"
#include "uwb_core.h"

#include "os/os_cputime.h"
#include "hal/hal_timer.h"

#ifndef UWB_CORE_STACKSIZE
#define UWB_CORE_STACKSIZE  (THREAD_STACKSIZE_LARGE)
#endif
#ifndef UWB_CORE_PRIO
#define UWB_CORE_PRIO       (THREAD_PRIORITY_MAIN - 5)
#endif

static char _stack_uwb_core[UWB_CORE_STACKSIZE];

static event_queue_t _queue;

static void *_uwb_core_thread(void *arg)
{
    (void)arg;

    event_queue_init(&_queue);
    event_loop(&_queue);
    /* never reached */
    return NULL;
}

event_queue_t *uwb_core_get_eventq(void)
{
    return &_queue;
}

void uwb_core_riot_init(void)
{
    thread_create(_stack_uwb_core, sizeof(_stack_uwb_core),
                  UWB_CORE_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _uwb_core_thread, NULL,
                  "uwb_core");
}
