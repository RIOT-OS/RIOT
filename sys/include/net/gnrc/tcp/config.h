/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       GNRC TCP configuration
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gnrc_tcp_conf GNRC TCP compile configurations
 * @ingroup net_gnrc_conf
 * @ingroup net_gnrc_tcp
 *
 * ## Calculating RTO
 * To calculate retransmission timeout (RTO), Round Trip Time (RTT) needs to be
 * taken into account. SRTT (smoothed round-trip time) and RTTVAR (round-trip
 * time variation) are hence calculated as follows:
 *
 *     RTTVAR <- (1 - beta) * RTTVAR + beta * |SRTT - R'|
 *     SRTT <- (1 - alpha) * SRTT + alpha * R'
 *
 * where alpha ( 1 / @ref CONFIG_GNRC_TCP_RTO_A_DIV ) and beta
 * ( 1 / @ref CONFIG_GNRC_TCP_RTO_B_DIV) are constants, and R' is the
 * instantaneous RTT value.
 *
 * RTO is then calculated as :
 *
 *     RTO <- SRTT + max (G, K*RTTVAR)
 *
 * where K is a constant, and G is clock granularity in seconds
 * ( @ref CONFIG_GNRC_TCP_RTO_GRANULARITY_MS).
 * For more information refer to https://tools.ietf.org/html/rfc6298
 * @{
 */
/**
 * @brief Timeout duration in milliseconds for user calls. Default is 2 minutes.
 */
#ifndef CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS
#define CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS (120U * MS_PER_SEC)
#endif

/**
 * @brief Maximum segment lifetime (MSL) in milliseconds. Default is 30 seconds.
 */
#ifndef CONFIG_GNRC_TCP_MSL_MS
#define CONFIG_GNRC_TCP_MSL_MS (30U * MS_PER_SEC)
#endif

/**
 * @brief Maximum Segment Size (MSS).
 */
#ifndef CONFIG_GNRC_TCP_MSS
#ifdef  MODULE_GNRC_IPV6
#define CONFIG_GNRC_TCP_MSS (1220U) /**< If IPv6 is used. Get MSS = 1280 - IPv6 Hdr - TCP Hdr = 1220 */
#else
#define CONFIG_GNRC_TCP_MSS (576U) /**< Default MSS */
#endif
#endif

/**
 * @brief MSS Multiplicator = Number of MSS sized packets stored in receive buffer
 */
#ifndef CONFIG_GNRC_TCP_MSS_MULTIPLICATOR
#define CONFIG_GNRC_TCP_MSS_MULTIPLICATOR (1U)
#endif

/**
 * @brief Default receive window size
 */
#ifndef CONFIG_GNRC_TCP_DEFAULT_WINDOW
#define CONFIG_GNRC_TCP_DEFAULT_WINDOW (CONFIG_GNRC_TCP_MSS * CONFIG_GNRC_TCP_MSS_MULTIPLICATOR)
#endif

/**
 * @brief Number of preallocated receive buffers.
 *
 * This value determines how many parallel TCP connections can be active at the
 * same time.
 */
#ifndef CONFIG_GNRC_TCP_RCV_BUFFERS
#define CONFIG_GNRC_TCP_RCV_BUFFERS (1U)
#endif

/**
 * @brief Default receive buffer size
 */
#ifndef GNRC_TCP_RCV_BUF_SIZE
#define GNRC_TCP_RCV_BUF_SIZE (CONFIG_GNRC_TCP_DEFAULT_WINDOW)
#endif

/**
 * @brief Lower bound for RTO in milliseconds. Default is 1 sec (see RFC 6298)
 *
 * @note Retransmission Timeout (RTO) determines how long TCP waits for
 * acknowledgment (ACK) of transmitted segment. If the acknowledgment
 * isn't received within this time it is considered lost.
 */
#ifndef CONFIG_GNRC_TCP_RTO_LOWER_BOUND_MS
#define CONFIG_GNRC_TCP_RTO_LOWER_BOUND_MS (1U * MS_PER_SEC)
#endif

/**
 * @brief Upper bound for RTO in milliseconds. Default is 60 sec (see RFC 6298)
 */
#ifndef CONFIG_GNRC_TCP_RTO_UPPER_BOUND_MS
#define CONFIG_GNRC_TCP_RTO_UPPER_BOUND_MS (60U * MS_PER_SEC)
#endif

/**
 * @brief Clock granularity for TCP in milliseconds. Default is 10 milliseconds (see RFC 6298)
 */
#ifndef CONFIG_GNRC_TCP_RTO_GRANULARITY_MS
#define CONFIG_GNRC_TCP_RTO_GRANULARITY_MS (10U)
#endif

/**
 * @brief Alpha value for RTO calculation, default is 1/8
 */
#ifndef CONFIG_GNRC_TCP_RTO_A_DIV
#define CONFIG_GNRC_TCP_RTO_A_DIV (8U)
#endif

/**
 * @brief Beta value for RTO calculation, default is 1/4
 */
#ifndef CONFIG_GNRC_TCP_RTO_B_DIV
#define CONFIG_GNRC_TCP_RTO_B_DIV (4U)
#endif

/**
 * @brief K value for RTO calculation, default is 4
 */
#ifndef CONFIG_GNRC_TCP_RTO_K
#define CONFIG_GNRC_TCP_RTO_K (4U)
#endif

/**
 * @brief Lower bound for the duration between probes in milliseconds. Default is 1 seconds
 */
#ifndef CONFIG_GNRC_TCP_PROBE_LOWER_BOUND_MS
#define CONFIG_GNRC_TCP_PROBE_LOWER_BOUND_MS (1U * MS_PER_SEC)
#endif

/**
 * @brief Upper bound for the duration between probes in milliseconds. Default is 60 seconds
 */
#ifndef CONFIG_GNRC_TCP_PROBE_UPPER_BOUND_MS
#define CONFIG_GNRC_TCP_PROBE_UPPER_BOUND_MS (60U * MS_PER_SEC)
#endif

/**
 * @brief Message queue size for TCP API internal messaging
 * @note The number of elements in a message queue must be a power of two.
 *       This value defines the exponent of 2^n.
 */
#ifndef CONFIG_GNRC_TCP_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_TCP_MSG_QUEUE_SIZE_EXP (2U)
#endif

/**
 * @brief Message queue size for the TCP eventloop
 * @note The number of elements in a message queue must be a power of two.
 *       This value defines the exponent of 2^n.
 */
#ifndef CONFIG_GNRC_TCP_EVENTLOOP_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_TCP_EVENTLOOP_MSG_QUEUE_SIZE_EXP (3U)
#endif

/**
 * @brief Enable experimental feature "dynamic msl". Disabled by default.
 * @experimental This feature is experimental because it deviates from the TCP RFC.
 * @note This features calculates the MSL based by multiplying the latest
 *       retransmission timeout value with
 *       CONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_RTO_MUL. This leads to much
 *       faster return times on gnrc_tcp_close.
 */
#ifndef CONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_EN
#define CONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_EN 0
#endif

/**
 * @brief Set RTO multiplication factor if experimental feature "dynamic msl" is enabled.
 * @experimental This feature is experimental because it deviates from the TCP RFC.
 */
#ifndef CONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_RTO_MUL
#define CONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_RTO_MUL (4U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
