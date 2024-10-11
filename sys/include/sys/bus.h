/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @defgroup    sys_bus System Buses for common events
 * @{
 *
 * @file
 * @brief       This provides System Buses for common events.
 *
 * @warning     Bus Events will be lost if receiver message queue is full.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef SYS_BUS_H
#define SYS_BUS_H

#include <assert.h>
#include "msg_bus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief System Bus types
 */
typedef enum {
#if MODULE_SYS_BUS_POWER
    SYS_BUS_POWER,      /**< Events related to system power */
#endif
    SYS_BUS_NUMOF       /**< Number of enabled system buses */
} sys_bus_t;

/**
 * @brief Power Bus Events
 */
typedef enum {
    /**
     * @brief Supply voltage fallen below threshold
     */
    SYS_BUS_POWER_EVENT_LOW_VOLTAGE,

    /* add more if needed, but not more than 32 */
} sys_bus_power_event_t;

/**
 * @brief The System Bus array - do not use directly
 */
extern msg_bus_t _sys_bus[SYS_BUS_NUMOF];

/**
 * @brief Get a System Bus for a category of events.
 *
 * @param[in] bus           The event category of the the user
 *                          is interested in
 *
 * @return                  The message bus for those events
 */
static inline msg_bus_t *sys_bus_get(sys_bus_t bus)
{
    return &_sys_bus[bus];
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SYS_BUS_H */
