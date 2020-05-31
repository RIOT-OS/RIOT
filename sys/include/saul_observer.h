/*
 * Copyright (C) 2020 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_saul_observer SAUL observer
 * @ingroup     sys
 * @brief       Observer for SAUL device changes
 *
 * @see @ref drivers_saul
 *
 * @{
 *
 * @file
 * @brief       SAUL observer interface definition
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef SAUL_OBSERVER_H
#define SAUL_OBSERVER_H

#include <stdint.h>

#include "msg_bus.h"
#include "saul.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

// FIXME
#define SAUL_OBSERVER_STACK_SIZE            (1024)
#define SAUL_OBSERVER_PRIO                  (8)
#define CONFIG_SAUL_OBSERVER_MSG_QUEUE_SIZE (8)

/**
 * @brief   Requests a check_change call from the SAUL obsevrer thread
 */
#define SAUL_OBSERVER_MSG_TYPE_CHECK_DEV  (0x7285)

/**
 * @brief   Bus ID for change notifications
 */
#define SAUL_OBSERVER_MSG_BUS_ID_CHANGE   (0x1)

/**
 * @brief   Initilize the observer thread
 */
void saul_observer_init(void);

/**
 * @brief   Export the SAUL observer bus
 */
extern msg_bus_t saul_observer_bus;

/**
 * @brief   Export the SAUL observer pid
 */
extern kernel_pid_t saul_observer_pid;

/**
 * @brief   Trigger change check of a SAUL device
 *
 * @param[in] dev       pointer to the device's SAUL registry entry
 */
void saul_observer_queue_check(saul_reg_t *dev);

/**
 * @brief   Subscribe to a SAUL device observer
 *
 * @param[in] entry     pointer to the attached message bus entry
 * @param[in] dev       SAUL device to observer
 */
static inline void saul_observer_subscribe(msg_bus_entry_t *entry, const saul_reg_t *dev)
{
    msg_bus_subscribe(entry, dev->id % 32);
}

/**
 * @brief   Unsubscribe from a SAUL device observer
 *
 * @param[in] entry     pointer to the attached message bus entry
 * @param[in] dev       SAUL device to unsubscribe
 */
static inline void saul_observer_unsubscribe(msg_bus_entry_t *entry, const saul_reg_t *dev)
{
    assert(dev->id < 32);
    msg_bus_unsubscribe(entry, dev->id);
}

/**
 * @brief   Check if a recieved change notification is from a certian SAUL device
 *
 * @param[in] dev       SAUL device
 * @param[in] msg       received message
 */
static inline bool saul_observer_dev_has_changed(const saul_reg_t *dev, const msg_t *msg)
{
    return msg_is_from_bus(&saul_observer_bus, msg) && \
           msg_bus_get_type(msg) == (dev->id % 32);
}

#ifdef __cplusplus
}
#endif

#endif /* SAUL_OBSERVER_H */
/** @} */
