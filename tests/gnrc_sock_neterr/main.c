/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include "msg.h"
#include "net/sock/udp.h"

#define TEST_PORT               (38664U)
#define TEST_LINK_LOCAL_REMOTE  { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 }
#define TEST_GLOBAL_REMOTE      { 0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, \
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 }
#define TEST_PAYLOAD            { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }

static const uint8_t _test_link_local_remote[] = TEST_LINK_LOCAL_REMOTE;
static const uint8_t _test_global_remote[] = TEST_GLOBAL_REMOTE;
static const uint8_t _test_payload[] = TEST_PAYLOAD;

static msg_t _msg_queue[8U];
static sock_udp_t _udp_sock;

int _test_udp_send(sock_udp_ep_t *remote, char *errno_str, int exp_res) {

    int res = sock_udp_send(&_udp_sock, _test_payload, sizeof(_test_payload),
                        remote);
    /* remote is not reachable or route does not exist, so it should return an
     * error */
    if (-res == exp_res) {
        printf("SUCCESS: error code %s (%li == %i)\n",
               errno_str, (long)(-res), exp_res);
        return 0;
    }
    else {
        printf("FAILURE: gnrc_udp_send() had an unexpected error code: %li\n",
               (long int)res);
        return 1;
    }
}

int main(void)
{
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    local.port = TEST_PORT;
    remote.port = TEST_PORT - 1;

    msg_init_queue(_msg_queue, ARRAY_SIZE(_msg_queue));
    sock_udp_create(&_udp_sock, &local, NULL, 0);

    memcpy(remote.addr.ipv6, _test_link_local_remote,
           sizeof(_test_link_local_remote));
    /* With a 6LN this send may succeed, as according to RFC 6775 link-local
     * addresses are assumed to be generated from the EUI-64, reversing the
     * procedure of generating the address from EUI-64 for address resolution */
    _test_udp_send(&remote, "EHOSTUNREACH", EHOSTUNREACH);
    memcpy(remote.addr.ipv6, _test_global_remote,
           sizeof(_test_global_remote));
    _test_udp_send(&remote, "ENETUNREACH", ENETUNREACH);
}

/** @} */
