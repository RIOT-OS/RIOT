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

UNICOAP_DECL_RECEIVER_STORAGE_EXTERN;

#define SOCK_DTLS_CLIENT_TAG (2)
static sock_udp_t _dtls_base_socket;
static sock_dtls_t _dtls_socket;
static kernel_pid_t _dtls_auth_waiting_thread;

unicoap_scheduled_event_t _dtls_session_triage_event = { 0 };

extern int unicoap_messaging_process_rfc7252(const uint8_t* pdu, size_t size, bool truncated,
                                             unicoap_packet_t* packet);

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

static void _dtls_on_event(sock_dtls_t* sock, sock_async_flags_t type, void* arg)
{
    (void)arg;
    sock_dtls_session_t session = { 0 };

    if (type & SOCK_ASYNC_CONN_RECV) {
        _DTLS_DEBUG("establishing session\n");
        uint8_t buf[1];
        ssize_t res = sock_dtls_recv(sock, &session, buf, sizeof(buf),
                                     CONFIG_UNICOAP_DTLS_HANDSHAKE_TIMEOUT_MS * US_PER_MS);

        if (res != -SOCK_DTLS_HANDSHAKE) {
            _DTLS_DEBUG("could not establish DTLS session: %" PRIiSIZE " (%s)\n", res,
                       strerror(-(int)res));
            goto error;
        }

        dsm_state_t prev_state = dsm_store(sock, &session, SESSION_STATE_ESTABLISHED, false);

        /* If session is already stored and the state was SESSION_STATE_HANDSHAKE
         * before, the handshake has been initiated internally by a client request
         * and another thread is waiting for the handshake. Send message to the
         * waiting thread to inform about established session */
        if (prev_state == SESSION_STATE_HANDSHAKE) {
            msg_t msg = { .type = DTLS_EVENT_CONNECTED };
            msg_send(&msg, _dtls_auth_waiting_thread);
        }
        else if (prev_state == NO_SPACE) {
            /* No space in session management. Should not happen. If it occurs,
             * we lost track of sessions. */
            _DTLS_DEBUG("no space in session management\n");
            goto error;
        }

        /* If not enough session slots left: set timeout to free session. */
        if (dsm_get_num_available_slots() < CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS) {
            _DTLS_DEBUG("session triage: fewer than %u session slots available in session mgmt,"
                       " limiting session lifespan to %" PRIu32 " ms\n",
                       (unsigned int)CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS,
                       (uint32_t)CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS_TIMEOUT_MS);
            unicoap_event_schedule(&_dtls_session_triage_event, _dtls_session_triage,
                                   CONFIG_UNICOAP_DTLS_MINIMUM_AVAILABLE_SESSION_SLOTS_TIMEOUT_MS);
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

#if IS_ACTIVE(CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS)
        unicoap_endpoint_t local = { .proto = UNICOAP_PROTO_DTLS };
        packet.local = &local;

        if (aux_rx.local.family != AF_UNSPEC) {
            *unicoap_endpoint_get_udp(&local) = aux_rx.local;
        }
#endif

        /* Truncated DTLS messages would already have gotten lost at verification */
        unicoap_messaging_process_rfc7252((uint8_t*)pdu, received, false, &packet);

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
        unicoap_exchange_release_endpoint_state(&endpoint);
    }

    return;

error:
    sock_dtls_session_destroy(sock, &session);
}

static ssize_t _dtls_authenticate(const sock_udp_ep_t* remote, sock_dtls_session_t* session,
                                  uint32_t timeout)
{
    assert(session);
    int res;

    /* prepare session */
    sock_dtls_session_set_udp_ep(session, remote);
    dsm_state_t session_state = dsm_store(&_dtls_socket, session, SESSION_STATE_HANDSHAKE, true);
    if (session_state == SESSION_STATE_ESTABLISHED) {
        _DTLS_DEBUG("auth: session already established\n");
        return 0;
    }
    if (session_state == NO_SPACE) {
        _DTLS_DEBUG("auth: no space in DTLS session mgmt\n");
        return -ENOBUFS;
    }

    /* start handshake */
    _dtls_auth_waiting_thread = thread_getpid();
    res = sock_dtls_session_init(&_dtls_socket, remote, session);
    if (res == 0) {
        /* session already exists */
        _dtls_auth_waiting_thread = -1;
        return res;
    }

    msg_t msg;
    bool is_timed_out = false;
    do {
        uint32_t start = ztimer_now(ZTIMER_MSEC);
        res = ztimer_msg_receive_timeout(ZTIMER_MSEC, &msg, timeout);

        /* ensure whole timeout time for the case we receive other messages than
         * DTLS_EVENT_CONNECTED */
        if (timeout != SOCK_NO_TIMEOUT) {
            uint32_t diff = (ztimer_now(ZTIMER_MSEC) - start);
            timeout = (diff > timeout) ? 0 : timeout - diff;
            is_timed_out = (res < 0) || (timeout == 0);
        }
    } while (!is_timed_out && (msg.type != DTLS_EVENT_CONNECTED));

    if (is_timed_out && (msg.type != DTLS_EVENT_CONNECTED)) {
        _DTLS_DEBUG("auth: timeout\n");
        dsm_remove(&_dtls_socket, session);
        sock_dtls_session_destroy(&_dtls_socket, session);
        return -ENOTCONN;
    }
    return 0;
}

int unicoap_transport_sendv_dtls(iolist_t* iolist, const sock_udp_ep_t* remote,
                                 const sock_udp_ep_t* local, sock_dtls_session_t* session)
{
    assert(remote);
    ssize_t res = 0;

    if (!session) {
        if ((res = _dtls_authenticate(remote, session, CONFIG_UNICOAP_DTLS_HANDSHAKE_TIMEOUT_MS))
            < 0) {
            return res;
        }
    }

    // todo: not needed, already done in _dtls_authenticate, or session is already established?!
    // /* prepare session */
    // sock_dtls_session_set_udp_ep(session, remote);
    // dsm_state_t session_state = dsm_store(&_dtls_socket, session, SESSION_STATE_HANDSHAKE, true);
    // if (session_state == NO_SPACE) {
    //     return -1;
    // }

    _DTLS_DEBUG("started sending\n");

    if (unlikely(local)) {
        sock_dtls_aux_tx_t aux_tx = { .flags = SOCK_AUX_SET_LOCAL, .local = *local };
        res = sock_dtls_sendv_aux(&_dtls_socket, session, iolist, 500000, &aux_tx);
    }
    else {
        res = sock_dtls_sendv_aux(&_dtls_socket, session, iolist,
                                  CONFIG_UNICOAP_DTLS_HANDSHAKE_TIMEOUT_MS * US_PER_MS, NULL);
    }
    _DTLS_DEBUG("done sending\n");

    switch (res) {
    case -EHOSTUNREACH:
    case -ENOTCONN:
    case 0:
        _DTLS_DEBUG("DTLS sock not connected or remote unreachable. "
                   "Destroying session.\n");
        dsm_remove(&_dtls_socket, session);
        sock_dtls_session_destroy(&_dtls_socket, session);
        break;
    default:
        /* Temporary error. Keeping the DTLS session */
        break;
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
