/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_sock_dns
 * @{
 * @file
 * @brief   sock DNS client implementation
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#include <arpa/inet.h>

#include "net/dns.h"
#include "net/dns/msg.h"
#include "net/sock/udp.h"
#include "net/sock/dns.h"

/* min domain name length is 1, so minimum record length is 7 */
#define DNS_MIN_REPLY_LEN   (unsigned)(sizeof(dns_hdr_t) + 7)

/* global DNS server UDP endpoint */
sock_udp_ep_t sock_dns_server;

#ifdef MODULE_AUTO_INIT_SOCK_DNS
void auto_init_sock_dns(void)
{
    assert(   CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION == 4
           || CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION == 6);

    assert(   CONFIG_AUTO_INIT_SOCK_DNS_SERVER_PORT > 0
           && CONFIG_AUTO_INIT_SOCK_DNS_SERVER_PORT <= 0xffff);

    switch (CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION) {
#ifdef SOCK_HAS_IPV4
    case 4:
        inet_pton(AF_INET, CONFIG_AUTO_INIT_SOCK_DNS_SERVER_ADDR,
                  sock_dns_server.addr.ipv4);
        sock_dns_server.family = AF_INET;
        break;
#endif
#ifdef SOCK_HAS_IPV6
    case 6:
        inet_pton(AF_INET6, CONFIG_AUTO_INIT_SOCK_DNS_SERVER_ADDR,
                  sock_dns_server.addr.ipv6);
        sock_dns_server.family = AF_INET6;
        break;
#endif
    default:
        assert(0);
        return;
    }

    sock_dns_server.port = CONFIG_AUTO_INIT_SOCK_DNS_SERVER_PORT;
}
#endif /* MODULE_AUTO_INIT_SOCK_DNS */

int sock_dns_query(const char *domain_name, void *addr_out, int family)
{
    static uint8_t dns_buf[CONFIG_DNS_MSG_LEN];

    if (sock_dns_server.port == 0) {
        return -ECONNREFUSED;
    }

    if (strlen(domain_name) > SOCK_DNS_MAX_NAME_LEN) {
        return -ENOSPC;
    }

    sock_udp_t sock_dns;

    ssize_t res = sock_udp_create(&sock_dns, NULL, &sock_dns_server, 0);
    if (res) {
        goto out;
    }

    uint16_t id = 0;
    for (int i = 0; i < SOCK_DNS_RETRIES; i++) {
        size_t buflen = dns_msg_compose_query(dns_buf, domain_name, id, family);

        res = sock_udp_send(&sock_dns, dns_buf, buflen, NULL);
        if (res <= 0) {
            continue;
        }
        res = sock_udp_recv(&sock_dns, dns_buf, sizeof(dns_buf), 1000000LU, NULL);
        if (res > 0) {
            if (res > (int)DNS_MIN_REPLY_LEN) {
                if ((res = dns_msg_parse_reply(dns_buf, res, family,
                                               addr_out)) > 0) {
                    goto out;
                }
            }
            else {
                res = -EBADMSG;
            }
        }
    }

out:
    sock_udp_close(&sock_dns);
    return res;
}
