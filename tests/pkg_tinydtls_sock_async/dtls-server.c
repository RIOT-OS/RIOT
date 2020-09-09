/*
 * Copyright (C) 2019 HAW Hamburg
 * Copyright (C) 2020 Freie Universität Berlin
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
 * @brief       DTLS sock server test
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include <stdio.h>

#include "bitfield.h"
#include "event/thread.h"
#include "event/timeout.h"
#include "net/sock/async/event.h"
#include "net/sock/udp.h"
#include "net/sock/dtls.h"
#include "net/credman.h"
#include "msg.h"
#include "thread.h"

#include "tinydtls_common.h"
#include "tinydtls_keys.h"

#define SOCK_DTLS_SERVER_TAG (10)
#define READER_QUEUE_SIZE (8U)

#ifdef CONFIG_DTLS_ECC
static const ecdsa_public_key_t other_pubkeys[] = {
    { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
};

static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_SERVER_TAG,
    .params = {
        .ecdsa = {
            .private_key = ecdsa_priv_key,
            .public_key = {
                .x = ecdsa_pub_key_x,
                .y = ecdsa_pub_key_y,
            },
            .client_keys = (ecdsa_public_key_t *)other_pubkeys,
            .client_keys_size = ARRAY_SIZE(other_pubkeys),
        },
    },
};
#else /* #ifdef CONFIG_DTLS_PSK */
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;

static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_SERVER_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
        },
    },
};
#endif

static void _close_handler(event_t *);

static uint8_t _recv_buf[512];
static sock_udp_t _udp_sock;
static sock_dtls_t _dtls_sock;
static tinydtls_sock_event_t _close = {
    .super.handler = _close_handler,
    .sock = &_dtls_sock,
};
static bool _active = false;

static void _close_sock(sock_dtls_t *sock)
{
    sock_udp_t *udp_sock = sock_dtls_get_udp_sock(sock);

    sock_dtls_close(sock);
    sock_udp_close(udp_sock);
    _active = false;
    puts("Terminated server");
}

static void _close_handler(event_t *ev)
{
    tinydtls_sock_event_t *event = (tinydtls_sock_event_t *)ev;
    puts("Terminating server");
    _close_sock(event->sock);
}

static void _dtls_handler(sock_dtls_t *sock, sock_async_flags_t type, void *arg)
{
    (void)arg;
    if (!_active) {
        puts("Error: DTLS server is not running");
        return;
    }

    if (type & SOCK_ASYNC_CONN_RECV) {
        sock_dtls_session_t session = { 0 };

        puts("Session handshake received");
        if (sock_dtls_recv(sock, &session, _recv_buf, sizeof(_recv_buf),
                           0) != -SOCK_DTLS_HANDSHAKE) {
            puts("Error creating session");
            return;
        }
        puts("New client connected");
    }
    if (type & SOCK_ASYNC_CONN_FIN) {
        puts("Session was destroyed by peer");
    }
    if (type & SOCK_ASYNC_CONN_RDY) {
        puts("Session became ready");
    }
    if (type & SOCK_ASYNC_MSG_RECV) {
        sock_dtls_session_t session = { 0 };
        ssize_t res;

        res = sock_dtls_recv(sock, &session, _recv_buf, sizeof(_recv_buf), 0);
        if (res >= 0) {
            printf("Received %d bytes -- (echo)\n", (int)res);
            res = sock_dtls_send(sock, &session, _recv_buf, (size_t)res, 0);
            if (res < 0) {
                printf("Error resending DTLS message: %d\n", (int)res);
            }
        }
    }
}

static int start_server(void)
{
    /* Only one instance of the server */
    if (_active) {
        puts("Error: server already running");
        return 1;
    }

    /* Start the server */
    ssize_t res;

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = DTLS_DEFAULT_PORT;
    sock_udp_create(&_udp_sock, &local, NULL, 0);

    res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("Error cannot add credential to system: %d\n", (int)res);
        return 1;
    }
    res = sock_dtls_create(&_dtls_sock, &_udp_sock, SOCK_DTLS_SERVER_TAG,
                           SOCK_DTLS_1_2, SOCK_DTLS_SERVER);
    if (res < 0) {
        puts("Error creating DTLS sock");
        return 1;
    }
    _active = true;

    sock_dtls_event_init(&_dtls_sock, &event_queue_medium, _dtls_handler,
                         NULL);
    return 0;
}

static int stop_server(void)
{
    /* check if server is running at all */
    if (!_active) {
        puts("Error: DTLS server is not running");
        return 1;
    }

    event_post(&event_queue_medium, &_close.super);
    return 0;
}

int dtls_server_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s start | stop\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "start") == 0) {
        return start_server();
    }
    else if (strcmp(argv[1], "stop") == 0) {
        return stop_server();
    }
    else {
        printf("Error: invalid command. Usage: %s start | stop\n", argv[0]);
        return 1;
    }
    return 0;
}
