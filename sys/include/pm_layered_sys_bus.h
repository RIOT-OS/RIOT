/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_pm_layered_sys_bus Layered PM Infrastructure with message notification
 * @ingroup     sys
 * @{
 *
 * If you include this header file and have MODULE_PM_LAYERED_SYS_BUS enabled
 * the calls to pm_*() are substituted with their message notification variant,
 * which additionally sends an IPC message on the system power bus so that
 * subscribers can prepare for power saving.
 *
 * If this file is included but MODULE_PM_LAYERED_SYS_BUS is not used
 * the usual pm_layered header is included.
 *
 * @file
 * @brief       Layered low power mode infrastructure with message notification
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef PM_LAYERED_SYS_BUS_H
#define PM_LAYERED_SYS_BUS_H

#include "pm_layered.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_PM_LAYERED_SYS_BUS)

#include "msg_bus.h"
#include "sys/bus.h"

static inline void pm_set_notify_sys_bus(unsigned mode)
{
    if (mode >= PM_NUM_MODES) {
        /* do not notify when power mode is invalid */
        return;
    }
    msg_bus_post_ack(sys_bus_get(SYS_BUS_POWER),
                     SYS_BUS_POWER_EVENT_PM_CHANGED,
                     (void *)(uintptr_t)mode);
    /* zzzzzz ... */
    pm_set(mode);
    /* woke up */
    msg_bus_post_ack(sys_bus_get(SYS_BUS_POWER),
                     SYS_BUS_POWER_EVENT_PM_CHANGED,
                     (void *)(uintptr_t)PM_NUM_MODES);
}

/**
 * @brief   Calls to @ref pm_set() are substituted with their wrapper which
 *          sends a message on the system power bus
 */
#define pm_set pm_set_notify_sys_bus

#endif /* IS_USED(MODULE_PM_LAYERED_SYS_BUS) */

#ifdef __cplusplus
}
#endif

#endif /* PM_LAYERED_SYS_BUS_H */
/** @} */
