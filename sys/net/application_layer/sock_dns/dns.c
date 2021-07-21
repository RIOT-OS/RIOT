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

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#include "net/dns.h"
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

static ssize_t _skip_hostname(const uint8_t *buf, size_t len, uint8_t *bufpos)
{
    const uint8_t *buflim = buf + len;
    unsigned res = 0;

    if (bufpos >= buflim) {
        /* out-of-bound */
        return -EBADMSG;
    }
    /* handle DNS Message Compression */
    if (*bufpos >= 192) {
        if ((bufpos + 2) >= buflim) {
            return -EBADMSG;
        }
        return 2;
    }

    while (bufpos[res]) {
        res += bufpos[res] + 1;
        if ((&bufpos[res]) >= buflim) {
            /* out-of-bound */
            return -EBADMSG;
        }
    }
    return res + 1;
}

static int _parse_dns_reply(uint8_t *buf, size_t len, void* addr_out, int family)
{
    const uint8_t *buflim = buf + len;
    sock_dns_hdr_t *hdr = (sock_dns_hdr_t*) buf;
    uint8_t *bufpos = buf + sizeof(*hdr);

    /* skip all queries that are part of the reply */
    for (unsigned n = 0; n < ntohs(hdr->qdcount); n++) {
        ssize_t tmp = _skip_hostname(buf, len, bufpos);
        if (tmp < 0) {
            return tmp;
        }
        bufpos += tmp;
        /* skip type and class of query */
        bufpos += (RR_TYPE_LENGTH + RR_CLASS_LENGTH);
    }

    for (unsigned n = 0; n < ntohs(hdr->ancount); n++) {
        ssize_t tmp = _skip_hostname(buf, len, bufpos);
        if (tmp < 0) {
            return tmp;
        }
        bufpos += tmp;
        if ((bufpos + RR_TYPE_LENGTH + RR_CLASS_LENGTH +
             RR_TTL_LENGTH + sizeof(uint16_t)) >= buflim) {
            return -EBADMSG;
        }
        uint16_t _type = ntohs(_get_short(bufpos));
        bufpos += RR_TYPE_LENGTH;
        uint16_t class = ntohs(_get_short(bufpos));
        bufpos += RR_CLASS_LENGTH;
        bufpos += RR_TTL_LENGTH; /* skip ttl */

        unsigned addrlen = ntohs(_get_short(bufpos));
        /* skip unwanted answers */
        if ((class != DNS_CLASS_IN) ||
                ((_type == DNS_TYPE_A) && (family == AF_INET6)) ||
                ((_type == DNS_TYPE_AAAA) && (family == AF_INET)) ||
                ! ((_type == DNS_TYPE_A) || ((_type == DNS_TYPE_AAAA))
                    )) {
            if (addrlen > len) {
                /* buffer wraps around memory space */
                return -EBADMSG;
            }
            bufpos += addrlen;
            /* other out-of-bound is checked in `_skip_hostname()` at start of
             * loop */
            continue;
        }
        if (((addrlen != INADDRSZ) && (family == AF_INET)) ||
            ((addrlen != IN6ADDRSZ) && (family == AF_INET6)) ||
            ((addrlen != IN6ADDRSZ) && (addrlen != INADDRSZ) &&
             (family == AF_UNSPEC))) {
            return -EBADMSG;
        }
        bufpos += RR_RDLENGTH_LENGTH;
        if ((bufpos + addrlen) > buflim) {
            return -EBADMSG;
        }

        memcpy(addr_out, bufpos, addrlen);
        return addrlen;
    }

    return -1;
}

int sock_dns_query(const char *domain_name, void *addr_out, int family)
{
    static uint8_t dns_buf[SOCK_DNS_BUF_LEN];

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

    uint16_t id = 0; /* random? */
    for (int i = 0; i < SOCK_DNS_RETRIES; i++) {
        uint8_t *buf = dns_buf;

        sock_dns_hdr_t *hdr = (sock_dns_hdr_t*) buf;
        memset(hdr, 0, sizeof(*hdr));
        hdr->id = id;
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

        res = sock_udp_send(&sock_dns, buf, (bufpos-buf), NULL);
        if (res <= 0) {
            continue;
        }
        res = sock_udp_recv(&sock_dns, dns_buf, sizeof(dns_buf), 1000000LU, NULL);
        if (res > 0) {
            if (res > (int)DNS_MIN_REPLY_LEN) {
                if ((res = _parse_dns_reply(dns_buf, res, addr_out,
                                            family)) > 0) {
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
