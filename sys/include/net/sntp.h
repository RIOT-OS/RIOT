/*
 * SPDX-FileCopyrightText: 2016 Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_sntp Simple Network Time Protocol
 * @ingroup     net
 * @brief       Simple Network Time Protocol (SNTP) implementation
 * @{
 *
 * @note        The current implementation of SNTP uses @ref sys_ztimer64 with
 *              microsecond accuracy, which can have a strong impact on
 *              the power consumption of your device.
 *
 * @file
 * @brief       SNTP definitions
 *
 * @author      Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>
#include <time.h>

#include "net/ntp_packet.h"
#include "net/sock/udp.h"
#include "ztimer64.h"

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
 * @brief Get real time offset from system time as returned by @ref ztimer64_now()
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
    return (uint64_t)(sntp_get_offset() - (NTP_UNIX_OFFSET * US_PER_SEC) + \
                      ztimer64_now(ZTIMER64_USEC));
}

#ifdef __cplusplus
}
#endif

/** @} */
