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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Priority used for
 */
#ifndef UWB_CORE__PRIO
#define UWB_CORE__PRIO            (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Stacksize used for
 */
#ifndef UWB_CORE__STACKSIZE
#define UWB_CORE__STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Setup and run uwb-core thread
 */
void uwb_core_riot_init(void);

/**
 * @brief   Retrieves the default event queue.
 *
 * As there is no default event queue in RIOT, uwb-core will start and
 * handle a default event queue.
 *
 * @return  the default event queue.
 */
event_queue_t *uwb_core_get_eventq(void);

#ifdef __cplusplus
}
#endif

#endif /* UWB_CORE_H */
/** @} */
