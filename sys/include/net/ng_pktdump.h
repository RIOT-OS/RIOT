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
 * @brief   Start the packet dump thread and listening for incoming packets
 *
 * @param[in] stack         stack for the packet dump thread
 * @param[in] stacksize     size of @p stack
 * @param[in] priority      priority of the packet dump thread
 * @param[in] name          name for the packet dump thread
 *
 * @return                  PID of newly created task on success
 * @return                  negative value on error
 */
kernel_pid_t ng_pktdump_init(char *stack, int stacksize,
                             char priority, char *name);

#ifdef __cplusplus
}
#endif

#endif /* NG_PKTDUMP_H_ */
/** @} */
