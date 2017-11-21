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
 * @brief       The server side of TinyDTLS (Simple echo)
 *
 * @author      Raul A. Fuentes Samaniego  <ra.fuentes.sam+RIOT@gmail.com>
 * @author      Olaf Bergmann <bergmann@tzi.org>
 * @author      Hauke Mehrtens <hauke@hauke-m.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "timex.h"
#include "utlist.h"
#include "xtimer.h"
#include "msg.h"


/* TinyDTLS */
#include "dtls.h"
#include "dtls_debug.h"
#include "tinydtls.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

//#define DEFAULT_PORT 20220    /* DTLS default port  */
#define DEFAULT_PORT 61618      /* First valid FEBx address  */

/* TODO: MAke this local! */
static dtls_context_t *dtls_context = NULL;

static const unsigned char ecdsa_priv_key[] = {
    0xD9, 0xE2, 0x70, 0x7A, 0x72, 0xDA, 0x6A, 0x05,
    0x04, 0x99, 0x5C, 0x86, 0xED, 0xDB, 0xE3, 0xEF,
    0xC7, 0xF1, 0xCD, 0x74, 0x83, 0x8F, 0x75, 0x70,
    0xC8, 0x07, 0x2D, 0x0A, 0x76, 0x26, 0x1B, 0xD4
};

static const unsigned char ecdsa_pub_key_x[] = {
    0xD0, 0x55, 0xEE, 0x14, 0x08, 0x4D, 0x6E, 0x06,
    0x15, 0x59, 0x9D, 0xB5, 0x83, 0x91, 0x3E, 0x4A,
    0x3E, 0x45, 0x26, 0xA2, 0x70, 0x4D, 0x61, 0xF2,
    0x7A, 0x4C, 0xCF, 0xBA, 0x97, 0x58, 0xEF, 0x9A
};

static const unsigned char ecdsa_pub_key_y[] = {
    0xB4, 0x18, 0xB6, 0x4A, 0xFE, 0x80, 0x30, 0xDA,
    0x1D, 0xDC, 0xF4, 0xF4, 0x2E, 0x2F, 0x26, 0x31,
    0xD0, 0x43, 0xB1, 0xFB, 0x03, 0xE2, 0x2F, 0x4D,
    0x17, 0xDE, 0x43, 0xF9, 0xF9, 0xAD, 0xEE, 0x70
};


static gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(
                                                     GNRC_NETREG_DEMUX_CTX_ALL,
                                                     KERNEL_PID_UNDEF);

#define READER_QUEUE_SIZE (8U)
char _server_stack[THREAD_STACKSIZE_MAIN + THREAD_EXTRA_STACKSIZE_PRINTF];

static kernel_pid_t _dtls_kernel_pid;

/**
 * @brief This care about getting messages and continue with the DTLS flights
 */
static void dtls_handle_read(dtls_context_t *ctx, gnrc_pktsnip_t *pkt)
{

    static session_t session;

    /*
     * NOTE: GNRC (Non-socket) issue: we need to modify the current
     * DTLS Context for the IPv6 src (and in a future the port src).
     */

    /* Taken from the tftp server example */
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    gnrc_pktsnip_t *tmp2;

    tmp2 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    ipv6_hdr_t *hdr = (ipv6_hdr_t *)tmp2->data;

    ipv6_addr_to_str(addr_str, &hdr->src, sizeof(addr_str));
    /* This is unique to the server (Non-socket) */
    ctx->app = addr_str;

    /*
     * TODO: More testings with TinyDTLS is neccesary, but seem this is safe.
     */
    tmp2 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_UDP);
    udp_hdr_t *udp = (udp_hdr_t *)tmp2->data;

    session.size = sizeof(ipv6_addr_t) + sizeof(unsigned short);
    session.port = byteorder_ntohs(udp->src_port);

    ipv6_addr_from_str(&session.addr, addr_str);

    dtls_handle_message(ctx, &session, pkt->data, (unsigned int)pkt->size);

}


/**
 * @brief We got the TinyDTLS App Data message and answer with the same
 */
static int read_from_peer(struct dtls_context_t *ctx,
                          session_t *session, uint8 *data, size_t len)
{


#if ENABLE_DEBUG == 1
  size_t i;
  DEBUG("\nDBG-Server: Data from Client: ---");
    for (i = 0; i < len; i++)
        DEBUG("%c", data[i]);
    DEBUG("--- \t Sending echo..\n");
#endif
    /* echo incoming application data */
    dtls_write(ctx, session, data, len);
    return 0;
}

/**
 * @brief This will try to transmit using only GNRC stack (non-socket).
 */
static int gnrc_sending(char *addr_str, char *data, size_t data_len, unsigned short rem_port )
{
    int iface;
    ipv6_addr_t addr;
    gnrc_pktsnip_t *payload, *udp, *ip;

    /* get interface, if available */
    iface = ipv6_addr_split_iface(addr_str);
    if ((iface < 0) && (gnrc_netif_numof() == 1)) {
        iface = gnrc_netif_iter(NULL)->pid;
    }
    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return -1;
    }

    payload = gnrc_pktbuf_add(NULL, data, data_len, GNRC_NETTYPE_UNDEF);

    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return -1;
    }

    /* allocate UDP header */
    udp = gnrc_udp_hdr_build(payload, DEFAULT_PORT, rem_port);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        gnrc_pktbuf_release(payload);
        return -1;
    }

    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);
    if (ip == NULL) {
        puts("Error: unable to allocate IPv6 header");
        gnrc_pktbuf_release(udp);
        return -1;
    }
    /* add netif header, if interface was given */
    if (iface > 0) {
        gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

        ((gnrc_netif_hdr_t *)netif->data)->if_pid = (kernel_pid_t)iface;
        LL_PREPEND(ip, netif);
    }
    /* send packet */

    DEBUG("DBG-Server: Sending record to peer\n");

    /*
     * WARNING: Too fast and the nodes dies in middle of retransmissions.
     *         This issue appears in the FIT-Lab (m3 motes).
     */
    xtimer_usleep(500000);

    /* Probably this part will be removed.  **/
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
        puts("Error: unable to locate UDP thread");
        gnrc_pktbuf_release(ip);
        return -1;
    }

    return 1;
}

/**
 * @brief We communicate with the other peer.
 */
static int send_to_peer(struct dtls_context_t *ctx,
                        session_t *session, uint8 *buf, size_t len)
{

    (void) session;

    /*FIXME TODO: dtls_get_app_data(ctx) should have the remote port! */
    char *addr_str;
    addr_str = (char *)dtls_get_app_data(ctx);

    gnrc_sending(addr_str, (char *)buf, len, session->port);

    return len;
}

#ifdef DTLS_PSK
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int peer_get_psk_info(struct dtls_context_t *ctx, const session_t *session,
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
        { (unsigned char *)"Client_identity", 15,
          (unsigned char *)"secretPSK", 9 },
        { (unsigned char *)"default identity", 16,
          (unsigned char *)"\x11\x22\x33", 3 },
        { (unsigned char *)"\0", 2,
          (unsigned char *)"", 1 }
    };

    if (type != DTLS_PSK_KEY) {
        return 0;
    }

    if (id) {
        unsigned int i;
        for (i = 0; i < sizeof(psk) / sizeof(struct keymap_t); i++) {
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
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static int peer_get_ecdsa_key(struct dtls_context_t *ctx,
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

    *result = &ecdsa_key;
    return 0;
}

static int peer_verify_ecdsa_key(struct dtls_context_t *ctx,
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
    return 0;
}
#endif /* DTLS_ECC */

/**
 * @brief We prepare the DTLS for this node.
 */
static void init_dtls(void)
{
    static dtls_handler_t cb = {
        .write = send_to_peer,
        .read  = read_from_peer,
        .event = NULL,
#ifdef DTLS_PSK
        .get_psk_info = peer_get_psk_info,
#endif  /* DTLS_PSK */
#ifdef DTLS_ECC
        .get_ecdsa_key = peer_get_ecdsa_key,
        .verify_ecdsa_key = peer_verify_ecdsa_key
#endif  /* DTLS_ECC */
    };


#ifdef DTLS_PSK
    puts("Server support PSK");
#endif
#ifdef DTLS_ECC
    puts("Server support ECC");
#endif

    DEBUG("DBG-Server On\n");

    /*
     * The context for the server is a little different from the client.
     * The simplicity of GNRC do not mix transparently with
     * the DTLS Context. At this point, the server need a fresh context
     * however dtls_context->app must be populated with an unknown
     * IPv6 address.
     *
     * The non-valid Ipv6 address ( :: ) is discarded due the chaos.
     * For now, the first value will be the loopback.
     */
    char *addr_str = "::1";

    /*akin to syslog: EMERG, ALERT, CRITC, NOTICE, INFO, DEBUG */
    dtls_set_log_level(DTLS_LOG_DEBUG);


    dtls_context = dtls_new_context(addr_str);
    if (dtls_context) {
        dtls_set_handler(dtls_context, &cb);
    }
    else {
        puts("Server was unable to generate DTLS Context!");
        exit(-1);
    }



}

/* NOTE: wrapper or trampoline ? (Syntax question) */

void *dtls_server_wrapper(void *arg)
{
    (void) arg; /* TODO: Remove? We don't have args at all (NULL) */

    msg_t _reader_queue[READER_QUEUE_SIZE];
    msg_t msg;

    /* The GNRC examples uses packet dump but we want a custom one */
    msg_init_queue(_reader_queue, READER_QUEUE_SIZE);

    init_dtls();

    /*
     * FIXME: After mutliple retransmissions, and canceled client's sessions
     * the server become unable to sent NDP NA messages. Still, the TinyDTLS
     * debugs seems to be fine.
     */

    while (1) {

        /* wait for a message */
        msg_receive(&msg);

        DEBUG("DBG-Server: Record Rcvd!\n");
        dtls_handle_read(dtls_context, (gnrc_pktsnip_t *)(msg.content.ptr));

        /*TODO: What happens with other clients connecting at the same time? */

    } /*While */

    dtls_free_context(dtls_context);
}

static void start_server(void)
{
    uint16_t port;

    port = (uint16_t)DEFAULT_PORT;

    (void) _dtls_kernel_pid;

    /* Only one instance of the server */
    if (server.target.pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running\n");
        return;
    }

    /*TESTING tinydtls*/
    dtls_init();

    /* The server is initialized  */
    server.target.pid = thread_create(_server_stack, sizeof(_server_stack),
                               THREAD_PRIORITY_MAIN - 1,
                               THREAD_CREATE_STACKTEST,
                               dtls_server_wrapper, NULL, "DTLS Server");

    server.demux_ctx = (uint32_t)port;

    if (gnrc_netreg_register(GNRC_NETTYPE_UDP, &server) == 0)
      printf("Success: started DTLS server on port %" PRIu16 "\n", port);
   else
      printf("FAILURE: The UDP port is not registered!\n");
}

static void stop_server(void)
{
    /* check if server is running at all */
    if (server.target.pid == KERNEL_PID_UNDEF) {
        printf("Error: server was not running\n");
        return;
    }

    dtls_free_context(dtls_context);

    /* stop server */
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &server);
    server.target.pid = KERNEL_PID_UNDEF;
    puts("Success: stopped DTLS server");
}

int udp_server_cmd(int argc, char **argv)
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
        puts("error: invalid command");
    }
    return 0;
}
