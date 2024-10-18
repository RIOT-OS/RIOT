/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       Internal globals for RPL
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "evtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event queue for msg events.
 */
extern evtimer_msg_t gnrc_rpl_evtimer;

/**
 * @name Events for RPL.
 * @{
 */
/**
 * @brief   Message type for parent timeouts.
 */
#define GNRC_RPL_MSG_TYPE_PARENT_TIMEOUT      (0x0904)
/**
 * @brief   Message type for instance cleanup.
 */
#define GNRC_RPL_MSG_TYPE_INSTANCE_CLEANUP    (0x0905)
/**
 * @brief   Message type for DAO transmissions.
 */
#define GNRC_RPL_MSG_TYPE_DODAG_DAO_TX        (0x0906)
/** @} */

/**
 * @brief   Interval in milliseconds to probe a parent with DIS messages.
 */
#define GNRC_RPL_PARENT_PROBE_INTERVAL        (2 * MS_PER_SEC)

/**
 * @name State of a RPL parent
 * @{
 */
/**
 * @brief Parent is unused.
 */
#define GNRC_RPL_PARENT_UNUSED      (0)
/**
 * @brief Parent is active.
 */
#define GNRC_RPL_PARENT_ACTIVE      (1)
/**
 * @brief Parent is stale.
 */
#define GNRC_RPL_PARENT_STALE       (2)
/**
 * @brief Parent has timed out.
 */
#define GNRC_RPL_PARENT_TIMEOUT     (GNRC_RPL_PARENT_STALE + \
                                     CONFIG_GNRC_RPL_PARENT_TIMEOUT_DIS_RETRIES)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GLOBALS_H */
