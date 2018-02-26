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
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "net/sock/udp.h"
#include "net/sock/dns.h"

#ifdef RIOT_VERSION
#include "byteorder.h"
#endif

/* min domain name length is 1, so minimum record length is 7 */
#define DNS_MIN_REPLY_LEN   (unsigned)(sizeof(sock_dns_hdr_t ) + 7)

/* global DNS server UDP endpoint */
sock_udp_ep_t sock_dns_server;

static ssize_t _enc_domain_name(uint8_t *out, const char *domain_name)
{
    /*
     * DNS encodes domain names with "<len><part><len><part>", e.g.,
     * "example.org" ends up as "\7example\3org" in the packet.
     */
    uint8_t *part_start = out;
    uint8_t *out_pos = ++out;

    char c;

    while ((c = *domain_name)) {
        if (c == '.') {
            /* replace dot with length of name part as byte */
            *part_start = (out_pos - part_start - 1);
            part_start = out_pos++;
        }
        else {
            *out_pos++ = c;
        }
        domain_name++;
    }

    *part_start = (out_pos - part_start - 1);
    *out_pos++ = 0;

    return out_pos - out + 1;
}

static unsigned _put_short(uint8_t *out, uint16_t val)
{
    memcpy(out, &val, 2);
    return 2;
}

static unsigned _get_short(uint8_t *buf)
{
    uint16_t _tmp;
    memcpy(&_tmp, buf, 2);
    return _tmp;
}

static size_t _skip_hostname(uint8_t *buf)
{
    uint8_t *bufpos = buf;

    /* handle DNS Message Compression */
    if (*bufpos >= 192) {
        return 2;
    }

    while(*bufpos) {
        bufpos += *bufpos + 1;
    }
    return (bufpos - buf + 1);
}

static int _parse_dns_reply(uint8_t *buf, size_t len, void* addr_out, int family)
{
    sock_dns_hdr_t *hdr = (sock_dns_hdr_t*) buf;
    uint8_t *bufpos = buf + sizeof(*hdr);

    /* skip all queries that are part of the reply */
    for (unsigned n = 0; n < ntohs(hdr->qdcount); n++) {
        bufpos += _skip_hostname(bufpos);
        bufpos += 4;    /* skip type and class of query */
    }

    for (unsigned n = 0; n < ntohs(hdr->ancount); n++) {
        bufpos += _skip_hostname(bufpos);
        uint16_t _type = ntohs(_get_short(bufpos));
        bufpos += 2;
        uint16_t class = ntohs(_get_short(bufpos));
        bufpos += 2;
        bufpos += 4; /* skip ttl */

        unsigned addrlen = ntohs(_get_short(bufpos));
        bufpos += 2;
        if ((bufpos + addrlen) > (buf + len)) {
            return -EBADMSG;
        }

        /* skip unwanted answers */
        if ((class != DNS_CLASS_IN) ||
                ((_type == DNS_TYPE_A) && (family == AF_INET6)) ||
                ((_type == DNS_TYPE_AAAA) && (family == AF_INET)) ||
                ! ((_type == DNS_TYPE_A) || ((_type == DNS_TYPE_AAAA))
                    )) {
            bufpos += addrlen;
            continue;
        }

        memcpy(addr_out, bufpos, addrlen);
        return addrlen;
    }

    return -1;
}

int sock_dns_query(const char *domain_name, void *addr_out, int family)
{
    uint8_t buf[SOCK_DNS_QUERYBUF_LEN];
    uint8_t reply_buf[512];

    if (sock_dns_server.port == 0) {
        return -ECONNREFUSED;
    }

    if (strlen(domain_name) > SOCK_DNS_MAX_NAME_LEN) {
        return -ENOSPC;
    }

    sock_dns_hdr_t *hdr = (sock_dns_hdr_t*) buf;
    memset(hdr, 0, sizeof(*hdr));
    hdr->id = 0; /* random? */
    hdr->flags = htons(0x0120);
    hdr->qdcount = htons(1 + (family == AF_UNSPEC));

    uint8_t *bufpos = buf + sizeof(*hdr);

    unsigned _name_ptr;
    if ((family == AF_INET6) || (family == AF_UNSPEC)) {
        _name_ptr = (bufpos - buf);
        bufpos += _enc_domain_name(bufpos, domain_name);
        bufpos += _put_short(bufpos, htons(DNS_TYPE_AAAA));
        bufpos += _put_short(bufpos, htons(DNS_CLASS_IN));
    }

    if ((family == AF_INET) || (family == AF_UNSPEC)) {
        if (family == AF_UNSPEC) {
            bufpos += _put_short(bufpos, htons((0xc000) | (_name_ptr)));
        }
        else {
            bufpos += _enc_domain_name(bufpos, domain_name);
        }
        bufpos += _put_short(bufpos, htons(DNS_TYPE_A));
        bufpos += _put_short(bufpos, htons(DNS_CLASS_IN));
    }

    sock_udp_t sock_dns;

    ssize_t res = sock_udp_create(&sock_dns, NULL, &sock_dns_server, 0);
    if (res) {
            puts("a");
        goto out;
    }

    for (int i = 0; i < SOCK_DNS_RETRIES; i++) {
        res = sock_udp_send(&sock_dns, buf, (bufpos-buf), NULL);
        if (res <= 0) {
            continue;
        }
        res = sock_udp_recv(&sock_dns, reply_buf, sizeof(reply_buf), 1000000LU, NULL);
        if ((res > 0) && (res > (int)DNS_MIN_REPLY_LEN)) {
            if ((res = _parse_dns_reply(reply_buf, res, addr_out, family)) > 0) {
                goto out;
            }
        }
    }

out:
    sock_udp_close(&sock_dns);
    return res;
}
