/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_drivers_dtls
 * @brief   Transport implementation of CoAP over DTLS driver
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <stdint.h>
#include <errno.h>
#include "event.h"
#include "net/sock.h"
#include "net/sock/dtls.h"
#include "net/sock/async/types.h"
#include "net/sock/async/event.h"
#include "net/credman.h"
#include "net/dsm.h"
#include "net/unicoap/transport.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define _DTLS_DEBUG(...) _UNICOAP_PREFIX_DEBUG(".transport.dtls", __VA_ARGS__)
#define _DTLS_AUTH_DEBUG(...) _UNICOAP_PREFIX_DEBUG(".transport.dtls.auth", __VA_ARGS__)

UNICOAP_DECL_RECEIVER_STORAGE_EXTERN;

#define SOCK_DTLS_CLIENT_TAG (2)
static sock_udp_t _dtls_base_socket;
static sock_dtls_t _dtls_socket;

unicoap_scheduled_event_t _dtls_session_triage_event = { 0 };

/* tinydtls does not support auxiliary data
 * see https://github.com/RIOT-OS/RIOT/issues/16054 */
#if IS_ACTIVE(CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS)
#  pragma message "warning: tinydtls does not support retrieving local endpoints. \
           CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS will be ignored for the dtls transport"
#endif

/* Timeout function to free a session when too many session slots are occupied */
static void _dtls_session_triage(unicoap_scheduled_event_t* event)
{
    (void)event;
    sock_dtls_session_t session;
    if (dsm_get_num_available_slots() < CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS) {
        if (dsm_get_least_recently_used_session(&_dtls_socket, &session) != -1) {
            _DTLS_DEBUG("session triage: freeing least recently used session\n");
            dsm_remove(&_dtls_socket, &session);
            sock_dtls_session_destroy(&_dtls_socket, &session);
        }
    }
}

static void _dtls_on_event(sock_dtls_t* sock, sock_async_flags_t type, void* arg) {
    (void)arg;
    sock_dtls_session_t session = { 0 };

    /* This logic heavily depends on the tinydtls backend. At the moment, no other DTLS  
     * backend other than tinydtls exists. As soon as another becomes supported,
     * this logic needs to be adjusted to strictly follow the sock_dtls API.
     * For now, we need work around tinydtls quirks. */

    if (type & SOCK_ASYNC_CONN_RECV) {
        _DTLS_DEBUG("establishing session\n");
        /* finish handshake as per API contract of @ref sock_dtls_session_init.
         * use non-buf function with less parameters */
        uint8_t buf[1];
        ssize_t res = sock_dtls_recv(sock, &session, buf, sizeof(buf), 0);

        if (res != -SOCK_DTLS_HANDSHAKE) {
            _DTLS_DEBUG("could not establish DTLS session: %" PRIiSIZE " (%s)\n", res,
                        strerror(-(int)res));
            goto error;
        }

        dsm_state_t prev_state = dsm_store(sock, &session, SESSION_STATE_ESTABLISHED, false);
        if (prev_state == SESSION_STATE_HANDSHAKE) {
            _DTLS_DEBUG("established, telling messaging can resume\n");
            unicoap_endpoint_t remote = { .proto = UNICOAP_PROTO_DTLS };
            sock_dtls_session_get_udp_ep(&session, unicoap_endpoint_get_dtls(&remote));
            unicoap_packet_t packet = { .remote = &remote, .dtls_session = &session };
            unicoap_messaging_process_rfc7252(NULL, 0, UNICOAP_MESSAGING_RFC7252_EVENT_SESSION_ESTABLISHED, &packet);
        }
        else if (prev_state == NO_SPACE) {
            /* No space in session management. Should not happen. If it occurs,
             * we lost track of sessions. */
            _DTLS_DEBUG("no space in session management\n");
            goto error;
        }

        /* If not enough session slots left: set timeout to free session. */
        if (dsm_get_num_available_slots() < CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS) {
            _DTLS_DEBUG("session triage: fewer than %u session slots available,"
                       " limiting session lifespan to %" PRIu32 " ms\n",
                       (unsigned int)CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS,
                       (uint32_t)CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS_TIMEOUT_MS);
            unicoap_event_schedule(&_dtls_session_triage_event, _dtls_session_triage,
                                   CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS_TIMEOUT_MS,
                                   "messaging.dtls.triage");
        }
    }

    if (type & SOCK_ASYNC_CONN_RDY) {
        _DTLS_DEBUG("connection ready\n");
    }

    if (type & SOCK_ASYNC_MSG_RECV) {
        _DTLS_DEBUG("received encrypted datagram\n");
        sock_dtls_aux_rx_t aux_rx = {
            .flags = IS_ACTIVE(CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS) ? SOCK_AUX_GET_LOCAL : 0,
        };

        void* pdu = NULL;
        void* buffer_ctx = NULL;

        ssize_t received = sock_dtls_recv_buf_aux(sock, &session, &pdu, &buffer_ctx, 0, &aux_rx);
        if (received < 0) {
            _DTLS_DEBUG("recv failure: %" PRIdSIZE "\n", received);
            return;
        }

        assert(pdu);

        unicoap_endpoint_t remote = { .proto = UNICOAP_PROTO_DTLS };
        sock_dtls_session_get_udp_ep(&session, unicoap_endpoint_get_dtls(&remote));

        unicoap_packet_t packet = { .remote = &remote, .dtls_session = &session };

/* tinydtls does not support auxiliary data
 * see https://github.com/RIOT-OS/RIOT/issues/16054 */
#if false && IS_ACTIVE(CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS)
        assert((aux_rx.flags & SOCK_AUX_GET_LOCAL) == 0);
        unicoap_endpoint_t local = { .proto = UNICOAP_PROTO_DTLS };
        packet.local = &local;

        if (aux_rx.local.family != AF_UNSPEC) {
            *unicoap_endpoint_get_udp(&local) = aux_rx.local;
        }
#endif

        /* Truncated DTLS messages would already have gotten lost at verification */
        unicoap_messaging_process_rfc7252((uint8_t*)pdu, received, UNICOAP_MESSAGING_RFC7252_EVENT_RX, &packet);

        received = sock_dtls_recv_buf_aux(sock, &session, &pdu, &buffer_ctx, 0, &aux_rx);
        /* If the networking backends holds its zero-copy guarantee, then trying to read
         * another chunk must not yield any more data. */
        assert(received == 0);
    }

    if (type & SOCK_ASYNC_CONN_FIN) {
        if (sock_dtls_get_event_session(sock, &session)) {
            /* Session is already destroyed, only remove it from dsm */
            dsm_remove(sock, &session);
        }
        else {
            _DTLS_DEBUG("session was closed, but the corresponding session "
                       "could not be retrieved from the socket\n");
            return;
        }

        _DTLS_DEBUG("session ended, removing associated endpoint state\n");

        unicoap_endpoint_t endpoint = { .proto = UNICOAP_PROTO_DTLS };
        sock_dtls_session_get_udp_ep(&session, unicoap_endpoint_get_dtls(&endpoint));
        unicoap_exchange_notify_all(&endpoint, unicoap_layer_notification_async_failure_from_errno(ECONNABORTED), NULL);
    }

    return;

error:
    sock_dtls_session_destroy(sock, &session);
}

int unicoap_transport_connect_dtls(const sock_udp_ep_t* remote, sock_dtls_session_t* session) {
    assert(remote);
    assert(session);
    int res = 0;
    _DTLS_AUTH_DEBUG("connecting...\n");
    sock_dtls_session_set_udp_ep(session, remote);
    dsm_state_t session_state = dsm_store(&_dtls_socket, session, SESSION_STATE_HANDSHAKE, true);
    switch (session_state) {
        case SESSION_STATE_ESTABLISHED:
            _DTLS_AUTH_DEBUG("session established\n");
            return -EEXIST;
        case SESSION_STATE_NONE:
            _DTLS_AUTH_DEBUG("session not established\n");
            if ((res = sock_dtls_session_init(&_dtls_socket, remote, session)) < 0) {
                _DTLS_AUTH_DEBUG("init DTLS session failed: %i (%s)\n", (int)res, strerror(-(int)res));
                return res;
            }
            /* Need to wait until session is established. */
            return 0;
        case SESSION_STATE_HANDSHAKE:
            _DTLS_AUTH_DEBUG("handshaking\n");
            /* Need to wait until handshake is done. */
            return 0;
        case NO_SPACE:
            _DTLS_AUTH_DEBUG("DTLS session mgmt full\n");
            return -ENOBUFS;
        default:
            UNREACHABLE();
            assert(false);
            return -1;
    }
}

int unicoap_transport_sendv_dtls(iolist_t* iolist, const sock_udp_ep_t* remote,
                                 const sock_udp_ep_t* local, sock_dtls_session_t* session)
{
    assert(remote);
    ssize_t res = 0;

    _DTLS_DEBUG("sendv\n");
    sock_dtls_session_set_udp_ep(session, remote);
    /* Get session state to assert session has been established before using connect(). */
    dsm_state_t session_state = dsm_store(&_dtls_socket, session, SESSION_STATE_HANDSHAKE, true);
    (void)session_state;
    if (IS_ACTIVE(DEVELHELP) && session_state != SESSION_STATE_ESTABLISHED) {
        _DTLS_DEBUG("FATAL ERROR: session not established, call connect() before\n");
    }
    assert(session_state == SESSION_STATE_ESTABLISHED);

    if (unlikely(local)) {
        sock_dtls_aux_tx_t aux_tx = { .flags = SOCK_AUX_SET_LOCAL, .local = *local };
        res = sock_dtls_sendv_aux(&_dtls_socket, session, iolist,
                                  CONFIG_UNICOAP_DTLS_HANDSHAKE_TIMEOUT_MS * US_PER_MS, &aux_tx);
    }
    else {
        res = sock_dtls_sendv_aux(&_dtls_socket, session, iolist,
                                  CONFIG_UNICOAP_DTLS_HANDSHAKE_TIMEOUT_MS * US_PER_MS, NULL);
    }

    if (res <= 0) {
        switch (res) {
        case -EHOSTUNREACH:
        case -ENOTCONN:
        case 0:
            _DTLS_DEBUG("sock not connected or remote unreachable, tearing down session\n");
            dsm_remove(&_dtls_socket, session);
            sock_dtls_session_destroy(&_dtls_socket, session);
            break;
        default:
            /* Temporary error, keep DTLS session. */
            _DTLS_DEBUG("error: %s\n", strerror(-res));
            return res;
        }
    }
    return 0;
}

static int _add_socket(event_queue_t* queue, sock_dtls_t* socket, sock_udp_t* base_socket,
                       sock_udp_ep_t* local)
{
    _DTLS_DEBUG("creating DTLS sock, port=%" PRIu16 " if=%" PRIu16 " family=%s\n", local->port,
               local->netif,
               local->family == AF_INET6 ? "inet6" : (local->family == AF_INET ? "inet" : "?"));

    if (sock_udp_create(base_socket, local, NULL, 0)) {
        _DTLS_DEBUG("error creating DTLS base (UDP) sock\n");
        return 0;
    }
    /* tinydtls as a backend does not care about role, fine to reuse same socket for client */
    if (sock_dtls_create(socket, base_socket, CREDMAN_TAG_EMPTY, SOCK_DTLS_1_2,
                         SOCK_DTLS_SERVER) < 0) {
        _DTLS_DEBUG("error creating DTLS sock\n");
        sock_udp_close(base_socket);
        return -1;
    }
    sock_dtls_event_init(socket, queue, _dtls_on_event, NULL);

    return 0;
}

int unicoap_init_dtls(event_queue_t* queue)
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
        .port = CONFIG_UNICOAP_DTLS_PORT
    };

    return _add_socket(queue, &_dtls_socket, &_dtls_base_socket, &local);
}

sock_dtls_t* unicoap_transport_dtls_get_socket(void)
{
    return &_dtls_socket;
}

int unicoap_transport_dtls_add_socket(sock_dtls_t* socket,
                                      sock_udp_t* base_socket,
                                      sock_udp_ep_t* local) {
    return _add_socket(sock_dtls_get_async_ctx(&_dtls_socket)->queue, socket, base_socket, local);
}

int unicoap_transport_dtls_remove_socket(sock_dtls_t* socket) {
    sock_udp_t* udp_socket = socket->udp_sock;
    sock_dtls_close(socket);
    sock_udp_close(udp_socket);
    return 0;
}

int unicoap_deinit_dtls(event_queue_t* queue)
{
    (void)queue;
    sock_dtls_close(&_dtls_socket);
    sock_udp_close(&_dtls_base_socket);
    return 0;
}
