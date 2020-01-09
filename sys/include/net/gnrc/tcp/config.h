/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef NET_GNRC_TCP_CONFIG_H
#define NET_GNRC_TCP_CONFIG_H

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timeout duration for user calls. Default is 2 minutes.
 */
#ifndef GNRC_TCP_CONNECTION_TIMEOUT_DURATION
#define GNRC_TCP_CONNECTION_TIMEOUT_DURATION (120U * US_PER_SEC)
#endif

/**
 * @brief Maximum segment lifetime (MSL). Default is 30 seconds.
 */
#ifndef GNRC_TCP_MSL
#define GNRC_TCP_MSL (30U * US_PER_SEC)
#endif

/**
 * @brief Maximum Segment Size (MSS).
 */
#ifndef GNRC_TCP_MSS
#ifdef  MODULE_GNRC_IPV6
#define GNRC_TCP_MSS (1220U) /**< If IPv6 is used. Get MSS = 1280 - IPv6 Hdr - TCP Hdr = 1220 */
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
 * @brief Default receive window size
 */
#ifndef GNRC_TCP_DEFAULT_WINDOW
#define GNRC_TCP_DEFAULT_WINDOW (GNRC_TCP_MSS * GNRC_TCP_MSS_MULTIPLICATOR)
#endif

/**
 * @brief Number of preallocated receive buffers
 */
#ifndef GNRC_TCP_RCV_BUFFERS
#define GNRC_TCP_RCV_BUFFERS (1U)
#endif

/**
 * @brief Default receive buffer size
 */
#ifndef GNRC_TCP_RCV_BUF_SIZE
#define GNRC_TCP_RCV_BUF_SIZE (GNRC_TCP_DEFAULT_WINDOW)
#endif

/**
 * @brief Lower bound for RTO = 1 sec (see RFC 6298)
 */
#ifndef GNRC_TCP_RTO_LOWER_BOUND
#define GNRC_TCP_RTO_LOWER_BOUND (1U * US_PER_SEC)
#endif

/**
 * @brief Upper bound for RTO = 60 sec (see RFC 6298)
 */
#ifndef GNRC_TCP_RTO_UPPER_BOUND
#define GNRC_TCP_RTO_UPPER_BOUND (60U * US_PER_SEC)
#endif

/**
 * @brief Assumes clock granularity for TCP of 10 ms (see RFC 6298)
 */
#ifndef GNRC_TCP_RTO_GRANULARITY
#define GNRC_TCP_RTO_GRANULARITY (10U * MS_PER_SEC)
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
 * @brief Lower bound for the duration between probes
 */
#ifndef GNRC_TCP_PROBE_LOWER_BOUND
#define GNRC_TCP_PROBE_LOWER_BOUND (1U * US_PER_SEC)
#endif

/**
 * @brief Upper bound for the duration between probes
 */
#ifndef GNRC_TCP_PROBE_UPPER_BOUND
#define GNRC_TCP_PROBE_UPPER_BOUND (60U * US_PER_SEC)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_TCP_CONFIG_H */
/** @} */
