/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_net_tcp TCP
 * @ingroup     net
 * @breif       RIOT's implementation of the TCP protocol
 *
 * @{
 *
 * @file
 * @brief       TCP configuration, includes buffersizes, timeout durations
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @}
 */

#ifndef GNRC_TCP_CONFIG_H_
#define GNRC_TCP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timeout Duration for user calls. Default 2 minutes
 */
#ifndef GNRC_TCP_USER_TIMEOUT_DURATION
#define GNRC_TCP_USER_TIMEOUT_DURATION (2 * 60 * 1000 * 1000)
#endif

/**
 * @brief Maximum Segment Lifetime
 */
#ifndef GNRC_TCP_MSL
#define GNRC_TCP_MSL (1000 * 1000)
#endif

/**
 * @brief   Default message queue size for the TCP thread
 */
#ifndef GNRC_TCP_MSG_QUEUE_SIZE
#define GNRC_TCP_MSG_QUEUE_SIZE (8U)
#endif

/**
 * @brief Message Queue inside TCB.
 */
#ifndef TCB_MSG_QUEUE_SIZE
#define TCB_MSG_QUEUE_SIZE (8U)
#endif
/**
 * @brief   Priority of the tcp thread, must be lower than the applications prio.
 */
#ifndef GNRC_TCP_PRIO
#define GNRC_TCP_PRIO           (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Default stack size to use for the TCP thread
 */
#ifndef GNRC_TCP_STACK_SIZE
#define GNRC_TCP_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
#endif

/* Maximum Segment Size */
/* If IPv6 is used. Get MSS = 1280 - IPv6-Hdr - TCP-Hdr = 1220*/
#ifdef  MODULE_GNRC_IPV6
#ifndef GNRC_TCP_MSS
#define GNRC_TCP_MSS 1220
#endif
#endif

/* Default MSS. Maximum amount of payload in bytes */
#ifndef GNRC_TCP_MSS
#define GNRC_TCP_MSS 576
#endif

/* Default Window Size */
#define DEFAULT_WINDOW           (GNRC_TCP_MSS)

/* Default Receive Buffer Size */
#ifndef GNRC_TCP_RCV_BUF_SIZE
#define GNRC_TCP_RCV_BUF_SIZE    (DEFAULT_WINDOW)
#endif

/* Array Size to pick a seed for the PRNG */
/* Randomness by uninitialized memory garbage */
#define SEED_LEN 100

#ifdef __cplusplus
}
#endif

#endif
