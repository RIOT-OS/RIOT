/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_pktdump Dump Network Packets
 * @ingroup     net
 * @brief       Dump network packets to STDOUT for debugging
 *
 * @{
 *
 * @file
 * @brief       Interface for a generic network packet dumping module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_PKTDUMP_H_
#define NG_PKTDUMP_H_

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message queue size for the pktdump thread
 */
#ifndef NG_PKTDUMP_MSG_QUEUE_SIZE
#define NG_PKTDUMP_MSG_QUEUE_SIZE       (8U)
#endif

/**
 * @brief   Priority of the pktdump thread
 */
#ifndef NG_PKTDUMP_PRIO
#define NG_PKTDUMP_PRIO                 (PRIORITY_MAIN - 1)
#endif

/**
 * @brief   Stack size used for the pktdump thread
 */
#ifndef NG_PKTDUMP_STACKSIZE
#define NG_PKTDUMP_STACKSIZE            (KERNEL_CONF_STACKSIZE_MAIN)
#endif

/**
 * @brief   Get the PID of the pktdump thread
 *
 * @return  PID of the pktdump thread
 * @return  @ref KERNEL_PID_UNDEF if not initialized
 */
kernel_pid_t ng_pktdump_getpid(void);

/**
 * @brief   Start the packet dump thread and listening for incoming packets
 *
 * @return  PID of the pktdump thread
 * @return  negative value on error
 */
kernel_pid_t ng_pktdump_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NG_PKTDUMP_H_ */
/** @} */
