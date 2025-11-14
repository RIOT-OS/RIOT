/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup net_unicoap_drivers_udp
 * @brief   Transport implementation of CoAP over UDP driver
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <stdint.h>
#include <errno.h>
#include "architecture.h"
#include "net/unicoap/transport.h"
#include "net/sock.h"
#include "net/sock/async/types.h"
#include "net/sock/async/event.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define UDP_DEBUG(...) _UNICOAP_PREFIX_DEBUG(".transport.udp", __VA_ARGS__)

UNICOAP_DECL_RECEIVER_STORAGE_EXTERN;

static sock_udp_t _udp_socket;

extern int unicoap_messaging_process_rfc7252(const uint8_t* pdu, size_t size, bool truncated,
                                             unicoap_packet_t* packet);

static void _udp_on_event(sock_udp_t* sock, sock_async_flags_t type, void* arg)
{
    (void)arg;
    unicoap_endpoint_t remote = {
        .proto = UNICOAP_PROTO_UDP,
    };

    if (type & SOCK_ASYNC_MSG_RECV) {
        void* stackbuf = NULL;
        uint8_t* pdu;
        void* buffer_ctx = NULL;
        bool truncated = false;
        ssize_t received = 0;

        sock_udp_aux_rx_t aux_rx = {
            .flags = IS_ACTIVE(CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS) ? SOCK_AUX_GET_LOCAL : 0,
        };

        if (IS_ACTIVE(CONFIG_UNICOAP_SOCK_ZERO_COPY_GUARANTEES)) {
            received = sock_udp_recv_buf_aux(sock, &stackbuf, &buffer_ctx, 0,
                                             unicoap_endpoint_get_udp(&remote), &aux_rx);

            if (received < 0) {
                UDP_DEBUG("recv failure: %" PRIdSIZE "\n", received);
                return;
            }
            if (received == 0) {
                return;
            }
            assert(stackbuf);
            pdu = stackbuf;
        }
        else {
            pdu = unicoap_receiver_buffer;
            while (true) {
                ssize_t chunk_size = sock_udp_recv_buf_aux(
                    sock, &stackbuf, &buffer_ctx, 0, unicoap_endpoint_get_udp(&remote), &aux_rx);
                if (chunk_size < 0) {
                    UDP_DEBUG("recv failure: %" PRIdSIZE "\n", chunk_size);
                    return;
                }
                if (chunk_size == 0) {
                    break;
                }

                truncated = unicoap_transport_truncate_received((size_t*)&chunk_size, received);
                memcpy(&pdu[received], stackbuf, chunk_size);
                received += chunk_size;
            }
        }

        if (received == 0) {
            return;
        }

        unicoap_packet_t packet = { .remote = &remote };

#     if IS_ACTIVE(CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS)
        unicoap_endpoint_t local = { .proto = UNICOAP_PROTO_UDP };
        packet.local = &local;

        if (aux_rx.local.family != AF_UNSPEC) {
            *unicoap_endpoint_get_udp(&local) = aux_rx.local;
        }
#     endif

        unicoap_messaging_process_rfc7252(pdu, (size_t)received, truncated, &packet);

        if (IS_ACTIVE(CONFIG_UNICOAP_SOCK_ZERO_COPY_GUARANTEES)) {
            received = sock_udp_recv_buf_aux(sock, &stackbuf, &buffer_ctx, 0,
                                             unicoap_endpoint_get_udp(&remote), &aux_rx);
            /* If the networking backends holds its zero-copy guarantee, then trying to read
             * another chunk must not yield any more data. */
            assert(received == 0);
        }
    }
}

int unicoap_transport_sendv_udp(iolist_t* iolist, const sock_udp_ep_t* remote,
                                const sock_udp_ep_t* local)
{
    assert(remote);
    assert(iolist);

    UDP_DEBUG("sendv: %" PRIuSIZE " bytes\n", iolist_size(iolist));

    int res = 0;
    if (unlikely(local)) {
        sock_udp_aux_tx_t aux_tx = { .flags = SOCK_AUX_SET_LOCAL, .local = *local };
        res = (int)sock_udp_sendv_aux(&_udp_socket, iolist, remote, &aux_tx);
    }
    else {
        res = (int)sock_udp_sendv_aux(&_udp_socket, iolist, remote, NULL);
    }

    if (res < 0) {
        UDP_DEBUG("udp_sendv_aux failed: %i\n", res);
    }

    return res;
}

static int _add_socket(event_queue_t* queue, sock_udp_t* socket, sock_udp_ep_t* local)
{
    UDP_DEBUG("zero_copy_guarantees=%u creating UDP sock, port=%" PRIu16 " if=%" PRIu16
              " family=%s\n",
              CONFIG_UNICOAP_SOCK_ZERO_COPY_GUARANTEES, local->port, local->netif,
              local->family == AF_INET6 ? "inet6" : (local->family == AF_INET ? "inet" : "?"));

    int res = sock_udp_create(socket, local, NULL, 0);
    if (res < 0) {
        UDP_DEBUG("cannot create sock: %d (%s)\n", res, strerror(-res));
        return res;
    }

    sock_udp_event_init(socket, queue, _udp_on_event, NULL);

    return 0;
}

int unicoap_init_udp(event_queue_t* queue)
{
    sock_udp_ep_t local = {
    /* FIXME: Once the problems with IPv4/IPv6 dual stack use in RIOT are fixed, adapt these lines
         *        (and e.g. use AF_UNSPEC) */
#if defined(SOCK_HAS_IPV6)
        .family = AF_INET6,
#elif defined(SOCK_HAS_IPV4)
        .family = AF_INET,
#endif
        .netif = SOCK_ADDR_ANY_NETIF,
        .port = CONFIG_UNICOAP_UDP_PORT
    };

    return _add_socket(queue, &_udp_socket, &local);
}

sock_udp_t* unicoap_transport_udp_get_socket(void)
{
    return &_udp_socket;
}

int unicoap_transport_udp_add_socket(sock_udp_t* socket, sock_udp_ep_t* local) {
    return _add_socket(sock_udp_get_async_ctx(&_udp_socket)->queue, socket, local);
}

int unicoap_transport_udp_remove_socket(sock_udp_t* socket) {
    sock_udp_close(socket);
    return 0;
}

int unicoap_deinit_udp(event_queue_t* queue)
{
    (void)queue;
    sock_udp_close(&_udp_socket);
    return 0;
}
