/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       The cliend side of TinyDTLS (Simple echo)
 *
 * @author      Raul A. Fuentes Samaniego  <ra.fuentes.sam+RIOT@gmail.com>
 * @author      Olaf Bergmann <bergmann@tzi.org>
 * @author      Hauke Mehrtens <hauke@hauke-m.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/sock/udp.h"
#include "xtimer.h"
#include "timex.h"

#define ENABLE_DEBUG  (0)
#include "debug.h"

/* TinyDTLS */
#include "global.h"
#include "dtls_debug.h"
#include "dtls.h"

/* To avoid '(void) <variable>;' */
#ifdef __GNUC__
#define UNUSED_PARAM __attribute__((unused))
#else
#define UNUSED_PARAM
#endif /* __GNUC__ */

#ifdef DTLS_PSK

#define PSK_DEFAULT_IDENTITY "Client_identity"
#define PSK_DEFAULT_KEY      "secretPSK"
#define PSK_OPTIONS          "i:k:"

/* Max size for PSK lowered for embedded devices */
#define PSK_ID_MAXLEN 32
#define PSK_MAXLEN 32

#endif /* DTLS_PSK */

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220    /* DTLS default port  */
#endif

#define CLIENT_PORT  DTLS_DEFAULT_PORT + 1
#define MAX_TIMES_TRY_TO_SEND 10

/* TODO: Move this structure to pkg/TinyDTLS? */
typedef struct {
    sock_udp_t *sock;
    sock_udp_ep_t *local;
    sock_udp_ep_t *remote;
} dtls_remote_peer_t;




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


/**
 * @brief TinyDTLS callback for detecting the state of the DTLS channel.
 *
 * Exclusive of the TinyDTLS Client (For now).
 *
 * @note Once renegotiation is included this is also affected.
 */
static int client_events(struct dtls_context_t *ctx UNUSED_PARAM,
                         session_t *session UNUSED_PARAM,
                         dtls_alert_level_t level UNUSED_PARAM,
                         unsigned short code)
{
    /* dtls_connect has concluded in a DTLS session established? */
    if (code == DTLS_EVENT_CONNECTED) {
        dtls_connected = 1;
        DEBUG("CLIENT: DTLS Channel established!\n");
    }
#if ENABLE_DEBUG == 1
    /* At least a DTLS Client Hello was prepared? */
    else if (code == DTLS_EVENT_CONNECT) {
        DEBUG("CLIENT: DTLS Channel started\n");
    }
#endif
    /*TODO: DTLS_EVENT_RENEGOTIATE */

    return 0;
}


/**
 * @brief Handles the reception of the DTLS flights.
 *
 * Handles all the packets arriving at the node and identifies those that are
 * DTLS records. Also, it determines if said DTLS record is coming from a new
 * peer or a currently established peer.
 *
 * @note Deprecates dtls_handle_read()
 */
static void dtls_handle_read_sock(dtls_context_t *ctx, uint8_t *packet,
                                  size_t size)
{

    static session_t session;

    if (!ctx) {
        DEBUG("No DTLS context\n");
        return;
    }

    if (!dtls_get_app_data(ctx)) {
        DEBUG("No app_data stored!\n");
        return;
    }

    dtls_remote_peer_t *remote_peer;
    remote_peer =  (dtls_remote_peer_t *) dtls_get_app_data(ctx);

    /* session requires the remote socket address */
    session.size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    session.port = remote_peer->remote->port;

    if (memcpy(&session.addr, &remote_peer->remote->addr.ipv6, 16) == NULL) {
        puts("ERROR: memcpy failed!");
        return;
    }

#if ENABLE_DEBUG == 1
    DEBUG("DBG-Client: Msg received from \n\t Addr Src: [");
    ipv6_addr_print(&session.addr);
    DEBUG("]:%u\n", remote_peer->remote->port);
#endif

    dtls_handle_message(ctx, &session, packet, (int) size);

    return;
}


#ifdef DTLS_PSK
static unsigned char psk_id[PSK_ID_MAXLEN] = PSK_DEFAULT_IDENTITY;
static size_t psk_id_length = sizeof(PSK_DEFAULT_IDENTITY) - 1;
static unsigned char psk_key[PSK_MAXLEN] = PSK_DEFAULT_KEY;
static size_t psk_key_length = sizeof(PSK_DEFAULT_KEY) - 1;

/**
 * This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session.
 */
static int peer_get_psk_info(struct dtls_context_t *ctx UNUSED_PARAM,
                             const session_t *session UNUSED_PARAM,
                             dtls_credentials_type_t type,
                             const unsigned char *id, size_t id_len,
                             unsigned char *result, size_t result_length)
{

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
static int peer_get_ecdsa_key(struct dtls_context_t *ctx UNUSED_PARAM,
                              const session_t *session UNUSED_PARAM,
                              const dtls_ecdsa_key_t **result)
{
    static const dtls_ecdsa_key_t ecdsa_key = {
        .curve = DTLS_ECDH_CURVE_SECP256R1,
        .priv_key = ecdsa_priv_key,
        .pub_key_x = ecdsa_pub_key_x,
        .pub_key_y = ecdsa_pub_key_y
    };

    *result = &ecdsa_key;
    return 0;
}

static int peer_verify_ecdsa_key(struct dtls_context_t *ctx UNUSED_PARAM,
                                 const session_t *session UNUSED_PARAM,
                                 const unsigned char *other_pub_x UNUSED_PARAM,
                                 const unsigned char *other_pub_y UNUSED_PARAM,
                                 size_t key_size UNUSED_PARAM)
{
    /* As far for tinyDTLS 0.8.2 this is not used */
    return 0;
}
#endif /* DTLS_ECC */

/**
 * @brief Reception of a DTLS Applicaiton data record.
 */
static int read_from_peer(struct dtls_context_t *ctx UNUSED_PARAM,
                          session_t *session UNUSED_PARAM,
                          uint8 *data UNUSED_PARAM, size_t len UNUSED_PARAM)
{

    /*
     * This is where the data of the upper layer can be processed.
     * The example here only print the answer from the server.
     */

#if ENABLE_DEBUG == 1
    size_t i;
    DEBUG("Client: DTLS Data received -- ");
    for (i = 0; i < len; i++)
#ifndef COAP_MSG_SPOOF
        DEBUG("%c", data[i]);
#else
        DEBUG("%X  ", data[i]);
#endif
    DEBUG(" --\n");
#endif

    /*
     * NOTE: To answer the other peer use:
     * return dtls_write(ctx, session, data, len);
     */

    return 0;
}

/**
 * @brief  Transmits the next DTLS flight for a speicifc Peer.
 *
 * This is a simulation of an upper layer protocol. Such as CoAP.
 */
ssize_t  try_send(struct dtls_context_t *ctx, session_t *dst, uint8 *buf, size_t len)
{
    int res = 0;

    res = dtls_write(ctx, dst, buf, len);

    if (res >= 0) {
        memmove(buf, buf + res, len - res);
        len -= res;
        return len;
    }
    else if (res < 0) {
        dtls_crit("Client: dtls_write returned error!\n" );
        return 0;
    }

    return 0; /* No reachable */
}

/**
 * @brief Handles the DTLS communication with the other peer.
 */
static int send_to_peer(struct dtls_context_t *ctx UNUSED_PARAM,
                        session_t *session UNUSED_PARAM, uint8 *buf, size_t len)
{
    if (!dtls_get_app_data(ctx)) {
        return -1; /* At this point this should not happen anymore. */
    }

    dtls_remote_peer_t *remote_peer;
    remote_peer =  (dtls_remote_peer_t *) dtls_get_app_data(ctx);

    len = sock_udp_send(remote_peer->sock, buf, len, remote_peer->remote);
    if (!(len > 0)) {
        puts("Error: Unable to send message");
    }

    return len;
}

/**
 *  @brief DTLS variables and register are initialized.
 *
 *  Prepares the SIGNAL events and create a new DTLS context.
 */
dtls_context_t *init_dtls( dtls_remote_peer_t *remote_peer, session_t *dst,
                           char *addr_str)
{
    dtls_context_t *new_context = NULL;

    static dtls_handler_t cb = {
        .write = send_to_peer,
        .read = read_from_peer,
        .event = client_events,
     #ifdef DTLS_PSK
        .get_psk_info = peer_get_psk_info,
     #endif  /* DTLS_PSK */
     #ifdef DTLS_ECC
        .get_ecdsa_key = peer_get_ecdsa_key,
        .verify_ecdsa_key = peer_verify_ecdsa_key
     #endif  /* DTLS_ECC */
    };

   #ifdef DTLS_PSK
    DEBUG("Client support PSK\n");
   #endif
   #ifdef DTLS_ECC
    DEBUG("Client support ECC\n");
   #endif

    dtls_connected = 0;

    /* First, we prepare the UDP Sock  */
    /* TODO: Give control over DTLS_DEFAULT_PORT and CLIENT_PORT to the user. */
    remote_peer->local->port = (unsigned short)  CLIENT_PORT;
    remote_peer->remote->port = (unsigned short) DTLS_DEFAULT_PORT;

    if (ipv6_addr_from_str( (ipv6_addr_t *)remote_peer->remote->addr.ipv6, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return new_context;
    }

    /*
     *  Second: We prepare the DTLS Session by means of ctx->app
     *  This is to be used in the send_to_peer and (potentially) the
     *  read_from_peer, to continue the transmision with the next
     *  step of the DTLS flights.
     */
    dst->size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    dst->port = remote_peer->remote->port;

    /* NOTE: remote.addr.ipv6 and dst->addr are different structures. */
    if (ipv6_addr_from_str(&dst->addr, addr_str) == NULL) {
        puts("ERROR: init_dtls was unable to load the IPv6 addresses!");
        return new_context;
    }

    /*akin to syslog: EMERG, ALERT, CRITC, NOTICE, INFO, DEBUG, WARN */
    dtls_set_log_level(DTLS_LOG_INFO);

    new_context = dtls_new_context(remote_peer);
    if (new_context) {
        dtls_set_handler(new_context, &cb);
    }

    return new_context;
}

/**
 * This is the "client" part of this application.
 * Will be called each time a message is transmitted.
 */
static void client_send(char *addr_str, char *data UNUSED_PARAM,
                        unsigned int delay)
{
    static int8_t iWatch =  MAX_TIMES_TRY_TO_SEND;

    static session_t dst;
    dtls_context_t *dtls_context = NULL;
    dtls_remote_peer_t remote_peer;

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = { .family = AF_INET6 };
    sock_udp_t udp_socket;


    ssize_t pckt_rcvd_size = DTLS_MAX_BUF; /* The size of the packet received */
    uint8_t packet_rcvd[DTLS_MAX_BUF];

    ssize_t app_data_buf = 0; /* The size of the Upper layer packet to send */

    remote_peer.sock = &udp_socket;
    remote_peer.local = &local;
    remote_peer.remote = &remote;

    /*TinyDTLS settings (Universal and for the client) */
    dtls_init();
    dtls_context = init_dtls(&remote_peer, &dst, addr_str);
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
    app_data_buf =  strlen(client_payload);
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

    /* The sock must be opened */
    if (sock_udp_create(&udp_socket, &local, NULL, 0) != 0) {
        puts("Error: Unable to create UDP sock");
        return;
    }

    /*
     * The start the DTLS session for the server.
     * This send the first message from the client to the server.
     */
    if (dtls_connect(dtls_context, &dst) < 0) {
        puts("ERROR: Client unable to start a DTLS channel!\n");
        return;
    }

    /*
     * This loop has as objective to transmit all the DTLS records involved
     * in the DTLS session. Including the real (upper) data to send. There is a
     * watchdog involved if by any chance the server stop answering.
     */
    while ((app_data_buf > 0) && (iWatch > 0)) {

        /*  DTLS Session must be established before sending our data */
        if (dtls_connected == 1) {
            DEBUG("Sending (upper layer) data\n" );
            app_data_buf = try_send(dtls_context, &dst, (uint8 *) client_payload, app_data_buf);

            if (app_data_buf == 0) { /* This client only transmit data one time. */
                iWatch = 0;
            }
        }

        /* Sock is listening to any received answer */
        /* NOTE: DTLS Sessions can handles more than one single node */
        /* NOTE: Max lifetime expected for a DTLS retransmissions: 10 sec */
        pckt_rcvd_size = sock_udp_recv(&udp_socket, packet_rcvd, DTLS_MAX_BUF,
                                       10 * US_PER_SEC, &remote);

        switch (pckt_rcvd_size) {
            case -ENOBUFS:
                puts("ERROR: Buffer space not enough large!");
                break;

            case -EADDRNOTAVAIL:
                puts("\nERROR: Local Socket NULL");
                break;

            case -EAGAIN:
            case -ETIMEDOUT:
                DEBUG("\nTimeout for the server...");
                /* iWatch can be updated here */
                break;

            case -ENOMEM:
                puts("\nERROR: Memory overflow!");
                break;

            case 0:
            case -EPROTO:
            default:
                dtls_handle_read_sock(dtls_context, packet_rcvd, pckt_rcvd_size );
                break;

        }/*END-Switch*/

        iWatch--;
        xtimer_usleep(delay); /*Magic number*/
    } /*END while*/

    /* Release resources (strict order!) */
    dtls_free_context(dtls_context); /* This also send a DTLS Alert message */
    sock_udp_close(&udp_socket);
    dtls_connected = 0;
    DEBUG("Client DTLS session finished\n");

    return;
}

int udp_client_cmd(int argc, char **argv)
{
    uint32_t delay = 100;

#ifndef COAP_MSG_SPOOF
    if (argc < 3) {
        printf("usage: %s <addr> <data> [<delay in us>]\n", argv[0]);
        return 1;
    }
    else if (argc > 3) {
        delay = (uint32_t)atoi(argv[3]);
    }
    client_send(argv[1], argv[2],  delay);
#else
    if (argc < 2) {
        printf("usage: %s <addr> [<delay in us>]\n", argv[0]);
        return 1;
    }
    else if (argc > 2) {
        delay = (uint32_t)atoi(argv[2]);
    }
    client_send(argv[1], NULL,  delay);
#endif

    return 0;
}
