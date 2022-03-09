/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#include "log.h"
#include "net/uhcp.h"

void uhcp_handle_udp(uint8_t *buf, size_t len, uint8_t *src, uint16_t port, uhcp_iface_t iface)
{
    char addr_str[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, src, addr_str, INET6_ADDRSTRLEN);
    LOG_DEBUG("got packet from %s port %u\n", addr_str, (unsigned)port);

    if (len < sizeof(uhcp_req_t)) {
        LOG_ERROR("error: packet too small.\n");
        return;
    }

    uhcp_hdr_t *hdr = (uhcp_hdr_t *)buf;

    if (! (ntohl(hdr->uhcp_magic) == UHCP_MAGIC)) {
        LOG_ERROR("error: wrong magic number.\n");
        return;
    }

    unsigned ver, type;
    ver = hdr->ver_type >> 4;
    type = hdr->ver_type & 0xF;

    if (ver != UHCP_VER) {
        LOG_ERROR("error: wrong protocol version.\n");
    }

    switch(type) {
#ifdef MODULE_UHCPD
        case UHCP_REQ:
            if (len < sizeof(uhcp_req_t)) {
                LOG_ERROR("error: request too small\n");
            }
            else {
                uhcp_handle_req((uhcp_req_t*)hdr, src, port, iface);
            }
            break;
#endif
#ifdef MODULE_UHCPC
        case UHCP_PUSH:
            {
                uhcp_push_t *push = (uhcp_push_t*)hdr;
                if ((len < sizeof(uhcp_push_t))
                    || (len < (sizeof(uhcp_push_t) + (push->prefix_len >> 3)))
                   ) {
                    LOG_ERROR("error: request too small\n");
                }
                else {
                    uhcp_handle_push(push, src, port, iface);
                }
                break;
            }
#endif
        default:
             LOG_ERROR("error: unexpected type\n");
    }
}

#ifdef MODULE_UHCPD
extern char _prefix[16];
extern unsigned _prefix_len;
void uhcp_handle_req(uhcp_req_t *req, uint8_t *src, uint16_t port, uhcp_iface_t iface)
{
    size_t prefix_bytes = (_prefix_len + 7)>>3;
    uint8_t packet[sizeof(uhcp_push_t) + prefix_bytes];

    uhcp_push_t *reply = (uhcp_push_t *)packet;
    uhcp_hdr_set(&reply->hdr, UHCP_PUSH);

    reply->prefix_len = _prefix_len;
    memcpy(reply->prefix, _prefix, prefix_bytes);

    int res = udp_sendto(packet, sizeof(packet), src, port, iface);
    if (res == -1) {
        LOG_ERROR("uhcp_handle_req(): udp_sendto() res=%i\n", res);
    }
}
#endif /* MODULE_UHCPD */

#ifdef MODULE_UHCPC
void uhcp_handle_push(uhcp_push_t *req, uint8_t *src, uint16_t port, uhcp_iface_t iface)
{
    char addr_str[INET6_ADDRSTRLEN];
    char prefix_str[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, src, addr_str, INET6_ADDRSTRLEN);
    uint8_t prefix[IN6ADDRSZ] = { 0 };
    /* prefix_len can't be 0 or greater then IN6ADDRSZ * 8 (== 128) */
    if ((req->prefix_len == 0) || (req->prefix_len > (IN6ADDRSZ << 3))) {
        LOG_ERROR("uhcp_handle_push(): invalid prefix length\n");
        return;
    }
    size_t prefix_bytes = (req->prefix_len + 7)>>3;
    memcpy(prefix, req->prefix, prefix_bytes);

    inet_ntop(AF_INET6, prefix, prefix_str, INET6_ADDRSTRLEN);

    LOG_DEBUG("uhcp: push from %s:%u prefix=%s/%u\n", addr_str, (unsigned)port,
             prefix_str, req->prefix_len);
    uhcp_handle_prefix(prefix, req->prefix_len, 0xFFFF, src, iface);
}
#endif
