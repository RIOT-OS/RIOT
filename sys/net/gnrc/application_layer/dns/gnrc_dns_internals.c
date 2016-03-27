/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup net_gnrc_dns
 * @{
 *
 * @file
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#include <errno.h>
#include <string.h>
#include <time.h>

#include "net/gnrc/dns/internal.h"
#include "net/gnrc/dns.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/udp.h"

#define DNS_COMP_MASK               0xc0
#define DNS_COMPRESSED(x)           (((x) & DNS_COMP_MASK) == DNS_COMP_MASK)
#define DNS_COMP_OFFSET(a, b)       (((uint16_t)(a & ~(DNS_COMP_MASK)) << 8) + (uint16_t)b)

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

size_t _gnrc_dns_calc_snippet_len(gnrc_dns_request_t *req)
{
    /* calculate the length of the UDP snippet for the given query request */
    return strlen(req->query_str) + sizeof(dns_header_t)
           + sizeof(dns_qdata_footer_t) + 2;
}

bool _gnrc_dns_build_request(gnrc_dns_request_t *req, uint8_t *buf,
                             size_t *length)
{
    dns_header_t *header = (dns_header_t *) buf;

    /* validate the packet length */
    if (_gnrc_dns_calc_snippet_len(req) > *length) {
        return false;
    }

    /* fill the DNS header */
    memset(header, 0, sizeof(*header));
    header->id = HTONS(req->request_id);
    header->qd_count = CT_HTONS(1);
    header->flags = HDR_FLAGS_RD;

    /* fill the query part */
    char *data = (char *) (header + 1);

    /* copy the string to the query buffer */
    strcpy(data + 1, req->query_str);

    /* start converting all domain name parts to pascal strings */
    /* eg. replace all dots with the length of the next part */
    char *end;
    char *start = data + 1;
    while ((end = strchr(start, '.')) != NULL) {
        *data = end - start;
        data = end;
        start = end + 1;
    }

    /* finalize the domain name strings */
    *data = strlen(start);
    data += (*data) + 1;
    *data++ = 0;

    /* fill query footer */
    dns_qdata_footer_t *ftr = (dns_qdata_footer_t *) data;
    ftr->type = req->query_type;
    ftr->class = DNS_SQ_IN;

    /* get the pointer after the footer */
    data += sizeof(*ftr);

    /* calculate the total length of the packet */
    if ((size_t)(data - (char *)buf) > *length) {
        DEBUG("ERROR: DNS snippet overflowed buffer!\n");
    }

    *length = ((uint8_t *) data - buf);
    return buf;
}

void _gnrc_dns_send_request(msg_t *m)
{
    gnrc_dns_request_t *req = (gnrc_dns_request_t *) m->content.ptr;

    size_t len = _gnrc_dns_calc_snippet_len(req);
    uint8_t buffer[len];

    /* build the query and send it to the destination */
    _gnrc_dns_build_request(req, buffer, (size_t *) &len);
    _gnrc_dns_send_packet(req, buffer, len);

    /* start the timeout timer for the transaction */
    req->timeout_msg.content.ptr = (void *)req;
    req->timeout_msg.type = GNRC_DNS_MSG_RESPONSE_TIMEDOUT;
    xtimer_set_msg(&req->timeout_timer, GNRC_DNS_RESPONSE_TIMEOUT * 1000, &(req->timeout_msg), _dns_pid);
}

void _gnrc_dns_send_packet(gnrc_dns_request_t *req, uint8_t *packet,
                           size_t length)
{
    gnrc_pktsnip_t *payload, *udp, *ip;

    /* allocate payload */
    payload = gnrc_pktbuf_add(NULL, packet, length, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        DEBUG("dns: error unable to copy data to packet buffer");
        return;
    }

    /* allocate UDP header, set source port := destination port */
    network_uint16_t src_port, dst_port;
    src_port.u16 = GNRC_DNS_RESOLVE_SRC_PORT;
    dst_port.u16 = GNRC_DNS_RESOLVE_DST_PORT;
    udp = gnrc_udp_hdr_build(payload, src_port.u8, sizeof(src_port),
                             dst_port.u8, sizeof(dst_port));
    if (udp == NULL) {
        DEBUG("dns: error unable to allocate UDP header");
        gnrc_pktbuf_release(payload);
        return;
    }

    /* allocate IPv6 header */
    mutex_lock(&(_dns_context.mutex));
    /* iterate over the available DNS servers from primary to last */
    int dns_server = (GNRC_DNS_MAX_RETRIES - req->tries_left) % _dns_context.count;
    ip = gnrc_ipv6_hdr_build(udp, NULL, 0,
                             _dns_context.servers[dns_server].addr.u8,
                             sizeof(ipv6_addr_t));
    mutex_unlock(&(_dns_context.mutex));

    if (ip == NULL) {
        DEBUG("dns: error unable to allocate IPv6 header");
        gnrc_pktbuf_release(udp);
        return;
    }

    /* send packet */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL,
                                   ip)) {
        DEBUG("dns: error unable to locate UDP thread");
        gnrc_pktbuf_release(ip);
        return;
    }
}

bool _gnrc_dns_handle_request_retry(msg_t *m, gnrc_dns_type_t error)
{
    msg_t r;
    gnrc_dns_request_t *req = (gnrc_dns_request_t *) m->content.ptr;

    /* check if we must retry the transaction */
    if (!--(req->tries_left)) {
        msg_t om;
        om.sender_pid = req->request_thread;

        DEBUG("dns: inform requester that the request timed out.\n");

        /* inform that the request timed out */
        req->response->type = error;

        /* prepare the reply message */
        r.type = GNRC_DNS_MSG_QUERY_RESPONSE;
        r.content.value = 1;

        /* send the reply message */
        msg_reply(&om, &r);

        return false;
    }
    else {
        DEBUG("dns: DNS query timed out, retrying.\n");

        /* retry the DNS query */
        _gnrc_dns_send_request(m);

        return true;
    }
}

uint8_t *_gnrc_dns_parse_name(uint8_t *packet_start, uint8_t *data, char *domain)
{
    uint8_t *ret = NULL;
    int offset = 0;
    int seglen = *data++;

    /* check if the arguments are valid */
    if (!packet_start || !data) {
        return NULL;
    }

    /* parse the domain name and turn back to . separated string */
    while (seglen) {

        /* process compressed string if seglen is equal to 0xc0 */
        if (DNS_COMPRESSED(seglen)) {
            /* string finishes after next byte */
            ret = data + 1;

            /* continue processing in a previous part of the packet */
            data = packet_start + DNS_COMP_OFFSET(seglen, *data);
            seglen = *data++;

            continue;
        }

        if (domain) {
            /* check if the string is not longer than the maximum query length */
            if ((offset + seglen) > GNRC_DNS_MAX_QUERY_STR_LEN) {
                return NULL;
            }

            memcpy(domain + offset, data, seglen);
        }

        offset += seglen;
        data += seglen;

        if (domain) {
            *(domain + offset) = '.';
        }

        seglen = *data++;

        if (seglen) {
            ++offset;
        }
    }

    /* zero terminate our string */
    if (domain) {
        *(domain + offset) = 0;
    }

    return ret
           ? ret
           : data;
}

/**
 * @}
 */
