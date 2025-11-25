/*
 * Copyright (C) 2016 Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sntp Simple Network Time Protocol
 * @ingroup     net
 * @brief       Simple Network Time Protocol (SNTP) implementation
 * @{
 *
 * @file
 * @brief       SNTP definitions
 *
 * @author      Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifndef NET_SNTP_H
#define NET_SNTP_H

#include <stdint.h>
#include <time.h>

#include "net/ntp_packet.h"
#include "net/sock/udp.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Synchronize with time server
 *
 * @param[in] server    The time server
 * @param[in] timeout   Timeout for the server response in microseconds
 *
 * @return 0 on success
 * @return Negative number on error
 */
int sntp_sync(sock_udp_ep_t *server, uint32_t timeout);

/**
 * @brief Get real time offset from system time as returned by @ref xtimer_now64()
 *
 * @return Real time offset in microseconds relative to 1900-01-01 00:00 UTC
 */
int64_t sntp_get_offset(void);

/**
 * @brief   Get time in microseconds from 1970-01-01 00:00:00 UTC.
 *
 * @return  Time in microseconds from 1970-01-01 00:00:00 UTC
 */
static inline uint64_t sntp_get_unix_usec(void)
{
    return (uint64_t)(sntp_get_offset() - (NTP_UNIX_OFFSET * US_PER_SEC) + xtimer_now_usec64());
}

#ifdef __cplusplus
}
#endif

#endif /* NET_SNTP_H */
/** @} */
