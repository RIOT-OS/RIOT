/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief       TCP configuration, includes buffersizes, timeout durations
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 */

#ifndef GNRC_TCP_CONFIG_H_
#define GNRC_TCP_CONFIG_H_

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Status Flags for TCP
 * @{
 */
#define GNRC_TCP_STATUS_PASSIVE (1 << 0)
#define GNRC_TCP_STATUS_ACCEPTED (1 << 1)
#define GNRC_TCP_STATUS_ALLOW_ANY_ADDR (1 << 2)
/** @} */

/**
 * @brief Timeout Duration for user calls. Default 2 minutes
 */
#ifndef GNRC_TCP_CONNECTION_TIMEOUT_DURATION
#define GNRC_TCP_CONNECTION_TIMEOUT_DURATION (120 * US_PER_SEC)
#endif

/**
 * @brief Maximum Segment Lifetime. Default 30 secounds
 */
#ifndef GNRC_TCP_MSL
#define GNRC_TCP_MSL (30 * US_PER_SEC)
#endif

/**
 * @brief Message queue size for the TCP handling thread
 */
#ifndef GNRC_TCP_MSG_QUEUE_SIZE
#define GNRC_TCP_MSG_QUEUE_SIZE (8U)
#endif

/**
 * @brief Priority of the tcp handling thread, must be lower than the applications prio.
 */
#ifndef GNRC_TCP_PRIO
#define GNRC_TCP_PRIO (THREAD_PRIORITY_MAIN - 2U)
#endif

/**
 * @brief Default stack size for the TCP handling thread
 */
#ifndef GNRC_TCP_STACK_SIZE
#define GNRC_TCP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief Maximum Segement Size
 */
#ifndef GNRC_TCP_MSS
#ifdef  MODULE_GNRC_IPV6
#define GNRC_TCP_MSS (1220U) /**< If IPv6 is used. Get MSS = 1280 - IPv6-Hdr - TCP-Hdr = 1220 */
#else
#define GNRC_TCP_MSS (576U) /**< Default MSS */
#endif
#endif

/**
 * @brief MSS Multiplicator = Number of MSS sized packets stored in receive buffer
 */
#ifndef GNRC_TCP_MSS_MULTIPLICATOR
#define GNRC_TCP_MSS_MULTIPLICATOR (1U)
#endif

/**
 * @brief Default Window Size
 */
#ifndef GNRC_TCP_DEFAULT_WINDOW
#define GNRC_TCP_DEFAULT_WINDOW (GNRC_TCP_MSS * GNRC_TCP_MSS_MULTIPLICATOR)
#endif

/**
 * @brief Number of preallocated receive buffers
 */
#ifndef GNRC_TCP_RCV_BUFFERS
#define GNRC_TCP_RCV_BUFFERS 1
#endif

/**
 * @brief Default Receive Buffer Size
 */
#ifndef GNRC_TCP_RCV_BUF_SIZE
#define GNRC_TCP_RCV_BUF_SIZE (GNRC_TCP_DEFAULT_WINDOW)
#endif

/**
 * @brief Lower Bound for RTO = 1 sec (see RFC 6298)
 */
#ifndef GNRC_TCP_RTO_LOWER_BOUND
#define GNRC_TCP_RTO_LOWER_BOUND (1 * US_PER_SEC)
#endif

/**
 * @brief Upper Bound for RTO = 60 sec (see RFC 6298)
 */
#ifndef GNRC_TCP_RTO_UPPER_BOUND
#define GNRC_TCP_RTO_UPPER_BOUND (60 * US_PER_SEC)
#endif

/**
 * @brief Assumes clock granularity for TCP of 10 ms (see RFC 6298)
 */
#ifndef GNRC_TCP_RTO_GRANULARITY
#define GNRC_TCP_RTO_GRANULARITY (10 * MS_PER_SEC)
#endif

/**
 * @brief Alpha value for RTO calculation, default is 1/8
 */
#ifndef GNRC_TCP_RTO_A_DIV
#define GNRC_TCP_RTO_A_DIV (8U)
#endif

/**
 * @brief Beta value for RTO calculation, default is 1/4
 */
#ifndef GNRC_TCP_RTO_B_DIV
#define GNRC_TCP_RTO_B_DIV (4U)
#endif

/**
 * @brief K value for RTO calculation, default is 4
 */
#ifndef GNRC_TCP_RTO_K
#define GNRC_TCP_RTO_K (4U)
#endif

/**
 * @brief Macro to mark is the time measurement is uninitialized
 */
#ifndef GNRC_TCP_RTO_UNINITIALIZED
#define GNRC_TCP_RTO_UNINITIALIZED (-1)
#endif

/**
 * @brief Lower Bound for the duration between probes
 */
#ifndef GNRC_TCP_PROBE_LOWER_BOUND
#define GNRC_TCP_PROBE_LOWER_BOUND (1 * US_PER_SEC)
#endif

/**
 * @brief Upper Bound for the duration between probes
 */
#ifndef GNRC_TCP_PROBE_UPPER_BOUND
#define GNRC_TCP_PROBE_UPPER_BOUND (60 * US_PER_SEC)
#endif

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_CONFIG_H_ */
/** @} */
