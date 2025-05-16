/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_pktdump Dump Network Packets
 * @ingroup     net_gnrc
 * @brief       Dump network packets to STDOUT for debugging
 *
 * @{
 *
 * @file
 * @brief       Interface for a generic network packet dumping module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gnrc_pktdump_conf GNRC PKTDUMP compile configurations
 * @ingroup net_gnrc_conf
 * @{
 */
/**
 * @brief   Default message queue size for the PKTDUMP thread (as exponent of
 *          2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_GNRC_PKTDUMP_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_PKTDUMP_MSG_QUEUE_SIZE_EXP  3
#endif
/** @} */

/**
 * @brief   Message queue size for the pktdump thread
 */
#ifndef GNRC_PKTDUMP_MSG_QUEUE_SIZE
#define GNRC_PKTDUMP_MSG_QUEUE_SIZE  (1 << CONFIG_GNRC_PKTDUMP_MSG_QUEUE_SIZE_EXP)
#endif

/**
 * @brief   Priority of the pktdump thread
 */
#ifndef GNRC_PKTDUMP_PRIO
#define GNRC_PKTDUMP_PRIO               (THREAD_PRIORITY_MAIN - 1)
#endif

/**
 * @brief   Stack size used for the pktdump thread
 *
 * @note    The message queue was previously allocated on the stack.
 *          The default number of messages is 2³.
 *          Given sizeof(msg_t) == 8, the stack size is reduced by 64 bytes.
 */
#ifndef GNRC_PKTDUMP_STACKSIZE
#define GNRC_PKTDUMP_STACKSIZE          ((THREAD_STACKSIZE_MAIN) - 64)
#endif

/**
 * @brief   The PID of the pktdump thread
 */
extern kernel_pid_t gnrc_pktdump_pid;

/**
 * @brief   Start the packet dump thread and listening for incoming packets
 *
 * @return  PID of the pktdump thread
 * @return  negative value on error
 */
kernel_pid_t gnrc_pktdump_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
