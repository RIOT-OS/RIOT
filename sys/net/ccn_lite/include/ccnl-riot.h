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
 * @author  Christian Mehlis <christian@m3hlis.de>
 */
#ifndef RIOT_CCN_COMPAT_H_
#define RIOT_CCN_COMPAT_H_

#include <inttypes.h>

#include "transceiver.h"

#ifdef MODULE_CC110X_NG
#  include "cc110x_ng.h"
#  define transceiver_ids (TRANSCEIVER_CC1100 | TRANSCEIVER_NONE)
#else
#  include "nativenet.h"
#  define transceiver_ids (TRANSCEIVER_NATIVE | TRANSCEIVER_NONE)
#endif

#define CCNL_RIOT_EVENT_NUMBER_OFFSET (1 << 8)
typedef enum ccnl_riot_event {
    CCNL_RIOT_MSG = CCNL_RIOT_EVENT_NUMBER_OFFSET + 1,
    CCNL_RIOT_HALT,
    CCNL_RIOT_POPULATE,

    CCNL_RIOT_RESERVED
} ccnl_riot_event_t;

#define CCNL_HEADER_SIZE (40)

#ifdef MODULE_NATIVENET
/*
 * static content for testing ccn get has this chunk size
 * this test (populate + interest /riot/text) current works
 * only on transceiver which can handle ~130 bytes
 */
#  define CCNL_RIOT_CHUNK_SIZE (90)
#else
#  define CCNL_RIOT_CHUNK_SIZE (PAYLOAD_SIZE - CCNL_HEADER_SIZE)
#endif



/**
 * @brief starts the ccnl relay
 *
 * @note  to stop the relay send msg "RIOT_HALT" to this thread
 *
 * @param max_cache_entries number of slots in the CS
 */
void ccnl_riot_relay_start(int max_cache_entries);

/**
 * @brief  starts an appication server, which can repy to ccn interests
 *
 * @param relay_pid the pid of the relay
 */
void ccnl_riot_appserver_start(int relay_pid);

/**
 * @}
 */
#endif /* RIOT_CCN_COMPAT_H_ */
