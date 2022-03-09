/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       DTLS sock server example
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include <stdio.h>

#include "net/sock/udp.h"
#include "net/sock/dtls.h"
#include "net/sock/dtls/creds.h"
#include "net/sock/util.h"
#include "net/credman.h"
#include "msg.h"
#include "thread.h"
#include "timex.h"

#include "tinydtls_keys.h"

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT (20220) /* DTLS default port */
#endif

#define SOCK_DTLS_SERVER_TAG_0 (10)
#define SOCK_DTLS_SERVER_TAG_1 (11)
#define DTLS_STOP_SERVER_MSG 0x4001 /* Custom IPC type msg. */
#define READER_QUEUE_SIZE (8U)

char _dtls_server_stack[THREAD_STACKSIZE_MAIN +
                        THREAD_EXTRA_STACKSIZE_PRINTF];

static kernel_pid_t _dtls_server_pid = KERNEL_PID_UNDEF;

#ifdef CONFIG_DTLS_ECC
static const ecdsa_public_key_t other_pubkeys[] = {
    { .x = ecdsa_pub_key0_x, .y = ecdsa_pub_key0_y },
};

static const credman_credential_t credential0 = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_SERVER_TAG_0,
    .params = {
        .ecdsa = {
            .private_key = ecdsa_priv_key0,
            .public_key = {
                .x = ecdsa_pub_key0_x,
                .y = ecdsa_pub_key0_y,
            },
            .client_keys = (ecdsa_public_key_t *)other_pubkeys,
            .client_keys_size = ARRAY_SIZE(other_pubkeys),
        },
    },
};

static const ecdsa_public_key_t other_pubkeys1[] = {
    { .x = ecdsa_pub_key1_x, .y = ecdsa_pub_key1_y },
};

static const credman_credential_t credential1 = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_SERVER_TAG_1,
    .params = {
        .ecdsa = {
            .private_key = ecdsa_priv_key1,
            .public_key = {
                .x = ecdsa_pub_key1_x,
                .y = ecdsa_pub_key1_y,
            },
            .client_keys = (ecdsa_public_key_t *)other_pubkeys1,
            .client_keys_size = ARRAY_SIZE(other_pubkeys1),
        }
    },
};

static credman_tag_t _rpk_cb(sock_dtls_t *sock, sock_udp_ep_t *ep, credman_tag_t tags[],
                             unsigned tags_len)
{
    (void) sock;
    (void) ep;
    (void) tags;
    (void) tags_len;

    char addrstr[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;

    sock_udp_ep_fmt(ep, addrstr, &port);
    printf("From [%s]:%d\n", addrstr, port);

    return SOCK_DTLS_SERVER_TAG_1;
}

#else /* #ifdef CONFIG_DTLS_PSK */
static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;

static const credman_credential_t credential0 = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_SERVER_TAG_0,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        },
    },
};
#endif

void *dtls_server_wrapper(void *arg)
{
    (void) arg;

    ssize_t res;
    bool active = true;
    msg_t _reader_queue[READER_QUEUE_SIZE];
    msg_t msg;
    uint8_t rcv[512];

    /* Prepare (thread) messages reception */
    msg_init_queue(_reader_queue, READER_QUEUE_SIZE);

    sock_dtls_t sock;
    sock_udp_t udp_sock;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = DTLS_DEFAULT_PORT;
    sock_udp_create(&udp_sock, &local, NULL, 0);

    res = credman_add(&credential0);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("Error cannot add credential to system: %d\n", (int)res);
        return NULL;
    }

    res = sock_dtls_create(&sock, &udp_sock, SOCK_DTLS_SERVER_TAG_0,
                           SOCK_DTLS_1_2, SOCK_DTLS_SERVER);
    if (res < 0) {
        puts("Error creating DTLS sock");
        return NULL;
    }

    /* set PSK Identity hint, this is optional and application-specific */
#if IS_ACTIVE(CONFIG_DTLS_PSK)
    if (sock_dtls_set_server_psk_id_hint(&sock, PSK_DEFAULT_HINT) < 0) {
        puts("Error setting PSK Identity hint");
        return NULL;
    }
#endif

#if IS_ACTIVE(CONFIG_DTLS_ECC)
    /* register a second RPK */
    res = credman_add(&credential1);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("Error cannot add credential to system: %d\n", (int)res);
        return NULL;
    }

    /* make the new credential available to the sock */
    if (sock_dtls_add_credential(&sock, SOCK_DTLS_SERVER_TAG_1) < 0) {
        printf("Error cannot add credential to the sock: %d\n", (int)res);
        return NULL;
    }

    /* register a callback for RPK credential selection */
    sock_dtls_set_rpk_cb(&sock, _rpk_cb);
#endif

    while (active) {
        if ((msg_try_receive(&msg) == 1) &&
            (msg.type == DTLS_STOP_SERVER_MSG)){
            active = false;
        }
        else {
            sock_dtls_session_t session = { 0 };
            res = sock_dtls_recv(&sock, &session, rcv, sizeof(rcv),
                                  10 * US_PER_SEC);
            if (res >= 0) {
                printf("Received %d bytes -- (echo)\n", (int)res);
                res = sock_dtls_send(&sock, &session, rcv, (size_t)res, 0);
                if (res < 0) {
                    printf("Error resending DTLS message: %d", (int)res);
                }
                sock_dtls_session_destroy(&sock, &session);
            }
            else if (res == -SOCK_DTLS_HANDSHAKE) {
                printf("New client connected\n");
            }
        }
    }
    sock_dtls_close(&sock);
    sock_udp_close(&udp_sock);
    puts("Terminating");
    msg_reply(&msg, &msg);              /* Basic answer to the main thread */
    return NULL;
}

static void start_server(void)
{
    /* Only one instance of the server */
    if (_dtls_server_pid != KERNEL_PID_UNDEF) {
        puts("Error: server already running");
        return;
    }

    /* Start the server thread */
    _dtls_server_pid = thread_create(_dtls_server_stack,
                                     sizeof(_dtls_server_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST,
                                     dtls_server_wrapper, NULL, "dtls_server");

    /* Uncommon but better be sure */
    if (_dtls_server_pid < 0) {
        printf("ERROR: failed to create thread: %d\n", _dtls_server_pid);
        _dtls_server_pid = KERNEL_PID_UNDEF;
    }
}

static void stop_server(void)
{
    /* check if server is running at all */
    if (_dtls_server_pid == KERNEL_PID_UNDEF) {
        puts("Error: DTLS server is not running");
        return;
    }

    /* prepare the stop message */
    msg_t m;
    m.type = DTLS_STOP_SERVER_MSG;

    puts("Stopping server...");

    /* send the stop message to thread AND wait for (any) answer */
    msg_send_receive(&m, &m, _dtls_server_pid);

    _dtls_server_pid = KERNEL_PID_UNDEF;
    puts("Success: DTLS server stopped");
}

int dtls_server_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s start | stop\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "start") == 0) {
        start_server();
    }
    else if (strcmp(argv[1], "stop") == 0) {
        stop_server();
    }
    else {
        printf("Error: invalid command. Usage: %s start | stop\n", argv[0]);
        return 1;
    }
    return 0;
}
