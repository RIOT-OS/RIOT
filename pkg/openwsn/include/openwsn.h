/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_openwsn    OpenWSN
 * @ingroup     pkg
 * @brief       An IoT Network Stack Umplementing 6TiSCH
 * @see         https://github.com/openwsn-berkeley/openwsn-fw
 *
 *
 * @{
 *
 * @file
 * @brief   OpenWSN bootstrap definitions
 *
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */
#ifndef OPENWSN_H
#define OPENWSN_H

#ifdef __cplusplus
extern "C" {
#endif

#define OW_SCHED_MSG_TYPE_EVENT    0x667    // TODO ADAPT!!!
#define OW_NETDEV_MSG_TYPE_EVENT   0x666    // TODO ADAPT!!!


/**
 * @brief   Initializes the OpenWSN stack.
 */
void openwsn_bootstrap(void);

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_H */
/** @} */
