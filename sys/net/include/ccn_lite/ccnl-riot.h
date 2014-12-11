/*
 * @f ccnl-riot.h
 *
 * Copyright (C) 2013, Christian Mehlis, Freie Universität Berlin
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @defgroup ccnl   CCN lite
 * @ingroup net
 * @brief   CCN-lite, a lightweight implementation of the Content Centric
 * Networking Protocol of XEROX Parc
 *
 * @see     <a href="http://www.ccnx.org/releases/latest/doc/technical/CCNxProtocol.html">
 *          CCNx Protocol
 *          </a>
 *
 * @{
 * @file
 * @brief   CCN lite interface
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#ifndef RIOT_CCN_COMPAT_H_
#define RIOT_CCN_COMPAT_H_

#include <inttypes.h>

#include "transceiver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRANSCEIVER TRANSCEIVER_DEFAULT

#define CCNL_DEFAULT_CHANNEL 6
#define CCNL_DEFAULT_MAX_CACHE_ENTRIES  0   /* 0: no content caching, cache is disabled */
#define CCNL_DEFAULT_THRESHOLD_PREFIX   1
#define CCNL_DEFAULT_THRESHOLD_AGGREGATE 2

#define CCNL_RIOT_EVENT_NUMBER_OFFSET (1 << 8)
#define CCNL_RIOT_MSG                 (CCNL_RIOT_EVENT_NUMBER_OFFSET + 0)
#define CCNL_RIOT_HALT                (CCNL_RIOT_EVENT_NUMBER_OFFSET + 1)
#define CCNL_RIOT_POPULATE            (CCNL_RIOT_EVENT_NUMBER_OFFSET + 2)
#define CCNL_RIOT_PRINT_STAT          (CCNL_RIOT_EVENT_NUMBER_OFFSET + 3)
#define CCNL_RIOT_NACK                (CCNL_RIOT_EVENT_NUMBER_OFFSET + 4)
#define CCNL_RIOT_CONFIG_CACHE        (CCNL_RIOT_EVENT_NUMBER_OFFSET + 5)
#define CCNL_RIOT_RESERVED            (CCNL_RIOT_EVENT_NUMBER_OFFSET + 6)

#define CCNL_HEADER_SIZE (40)

#ifdef MODULE_NATIVENET
/*
 * static content for testing ccn get has this chunk size
 * this test (populate + interest /riot/text) current works
 * only on transceiver which can handle ~130 bytes
 */
#  define CCNL_RIOT_CHUNK_SIZE (90)
#else
/* XXX: For (at least) IEEE 802.15.4 radios (PAYLOAD_SIZE - CCNL_HEADER_SIZE)
 * is one byte too big for transmission. Therefore, we subtract one byte more.
 */
#  define CCNL_RIOT_CHUNK_SIZE (PAYLOAD_SIZE - CCNL_HEADER_SIZE - 1)
#endif

/**
 * @brief starts the ccnl relay
 *
 * @note  to stop the relay send msg "RIOT_HALT" to this thread
 *
 */
void *ccnl_riot_relay_start(void *arg);

/**
 * @brief  starts an appication server, which can repy to ccn interests
 *
 * @param arg the pid of the relay
 */
void *ccnl_riot_appserver_start(void *arg);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* RIOT_CCN_COMPAT_H_ */
