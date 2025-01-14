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
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
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

#include "dtls_server_credentials.h"

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT (20220) /* DTLS default port */
#endif

static bool _server_credentials_configured = false;

/* Credman tags to select which credentials to use */
#define SOCK_DTLS_SERVER_TAG_0 (10)
#define SOCK_DTLS_SERVER_TAG_1 (11)

#define DTLS_STOP_SERVER_MSG 0x4001 /* Custom IPC type msg. */
#define READER_QUEUE_SIZE (8U)

char _dtls_server_stack[THREAD_STACKSIZE_MAIN + THREAD_EXTRA_STACKSIZE_PRINTF];

static kernel_pid_t _dtls_server_pid = KERNEL_PID_UNDEF;

static unsigned int _ecc_credential = 0;

/*
 * Each credman_credential_t acts as a sort of keyring, containing a single
 * private / public key pair and a list of public keys of clients that are
 * known.
*/
static const ecdsa_public_key_t known_client_pub_keys[] = {
    { .x = known_client_public_key_0_x, .y = known_client_public_key_0_y }
};

static const credman_credential_t ecc_credential_0 = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_SERVER_TAG_0,
    .params = {
        .ecdsa = {
            .private_key = server_private_key_0,
            .public_key = {
                .x = server_public_key_0_x,
                .y = server_public_key_0_y,
            },
            .client_keys = (ecdsa_public_key_t *)known_client_pub_keys,
            .client_keys_size = ARRAY_SIZE(known_client_pub_keys),
        },
    },
};

static const credman_credential_t ecc_credential_1 = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_SERVER_TAG_1,
    .params = {
        .ecdsa = {
            .private_key = server_private_key_1,
            .public_key = {
                .x = server_public_key_1_x,
                .y = server_public_key_1_y,
            },
            .client_keys = (ecdsa_public_key_t *)known_client_pub_keys,
            .client_keys_size = ARRAY_SIZE(known_client_pub_keys),
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

    if (!_ecc_credential) {
        return SOCK_DTLS_SERVER_TAG_0;
    }
    else {
        return SOCK_DTLS_SERVER_TAG_1;
    }
}

/*
 * We have a single PSK.
*/
static const uint8_t psk_id[] = SERVER_PSK_IDENTITY;
static const uint8_t psk_key[] = SERVER_PSK_IDENTITY_KEY;

static const credman_credential_t psk_credential_0 = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_SERVER_TAG_0,
    .params = {
        .psk = {
            .key = { .s = psk_key, .len = sizeof(psk_key) - 1, },
            .id = { .s = psk_id, .len = sizeof(psk_id) - 1, },
        },
    },
};

static int _configure_server_credentials(void)
{
    /* register the credentials on credman */
    if (IS_ACTIVE(CONFIG_DTLS_ECC)) {
        if (credman_add(&ecc_credential_0) != CREDMAN_OK) {
            puts("Error cannot add ECC credential 0 to system");
            return -1;
        }

        if (credman_add(&ecc_credential_1) != CREDMAN_OK) {
            puts("Error cannot add ECC credential 1 to system");
            return -1;
        }
    }
    else if (IS_ACTIVE(CONFIG_DTLS_PSK)) {
        if (credman_add(&psk_credential_0) != CREDMAN_OK) {
            puts("Error cannot add PSK credential 0 to system");
            return -1;
        }
    }
    return 0;
}

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

    /*
     * Currently DTLS sock needs one and only one credential for the
     * initialization. Subsequent credentials are made available to the sock
     * by means of `sock_dtls_add_credential`.
     */
    res = sock_dtls_create(&sock, &udp_sock, SOCK_DTLS_SERVER_TAG_0,
                           SOCK_DTLS_1_2, SOCK_DTLS_SERVER);
    if (res < 0) {
        puts("Error creating DTLS sock");
        return NULL;
    }

    if (IS_ACTIVE(CONFIG_DTLS_ECC)) {
        /* make the second ECC credential available to the sock */
        if (sock_dtls_add_credential(&sock, SOCK_DTLS_SERVER_TAG_1) < 0) {
            puts("Error cannot add second ECC credential to the sock");
            return NULL;
        }

        /* register a callback for RPK credential selection */
        sock_dtls_set_rpk_cb(&sock, _rpk_cb);
    }
    else if (IS_ACTIVE(CONFIG_DTLS_PSK)) {
        /* set PSK Identity hint, this is optional and application-specific */
        if (sock_dtls_set_server_psk_id_hint(&sock, SERVER_PSK_IDENTITY_HINT) < 0) {
            puts("Error setting PSK Identity hint");
            return NULL;
        }
    }

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
                printf("Received %" PRIdSIZE " bytes -- (echo)\n", res);
                res = sock_dtls_send(&sock, &session, rcv, (size_t)res, 0);
                if (res < 0) {
                    printf("Error resending DTLS message: %" PRIdSIZE, res);
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
                                     0,
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

void _print_usage(const char *cmd)
{
    if (IS_ACTIVE(CONFIG_DTLS_ECC)) {
        printf("usage: %s start | stop | ecc <0|1>\n", cmd);
    }
    else {
        printf("usage: %s start | stop\n", cmd);
    }
}

int dtls_server_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "start") == 0) {
        if (!_server_credentials_configured) {
            int res = _configure_server_credentials();
            if (res < 0) {
                return res;
            }
            _server_credentials_configured = true;
        }
        start_server();
    }
    else if (strcmp(argv[1], "stop") == 0) {
        stop_server();
    }
    else if (IS_ACTIVE(CONFIG_DTLS_ECC) && strcmp(argv[1], "ecc") == 0) {
        /* if using ECC, allow choosing which key to use on handshakes at runtime */
        if (argc < 3) {
            _print_usage(argv[0]);
            return 1;
        }

        int value = atoi(argv[2]);
        if (value != 0 && value != 1) {
            printf("Error: invalid value, should be 0 or 1, got %i\n", value);
            return 1;
        }
        else {
            _ecc_credential = value;
        }
    }
    else {
        printf("Error: invalid command.");
        _print_usage(argv[0]);
        return 1;
    }
    return 0;
}
