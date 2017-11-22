/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2018 Inria
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
 * @brief       Demonstrating the client side of TinyDTLS (Simple echo)
 *
 * @author      Raul A. Fuentes Samaniego <raul.fuentes-samaniego@inria.fr>
 * @author      Olaf Bergmann <bergmann@tzi.org>
 * @author      Hauke Mehrtens <hauke@hauke-m.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/sock/udp.h"

/* TinyDTLS */
#include "dtls_debug.h"
#include "dtls.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef DTLS_PSK

#define PSK_DEFAULT_IDENTITY "Client_identity"
#define PSK_DEFAULT_KEY "secretPSK"
#define PSK_OPTIONS "i:k:"

/* Max size for PSK lowered for embedded devices */
#define PSK_ID_MAXLEN 32
#define PSK_MAXLEN 32

#endif /* DTLS_PSK */

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220 /* DTLS default port */
#endif

#define CLIENT_PORT DTLS_DEFAULT_PORT + 1
#define MAX_TIMES_TRY_TO_SEND 10
#define DEFAULT_DELAY 100

static int dtls_connected = 0; /* This is handled by Tinydtls' callbacks */

#ifdef DTLS_ECC
static const unsigned char ecdsa_priv_key[] = {
    0x41, 0xC1, 0xCB, 0x6B, 0x51, 0x24, 0x7A, 0x14,
    0x43, 0x21, 0x43, 0x5B, 0x7A, 0x80, 0xE7, 0x14,
    0x89, 0x6A, 0x33, 0xBB, 0xAD, 0x72, 0x94, 0xCA,
    0x40, 0x14, 0x55, 0xA1, 0x94, 0xA9, 0x49, 0xFA
};

static const unsigned char ecdsa_pub_key_x[] = {
    0x36, 0xDF, 0xE2, 0xC6, 0xF9, 0xF2, 0xED, 0x29,
    0xDA, 0x0A, 0x9A, 0x8F, 0x62, 0x68, 0x4E, 0x91,
    0x63, 0x75, 0xBA, 0x10, 0x30, 0x0C, 0x28, 0xC5,
    0xE4, 0x7C, 0xFB, 0xF2, 0x5F, 0xA5, 0x8F, 0x52
};

static const unsigned char ecdsa_pub_key_y[] = {
    0x71, 0xA0, 0xD4, 0xFC, 0xDE, 0x1A, 0xB8, 0x78,
    0x5A, 0x3C, 0x78, 0x69, 0x35, 0xA7, 0xCF, 0xAB,
    0xE9, 0x3F, 0x98, 0x72, 0x09, 0xDA, 0xED, 0x0B,
    0x4F, 0xAB, 0xC3, 0x6F, 0xC7, 0x72, 0xF8, 0x29
};
#endif

/* TinyDTLS callback for detecting the state of the DTLS channel. */
static int _events_handler(struct dtls_context_t *ctx,
                           session_t *session,
                           dtls_alert_level_t level,
                           unsigned short code)
{
    (void) ctx;
    (void) session;
    (void) level;

    if (code == DTLS_EVENT_CONNECTED) {
        dtls_connected = 1;
        DEBUG("CLIENT: DTLS Channel established!\n");
    }
    /* At least a DTLS Client Hello was prepared? */
    else if ((ENABLE_DEBUG) && (code == DTLS_EVENT_CONNECT)) {
        DEBUG("CLIENT: DTLS Channel started\n");
    }

    /*NOTE: DTLS_EVENT_RENEGOTIATE can be handled here */

    return 0;
}

/*
 * Handles all the packets arriving at the node and identifies those that are
 * DTLS records. Also, it determines if said DTLS record is coming from a new
 * peer or a currently established peer.
 *
 */
static void dtls_handle_read(dtls_context_t *ctx, uint8_t *packet,
                             size_t size)
{
    static session_t session;
    static sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    if (!ctx) {
        DEBUG("%s: No DTLS context\n", __func__);
        return;
    }

    if (!dtls_get_app_data(ctx)) {
        DEBUG("%s: No app_data stored!\n", __func__);
        return;
    }

    sock_udp_t *sock;
    sock =  (sock_udp_t *)dtls_get_app_data(ctx);


    if (sock_udp_get_remote(sock, &remote) == -ENOTCONN) {
        DEBUG("%s: Unable to retrieve remote!\n", __func__);
        return;
    }

    /* session requires the remote socket (IPv6:UDP) address and netif  */
    session.size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    session.port = remote.port;
    if (&remote.netif ==  SOCK_ADDR_ANY_NETIF) {
        session.ifindex  = SOCK_ADDR_ANY_NETIF;
    }
    else {
        session.ifindex  = remote.netif;
    }

    if (memcpy(&session.addr, &remote.addr.ipv6, 16) == NULL) {
        puts("ERROR: memcpy failed!");
        return;
    }

    if (ENABLE_DEBUG) {
        DEBUG("DBG-Client: Msg received from \n\t Addr Src: [");
        ipv6_addr_print(&session.addr);
        DEBUG("]:%u\n", remote.port);
    }

    dtls_handle_message(ctx, &session, packet, (int)size);

    return;
}

#ifdef DTLS_PSK
static unsigned char psk_id[PSK_ID_MAXLEN] = PSK_DEFAULT_IDENTITY;
static size_t psk_id_length = sizeof(PSK_DEFAULT_IDENTITY) - 1;
static unsigned char psk_key[PSK_MAXLEN] = PSK_DEFAULT_KEY;
static size_t psk_key_length = sizeof(PSK_DEFAULT_KEY) - 1;

/*
 * This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session.
 */
static int _peer_get_psk_info_handler(struct dtls_context_t *ctx,
                                      const session_t *session,
                                      dtls_credentials_type_t type,
                                      const unsigned char *id, size_t id_len,
                                      unsigned char *result, size_t result_length)
{
    (void) ctx;
    (void) session;

    switch (type) {
        case DTLS_PSK_IDENTITY:
            if (id_len) {
                dtls_debug("got psk_identity_hint: '%.*s'\n", id_len, id);
            }

            if (result_length < psk_id_length) {
                dtls_warn("cannot set psk_identity -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, psk_id, psk_id_length);
            return psk_id_length;
        case DTLS_PSK_KEY:
            if (id_len != psk_id_length || memcmp(psk_id, id, id_len) != 0) {
                dtls_warn("PSK for unknown id requested, exiting\n");
                return dtls_alert_fatal_create(DTLS_ALERT_ILLEGAL_PARAMETER);
            }
            else if (result_length < psk_key_length) {
                dtls_warn("cannot set psk -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, psk_key, psk_key_length);
            return psk_key_length;
        default:
            dtls_warn("unsupported request type: %d\n", type);
    }

    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static int _peer_get_ecdsa_key_handler(struct dtls_context_t *ctx,
                                       const session_t *session,
                                       const dtls_ecdsa_key_t **result)
{
    (void) ctx;
    (void) session;

    /*TODO: Load the key from external source */

    static const dtls_ecdsa_key_t ecdsa_key = {
        .curve = DTLS_ECDH_CURVE_SECP256R1,
        .priv_key = ecdsa_priv_key,
        .pub_key_x = ecdsa_pub_key_x,
        .pub_key_y = ecdsa_pub_key_y
    };

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
    (void) other_pub_y;
    (void) other_pub_x;
    (void) key_size;

    /* TODO: As far for tinyDTLS 0.8.2 this is not used */

    return 0;
}
#endif /* DTLS_ECC */

/* Reception of a DTLS Applicaiton data record. */
static int _read_from_peer_handler(struct dtls_context_t *ctx,
                                   session_t *session,
                                   uint8 *data, size_t len)
{
    (void) ctx;
    (void) session;

    printf("Client: got DTLS Data App -- ");
    for (size_t i = 0; i < len; i++)
#ifndef COAP_MSG_SPOOF
        printf("%c", data[i]);
#else
        printf("%X  ", data[i]);
#endif
    puts(" --");

    /*
     * NOTE: To answer the other peer uses dtls_write(). E.g.
     * return dtls_write(ctx, session, data, len);
     */

    return 0;
}

/* Transmits the next DTLS flight for a speicifc Peer. */
ssize_t try_send(struct dtls_context_t *ctx, session_t *dst, uint8 *buf, size_t len)
{
    int res = 0;

    res = dtls_write(ctx, dst, buf, len);

    if (res >= 0) {
        memmove(buf, buf + res, len - res);
        len -= res;
        return len;
    }
    else if (res < 0) {
        dtls_crit("Client: dtls_write returned error!\n");
        return -1;
    }

    return 0;
}

/* Handles the DTLS communication with the other peer. */
static int _send_to_peer_handler(struct dtls_context_t *ctx,
                                 session_t *session, uint8 *buf, size_t len)
{
    (void) session;

    assert(ctx);
    assert(dtls_get_app_data(ctx));

    if (!dtls_get_app_data(ctx)) {
        return -1; /* At this point this should not happen anymore. */
    }

    sock_udp_t *sock;
    sock = (sock_udp_t *)dtls_get_app_data(ctx);

    ssize_t res = sock_udp_send(sock, buf, len, NULL);
    if (res <= 0) {
        puts("ERROR: Unable to send DTLS record");
    }

    return res;
}

/* DTLS variables are initialized. */
dtls_context_t *_init_dtls(sock_udp_t *sock, sock_udp_ep_t *local,
                           sock_udp_ep_t *remote, session_t *dst,
                           char *addr_str)
{
    dtls_context_t *new_context = NULL;

    static dtls_handler_t cb = {
        .write = _send_to_peer_handler,
        .read = _read_from_peer_handler,
        .event = _events_handler,
#ifdef DTLS_PSK
        .get_psk_info = _peer_get_psk_info_handler,
#endif  /* DTLS_PSK */
#ifdef DTLS_ECC
        .get_ecdsa_key = _peer_get_ecdsa_key_handler,
        .verify_ecdsa_key = _peer_verify_ecdsa_key_handler
#endif  /* DTLS_ECC */
    };

#ifdef DTLS_PSK
    DEBUG("Client support PSK\n");
#endif
#ifdef DTLS_ECC
    DEBUG("Client support ECC\n");
#endif

    dtls_connected = 0;

#ifdef TINYDTLS_LOG_LVL
    dtls_set_log_level(TINYDTLS_LOG_LVL);
#endif

    /* First, we prepare the UDP Sock */
    local->port = (unsigned short) CLIENT_PORT;
    remote->port = (unsigned short) DTLS_DEFAULT_PORT;

    /* Parsing <address>[:<iface>]:Port */
    int iface = ipv6_addr_split_iface(addr_str);
    if (iface == -1) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            dst->ifindex = (uint16_t)gnrc_netif_iter(NULL)->pid;
            remote->netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            /* FIXME This probably is not valid with multiple interfaces */
            dst->ifindex = remote->netif;
        }
    }
    else {
        if (gnrc_netif_get_by_pid(iface) == NULL) {
            puts("ERROR: interface not valid");
            return new_context;
        }
        dst->ifindex = (uint16_t)gnrc_netif_iter(NULL)->pid;
        remote->netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
    }

    if (ipv6_addr_from_str((ipv6_addr_t *)remote->addr.ipv6, addr_str) == NULL) {
        puts("ERROR: unable to parse destination address");
        return new_context;
    }

    /* Second: We prepare the DTLS Session by means of ctx->app */
    dst->size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    dst->port = remote->port;

    /* NOTE: remote.addr.ipv6 and dst->addr are different structures. */
    if (ipv6_addr_from_str(&dst->addr, addr_str) == NULL) {
        puts("ERROR: init_dtls was unable to load the IPv6 addresses!");
        return new_context;
    }

    new_context = dtls_new_context(sock);
    if (new_context) {
        dtls_set_handler(new_context, &cb);
    }

    return new_context;
}

static void client_send(char *addr_str, char *data,
                        unsigned int delay)
{
    static session_t dst;
    dtls_context_t *dtls_context = NULL;

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;

    uint8_t packet_rcvd[DTLS_MAX_BUF];
    int8_t watch = MAX_TIMES_TRY_TO_SEND;

    ssize_t pckt_rcvd_size = DTLS_MAX_BUF;  /* DTLS flights records */
    ssize_t app_data_buf = 0;               /* Upper layer packet to send */


    /* NOTE: dtls_init() must be called previous to this (see main.c) */

    dtls_context = _init_dtls(&sock, &local, &remote, &dst, addr_str);
    if (!dtls_context) {
        puts("ERROR: Client unable to load context!");
        return;
    }

#ifndef COAP_MSG_SPOOF
    char *client_payload;
    if (strlen(data) > DTLS_MAX_BUF) {
        puts("ERROR: Exceeded max size of DTLS buffer.");
        return;
    }
    client_payload = data;
    app_data_buf = strlen(client_payload);
#else /* COAP_MSG_SPOOF */
    (void) data;
    /* CoAP Message: NON, MID:59394, GET, TKN:0d 9f, /riot/value */
    unsigned char client_payload[] = {
        0x52, 0x01, 0xe8, 0x02, 0x0d, 0x9f, 0xb4, 0x72,
        0x69, 0x6f, 0x74, 0x05, 0x76, 0x61, 0x6c, 0x75,
        0x65
    };
    app_data_buf = sizeof(client_payload);
#endif /* COAP_MSG_SPOOF */

    /* The sock must be opened with the remote already linked to it */
    if (sock_udp_create(&sock, &local, &remote, 0) != 0) {
        puts("ERROR: Unable to create UDP sock");
        return;
    }

    /*
     * Starts the DTLS handshake process by sending the first DTLS Hello Client
     * record.
     *
     * NOTE: If dtls_connect() returns zero, then the DTLS channel for the
     *      dtls_context is already created (never the case for this example)
     */
    if (dtls_connect(dtls_context, &dst) < 0) {
        puts("ERROR: Client unable to start a DTLS channel!\n");
        return;
    }

    /*
     * This loop transmits all the DTLS records involved in the DTLS session.
     * Including the real (upper) data to send and to receive. There is a
     * watchdog if the remote peer stop answering.
     *
     * Max lifetime expected for a DTLS handshake is 10 sec. This is reflected
     * with the variable watch and the timeout for sock_udp_recv().
     *
     * NOTE: DTLS Sessions can handles more than one single node but by
     *       default is limited to a single peer with a single context and
     *       a single concurrent handshake.
     *       See tinydtls/platform-specific/riot_boards.h for more info.
     * NOTE: DTLS_DEFAULT_MAX_RETRANSMIT has an impact here.
     */
    while ((app_data_buf > 0) && (watch > 0)) {

        /*  DTLS Session must be established before sending our data */
        if (dtls_connected) {
            DEBUG("Sending (upper layer) data\n");
            app_data_buf = try_send(dtls_context, &dst,
                                    (uint8 *)client_payload, app_data_buf);

            if (app_data_buf == 0) { /* Client only transmit data one time. */
                watch = 0;
            }
        }

        pckt_rcvd_size = sock_udp_recv(&sock, packet_rcvd, DTLS_MAX_BUF,
                                       1 * US_PER_SEC, &remote);

        if (pckt_rcvd_size >= 0) {
            dtls_handle_read(dtls_context, packet_rcvd, pckt_rcvd_size);
        }
#if ENABLE_DEBUG
        else {
            switch (pckt_rcvd_size) {
                case -ENOBUFS:
                    puts("ERROR: Buffer space not enough large!");
                    break;

                case -EADDRNOTAVAIL:
                    puts("ERROR: Local Socket NULL");
                    break;

                /* NOTE: Actually, those are OK for this test */
                case -EAGAIN:
                case -ETIMEDOUT:
                    /* watchdog can be updated here */
                    break;

                case -ENOMEM:
                    puts("ERROR: Memory overflow!");
                    break;

                case -EINVAL:
                    puts("ERROR: remote or sock is not properly initialized");
                    break;

                case -EPROTO:
                    puts("ERROR: source address of received packet did not equal the remote");
                    break;

                default:
                    printf("ERROR: unexpected code error: %zd \n", pckt_rcvd_size);
                    break;
            } /* END-Switch */
        } /*END-Else */
#endif

        watch--;
        /*
         * This delay is to give time to the remote peer to do the compute.
         * This become strongly important when ECC is used in another sensor.
         */
        xtimer_usleep(delay);


    } /*END while*/

    /*
     * BUG: tinyDTLS (<= 0.8.6)
     * If dtls_connect() is called but the handshake does not complete (e.g.
     * peer is offline) then a netq_t object is allocated and never freed
     * leaving a memory leak of 124 bytes.
     * This can lead to "retransmit buffer full" error.
     *
     * A temporary solution is to make the dtls_context_t global and be sure
     * to never release it. Alternatively, never let this part of the code
     * ends, in a similar approach to the server side.
     */

    /* Release resources (strict order!) */
    dtls_free_context(dtls_context); /* This also sends a DTLS Alert record */
    sock_udp_close(&sock);
    dtls_connected = 0;
    DEBUG("Client DTLS session finished\n");

    return;
}

int udp_client_cmd(int argc, char **argv)
{
    uint32_t delay = DEFAULT_DELAY;

#ifndef COAP_MSG_SPOOF
    if (argc < 3) {
        printf("usage: %s <addr> <data> [<delay in us>]\n", argv[0]);
        return 1;
    }
    else if (argc > 3) {
        delay = atoi(argv[3]);
    }
    client_send(argv[1], argv[2], delay);
#else
    if (argc < 2) {
        printf("usage: %s <addr> [<delay in us>]\n", argv[0]);
        return 1;
    }
    else if (argc > 2) {
        delay = atoi(argv[2]);
    }
    client_send(argv[1], NULL, delay);
#endif

    return 0;
}
