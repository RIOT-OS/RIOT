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
 *
 * @}
 */

#include <string.h>
#include "net/sntp.h"
#include "net/ntp_packet.h"
#include "net/sock/udp.h"
#include "xtimer.h"
#include "mutex.h"
#include "byteorder.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static sock_udp_t _sntp_sock;
static int64_t _sntp_offset = 0;
static mutex_t _sntp_mutex = MUTEX_INIT;
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
    _sntp_offset = (((int64_t)byteorder_ntohl(_sntp_packet.transmit.seconds)) * US_PER_SEC) +
                   ((((int64_t)byteorder_ntohl(_sntp_packet.transmit.fraction)) * 232)
                   / 1000000) - xtimer_now_usec64();
    mutex_unlock(&_sntp_mutex);
    return 0;
}

int64_t sntp_get_offset(void)
{
    int64_t result;

    mutex_lock(&_sntp_mutex);
    result = _sntp_offset;
    mutex_unlock(&_sntp_mutex);
    return result;
}
