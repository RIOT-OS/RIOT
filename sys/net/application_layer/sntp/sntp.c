/*
 * Copyright (C) 2016 Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       SNTP implementation
 *
 * @author      Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * @}
 */

#include <string.h>
#include "net/sntp.h"
#include "net/ntp_packet.h"
#include "net/sock/udp.h"
#include "ztimer.h"
#include "mutex.h"
#include "byteorder.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _validity_expires(void);

static sock_udp_t _sntp_sock;
/* If set to this value, _sntp_offset is invalid. The choice of 0 is good
 * because it is cheap to test for, and if the time machine to run this code in
 * the 50 first days of 1970 is invented, we'll have bigger problems than this
 * code failing. */
#define SNTP_OFFSET_INVALID (0)
/* This plus ztimer_now() will produce the current POSIX timestamp (seconds since UTC without leap seconds) */
static int64_t _sntp_offset = SNTP_OFFSET_INVALID;
/* Lock this when accessing _sntp_offset (yes, even when reading, for the
 * alternative would be 64bit atomics) or _sntp_validity_timer */
static mutex_t _sntp_mutex = MUTEX_INIT;
/* Time for which the clock will be considered good. This could be decreased,
 * and is not set to the absolute maximum a ztimer can have, but a minute less,
 * because that allows users to trust that interrupts have been off in-between.
 * (If the ZTimer interrupt has not had its chance to run in 60 seconds, the
 * system is already stalled beyond hope) */
#define SNTP_VALIDITY_TIMER_LENGTH (UINT32_MAX - 60 * MS_PER_SEC)
/* Timer that keeps ZTIMER_MSEC alive */
static ztimer_t _sntp_validity_timer = { .callback = _validity_expires };
static ntp_packet_t _sntp_packet;

int sntp_sync(sock_udp_ep_t *server, uint32_t timeout)
{
    int result;

    if ((result = sock_udp_create(&_sntp_sock,
                                  NULL,
                                  server,
                                  0)) < 0) {
        DEBUG("Error creating UDP sock\n");
        return result;
    }
    memset(&_sntp_packet, 0, sizeof(_sntp_packet));
    ntp_packet_set_vn(&_sntp_packet);
    ntp_packet_set_mode(&_sntp_packet, NTP_MODE_CLIENT);

    if ((result = (int)sock_udp_send(&_sntp_sock,
                                     &_sntp_packet,
                                     sizeof(_sntp_packet),
                                     NULL)) < 0) {
        DEBUG("Error sending message\n");
        sock_udp_close(&_sntp_sock);
        return result;
    }
    if ((result = (int)sock_udp_recv(&_sntp_sock,
                                     &_sntp_packet,
                                     sizeof(_sntp_packet),
                                     timeout,
                                     NULL)) < 0) {
        DEBUG("Error receiving message\n");
        sock_udp_close(&_sntp_sock);
        return result;
    }
    sock_udp_close(&_sntp_sock);
    mutex_lock(&_sntp_mutex);
    uint32_t now = ztimer_set(ZTIMER_MSEC, _sntp_validity_timer,
            SNTP_VALIDITY_TIMER_LENGTH);
    _sntp_offset = (((int64_t)byteorder_ntohl(_sntp_packet.transmit.seconds)) * MS_PER_SEC) +
                   ((((int64_t)byteorder_ntohl(_sntp_packet.transmit.fraction)) * 232)
                   / 1000000000) - now;
    mutex_unlock(&_sntp_mutex);
    return 0;
}

static void _validity_expires(void) {
    // FIXME how to clear the expiry field w/o waiting for a mutex? need better mutex setup.
}

int64_t sntp_get_offset(void)
{
    int64_t result;

    mutex_lock(&_sntp_mutex);
    result = _sntp_offset;
    mutex_unlock(&_sntp_mutex);
    if (!ztimer_is_set(ZTIMER_MS, _sntp_validity_timer)) {
        return SNTP_OFFSET_INVALID;
    }
    return result;
}
