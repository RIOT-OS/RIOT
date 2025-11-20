/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Demonstrating the server side of TinyDTLS (Simple echo)
 *
 * @author      Raul A. Fuentes Samaniego <ra.fuentes.sam+RIOT@gmail.com>
 * @author      Olaf Bergmann <bergmann@tzi.org>
 * @author      Hauke Mehrtens <hauke@hauke-m.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

#include "net/sock/udp.h"
#include "msg.h"
#include "shell.h"
#include "timex.h"
#include "tinydtls_keys.h"

/* TinyDTLS */
#include "dtls.h"
#include "dtls_debug.h"
#include "tinydtls.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220 /* DTLS default port */
#endif

#define DTLS_STOP_SERVER_MSG 0x4001 /* Custom IPC type msg. */

/*
 * This structure will be used for storing the sock and the remote into the
 * dtls_context_t variable.
 *
 * This is because remote must not have port set to zero on sock_udp_create()
 * making impossible to recover the remote with sock_udp_get_remote()
 *
 * An alternative is to modify dtls_handle_message () to receive the remote
 * from sock_udp_recv(). Also, it's required to modify _send_to_peer_handler()  for
 * parsing an auxiliary sock_udp_ep_t variable from the dls session.
 */
typedef struct {
    sock_udp_t *sock;
    sock_udp_ep_t *remote;
} dtls_remote_peer_t;

static kernel_pid_t _dtls_server_pid = KERNEL_PID_UNDEF;

#define READER_QUEUE_SIZE (8U)

/*  NOTE: Temporary patch for tinyDTLS 0.8.6 */
#ifndef TINYDTLS_EXTRA_BUFF
#define TINYDTLS_EXTRA_BUFF (0U)
#endif

char _dtls_server_stack[THREAD_STACKSIZE_MAIN +
                        THREAD_EXTRA_STACKSIZE_PRINTF +
                        TINYDTLS_EXTRA_BUFF];

/*
 * Handles all the packets arriving at the node and identifies those that are
 * DTLS records. Also, it determines if said DTLS record is coming from a new
 * peer or a currently established peer.
 */
static int dtls_handle_read(dtls_context_t *ctx)
{
    static session_t session;
    static uint8_t packet_rcvd[DTLS_MAX_BUF];

    assert(ctx);
    assert(dtls_get_app_data(ctx));

    if (!ctx) {
        DEBUG("No DTLS context!\n");
        return 0;
    }

    if (!dtls_get_app_data(ctx)) {
        DEBUG("No app_data stored!\n");
        return 0;
    }

    dtls_remote_peer_t *remote_peer;
    remote_peer = (dtls_remote_peer_t *)dtls_get_app_data(ctx);

    ssize_t res = sock_udp_recv(remote_peer->sock, packet_rcvd,
                                sizeof(packet_rcvd), 1 * US_PER_SEC,
                                remote_peer->remote);

    if (res <= 0) {
        if ((ENABLE_DEBUG) && (res != -EAGAIN) && (res != -ETIMEDOUT)) {
            DEBUG("sock_udp_recv unexpected code error: %" PRIiSIZE "\n", res);
        }
        return 0;
    }

    DEBUG("DBG-Server: Record Rcvd\n");

    /* (DTLS) session requires the remote peer address (IPv6:Port) and netif */
    dtls_session_init(&session);
    session.addr.port = remote_peer->remote->port;
    session.addr.family = AF_INET6;
    if (remote_peer->remote->netif ==  SOCK_ADDR_ANY_NETIF) {
        session.ifindex = SOCK_ADDR_ANY_NETIF;
    }
    else {
        session.ifindex = remote_peer->remote->netif;
    }

    memcpy(&session.addr.ipv6, &remote_peer->remote->addr.ipv6, sizeof(session.addr.ipv6));
    return dtls_handle_message(ctx, &session, packet_rcvd, res);
}

/* Reception of a DTLS Application data record. */
static int _read_from_peer_handler(struct dtls_context_t *ctx,
                                   session_t *session, uint8 *data, size_t len)
{
    size_t i;

    printf("\nServer: got DTLS Data App: --- ");
    for (i = 0; i < len; i++) {
        printf("%c", data[i]);
    }
    puts(" ---\t(echo!)");

    /* echo back the application data rcvd. */
    return dtls_write(ctx, session, data, len);
}

/* Handles the DTLS communication with the other peer. */
static int _send_to_peer_handler(struct dtls_context_t *ctx,
                                 session_t *session, uint8 *buf, size_t len)
{

    /*
     * It's possible to create a sock_udp_ep_t variable. But, it's required
     * to copy memory from the session variable to it.
     */
    (void) session;

    assert(ctx);
    assert(dtls_get_app_data(ctx));

    if (!dtls_get_app_data(ctx)) {
        return -1;
    }

    dtls_remote_peer_t *remote_peer;
    remote_peer = (dtls_remote_peer_t *)dtls_get_app_data(ctx);

    DEBUG("DBG-Server: Sending record\n");

    return sock_udp_send(remote_peer->sock, buf, len, remote_peer->remote);
}

#ifdef CONFIG_DTLS_PSK
static unsigned char psk_id[PSK_ID_MAXLEN] = PSK_DEFAULT_IDENTITY;
static size_t psk_id_length = sizeof(PSK_DEFAULT_IDENTITY) - 1;
static unsigned char psk_key[PSK_MAXLEN] = PSK_DEFAULT_KEY;
static size_t psk_key_length = sizeof(PSK_DEFAULT_KEY) - 1;

/*
 * This function is the "key store" for tinyDTLS. It is called to retrieve a
 * key for the given identity within this particular session.
 */
static int _peer_get_psk_info_handler(struct dtls_context_t *ctx, const session_t *session,
                                      dtls_credentials_type_t type,
                                      const unsigned char *id, size_t id_len,
                                      unsigned char *result, size_t result_length)
{
    (void) ctx;
    (void) session;

    struct keymap_t {
        unsigned char *id;
        size_t id_length;
        unsigned char *key;
        size_t key_length;
    } psk[3] = {
        { (unsigned char *)psk_id, psk_id_length,
          (unsigned char *)psk_key, psk_key_length },
        { (unsigned char *)"default identity", 16,
          (unsigned char *)"\x11\x22\x33", 3 },
        { (unsigned char *)"\0", 2,
          (unsigned char *)"", 1 }
    };

    if (type != DTLS_PSK_KEY) {
        return 0;
    }

    if (id) {
        uint8_t i;
        for (i = 0; i < ARRAY_SIZE(psk); i++) {
            if (id_len == psk[i].id_length && memcmp(id, psk[i].id, id_len) == 0) {
                if (result_length < psk[i].key_length) {
                    dtls_warn("buffer too small for PSK");
                    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
                }

                memcpy(result, psk[i].key, psk[i].key_length);
                return psk[i].key_length;
            }
        }
    }

    return dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR);
}
#endif /* CONFIG_DTLS_PSK */

#ifdef CONFIG_DTLS_ECC
static int _peer_get_ecdsa_key_handler(struct dtls_context_t *ctx,
                                       const session_t *session,
                                       const dtls_ecdsa_key_t **result)
{
    (void) ctx;
    (void) session;
    static const dtls_ecdsa_key_t ecdsa_key = {
        .curve = DTLS_ECDH_CURVE_SECP256R1,
        .priv_key = ecdsa_priv_key,
        .pub_key_x = ecdsa_pub_key_x,
        .pub_key_y = ecdsa_pub_key_y
    };

    /* TODO: Load the key from external source */

    *result = &ecdsa_key;
    return 0;
}

static int _peer_verify_ecdsa_key_handler(struct dtls_context_t *ctx,
                                          const session_t *session,
                                          const unsigned char *other_pub_x,
                                          const unsigned char *other_pub_y,
                                          size_t key_size)
{
    (void) ctx;
    (void) session;
    (void) other_pub_x;
    (void) other_pub_y;
    (void) key_size;

    /* TODO: As far for tinyDTLS 0.8.2 this is not used */

    return 0;
}
#endif /* CONFIG_DTLS_ECC */

/* DTLS variables and register are initialized. */
dtls_context_t *_server_init_dtls(dtls_remote_peer_t *remote_peer)
{
    dtls_context_t *new_context;

    static dtls_handler_t cb = {
        .write = _send_to_peer_handler,
        .read = _read_from_peer_handler,
        .event = NULL,
#ifdef CONFIG_DTLS_PSK
        .get_psk_info = _peer_get_psk_info_handler,
#endif  /* CONFIG_DTLS_PSK */
#ifdef CONFIG_DTLS_ECC
        .get_ecdsa_key = _peer_get_ecdsa_key_handler,
        .verify_ecdsa_key = _peer_verify_ecdsa_key_handler
#endif  /* CONFIG_DTLS_ECC */
    };

#ifdef CONFIG_DTLS_PSK
    DEBUG("Server support PSK\n");
#endif
#ifdef CONFIG_DTLS_ECC
    DEBUG("Server support ECC\n");
#endif

#ifdef TINYDTLS_LOG_LVL
    dtls_set_log_level(TINYDTLS_LOG_LVL);
#endif

    /*
     * The context for the server is different from the client.
     * This is because sock_udp_create() cannot work with a remote endpoint
     * with port set to 0. And even after sock_udp_recv(), sock_udp_get_remote()
     * cannot retrieve the remote.
     */
    new_context = dtls_new_context(remote_peer);

    if (new_context) {
        dtls_set_handler(new_context, &cb);
    }
    else {
        return NULL;
    }

    return new_context;
}

void *_dtls_server_wrapper(void *arg)
{
    (void) arg;

    bool active = true;
    msg_t _reader_queue[READER_QUEUE_SIZE];
    msg_t msg;

    sock_udp_t udp_socket;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    dtls_context_t *dtls_context = NULL;
    dtls_remote_peer_t remote_peer;

    remote_peer.sock = &udp_socket;
    remote_peer.remote = &remote;

    /* Prepare (thread) messages reception */
    msg_init_queue(_reader_queue, READER_QUEUE_SIZE);

    /* NOTE: dtls_init() must be called previous to this (see main.c) */

    local.port = DTLS_DEFAULT_PORT;
    ssize_t res = sock_udp_create(&udp_socket, &local, NULL, 0);
    if (res == -1) {
        puts("ERROR: Unable create sock.");
        return (void *) NULL;
    }

    dtls_context = _server_init_dtls(&remote_peer);

    if (!dtls_context) {
        puts("ERROR: Server unable to load context!");
        return (void *) NULL;
    }

    while (active) {
        if ((msg_try_receive(&msg) == -1) && (msg.type == DTLS_STOP_SERVER_MSG)) {
            active = false;
        }
        else {
            /* Listening for any DTLS recodrd */
            if (dtls_handle_read(dtls_context) < 0) {
                printf("Received alert from client\n");
            }
        }
    }

    /* Release resources (strict order) */
    dtls_free_context(dtls_context);    /* This also sends a DTLS Alert record */
    sock_udp_close(&udp_socket);
    msg_reply(&msg, &msg);              /* Basic answer to the main thread */

    return (void *) NULL;
}

static void start_server(void)
{
    /* Only one instance of the server */
    if (_dtls_server_pid != KERNEL_PID_UNDEF) {
        puts("Error: server already running");
        return;
    }

    /* The server is initialized */
    _dtls_server_pid = thread_create(_dtls_server_stack,
                                     sizeof(_dtls_server_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     0,
                                     _dtls_server_wrapper, NULL, "DTLS_Server");

    /* Uncommon but better be sure */
    if (_dtls_server_pid == EINVAL) {
        puts("ERROR: Thread invalid");
        _dtls_server_pid = KERNEL_PID_UNDEF;
        return;
    }

    if (_dtls_server_pid == EOVERFLOW) {
        puts("ERROR: Thread overflow!");
        _dtls_server_pid = KERNEL_PID_UNDEF;
        return;
    }

    return;
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

    DEBUG("Stopping server...\n");

    /* send the stop message to thread AND wait for (any) answer */
    msg_send_receive(&m, &m, _dtls_server_pid);

    _dtls_server_pid = KERNEL_PID_UNDEF;
    puts("Success: DTLS server stopped");
}

static int _server_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s start|stop\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "start") == 0) {
        start_server();
    }
    else if (strcmp(argv[1], "stop") == 0) {
        stop_server();
    }
    else {
        printf("Error: invalid command. Usage: %s start|stop\n", argv[0]);
    }
    return 0;
}

SHELL_COMMAND(dtlss, "Start and stop a DTLS server", _server_cmd);
