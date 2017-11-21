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

#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"
#include "timex.h"
#include "utlist.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* TinyDTLS */
#include "tinydtls.h"
#include "dtls_debug.h"
#include "dtls.h"
#include "global.h"



/* TODO: Remove the UNUSED_PARAM from TinyDTLS' stack? */
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

//#define DEFAULT_PORT 20220    /* DTLS default port  */
#define DEFAULT_PORT 61618      /* First valid FEBx address  */

#define CLIENT_PORT  DEFAULT_PORT + 1
#define MAX_TIMES_TRY_TO_SEND 10

static dtls_context_t *dtls_context = NULL;
static char *client_payload;
static size_t buflen = 0;

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


/**
 * @brief This care about getting messages and continue with the DTLS flights.
 * This will handle all the packets arriving to the node.
 * Will determine if its from a new DTLS peer or a previous one.
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

    /*
       *TODO: More testings with TinyDTLS is neccesary, but seem this is safe.
     */
    tmp2 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_UDP);
    udp_hdr_t *udp = (udp_hdr_t *)tmp2->data;

    session.size = sizeof(ipv6_addr_t) + sizeof(unsigned short);
    session.port = byteorder_ntohs(udp->src_port);

    DEBUG("DBG-Client: Msg received from \n\t Addr Src: %s"
          "\n\t Current Peer: %s \n", addr_str, (char *)dtls_get_app_data(ctx));

    ipv6_addr_from_str(&session.addr, addr_str);

    dtls_handle_message(ctx, &session, pkt->data, (unsigned int)pkt->size);

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
            /* Removed due probably in the motes is useless
               if (id_len) {
               dtls_debug("got psk_identity_hint: '%.*s'\n", id_len, id);
               }
             */

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
    (void) key_size;
    (void) other_pub_x;
    (void) other_pub_y;
    return 0;
}
#endif /* DTLS_ECC */

/**
 * @brief This will try to transmit using only GNRC stack.
 * This is basically the original send function from gnrc/networking
 */
static int gnrc_sending(char *addr_str, char *data, size_t data_len )
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

    /*  allocate payload */
    payload = gnrc_pktbuf_add(NULL, data, data_len, GNRC_NETTYPE_UNDEF);

    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return -1;
    }

    /* allocate UDP header */
    udp = gnrc_udp_hdr_build(payload, (uint16_t) CLIENT_PORT, (uint16_t) DEFAULT_PORT);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        gnrc_pktbuf_release(payload);
        return -1;
    }

    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL,  &addr);
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

    /*
     * WARNING: Too fast and the nodes dies in middle of retransmissions.
     *          This issue appears in the FIT-Lab (m3 motes).
     *          In native, is not required.
     */
    xtimer_usleep(500000);

    /* send packet */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
        puts("Error: unable to locate UDP thread");
        gnrc_pktbuf_release(ip);
        return -1;
    }

    return 1;
}

/**
 * @brief This will print the data read from the Peer.
 * THIS AT THE END of the 6 flights (DTLS App Data).
 * Is here where we know that the connection has finished.
 * TODO: The connected variable could de modified here.
 */
static int read_from_peer(struct dtls_context_t *ctx,
               session_t *session, uint8 *data, size_t len)
{
    /* Linux and Contiki version are exactly the same. */
    (void) session;
    (void) ctx;
    size_t i;
    printf("\n\n Echo received: ");
    for (i = 0; i < len; i++)
        printf("%c", data[i]);
    printf(" \n\n\n");
    return 0;
}

/**
 * @brief  Will try to transmit the next DTLS flight for a speicifc Peer.
 * NOTE:The global buff and buflen is used for be able to transmit the
 * Payload in segmented datagrams.
 */
static void try_send(struct dtls_context_t *ctx, session_t *dst)
{

    int res;

    res = dtls_write(ctx, dst, (uint8_t *)client_payload, buflen);

    if (res >= 0) {
        memmove(client_payload, client_payload + res, buflen - res);
        buflen -= res;
    }
    else {
        DEBUG("DBG-Client: dtls_write returned error!\n" );
    }
}

/**
 * @brief This SIGNAL function will prepare the next DTLS flight to send.
 */
static int send_to_peer(struct dtls_context_t *ctx,
             session_t *session, uint8 *buf, size_t len)
{

    (void) session;
    /*
     * For this testing with GNR we are to extract the peer's addresses and
     * making the connection from zero.
     */
    char *addr_str;
    addr_str = (char *)dtls_get_app_data(ctx);


    /* TODO: Confirm that indeed len size of data was sent, otherwise
     * return the correct number of bytes sent
     */
    gnrc_sending(addr_str, (char *)buf, len);

    return len;
}



/***
 *  This is a custom function for preparing the SIGNAL events and
 *  create a new DTLS context.
 */
static void init_dtls(session_t *dst, char *addr_str)
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
    puts("Client support PSK");
#endif
#ifdef DTLS_ECC
    puts("Client support ECC");
#endif

    DEBUG("DBG-Client: Debug ON");
    /*
     *  The objective of ctx->App  is be able to retrieve
     *  enough information for restablishing a connection.
     *  This is to be used in the send_to_peer and (potentially) the
     *  read_from_peer, to continue the transmision with the next
     *  step of the DTLS flights.
     *  TODO: Take away the DEFAULT_PORT and CLIENT_PORT.
     */
    dst->size = sizeof(ipv6_addr_t) + sizeof(unsigned short);
    dst->port =  (unsigned short) DEFAULT_PORT;

    if (ipv6_addr_from_str(&dst->addr, addr_str) == NULL) {
        puts("ERROR: init_dtls was unable to load the IPv6 addresses!\n");
        dtls_context = NULL;
        return;
    }

    ipv6_addr_t addr_dbg;
    ipv6_addr_from_str(&addr_dbg, addr_str);

    /*akin to syslog: EMERG, ALERT, CRITC, NOTICE, INFO, DEBUG */
    dtls_set_log_level(DTLS_LOG_NOTICE);

    dtls_context = dtls_new_context(addr_str);
    if (dtls_context) {
        dtls_set_handler(dtls_context, &cb);
    }

    return;
}

/**
 * This is the "client" part of this program.
 * Will be called each time a message is transmitted.
 */
static void client_send(char *addr_str, char *data, unsigned int delay)
{
    int8_t iWatch;
    static session_t dst;
    static int connected = 0;
    msg_t msg;

   gnrc_netreg_entry_t entry = GNRC_NETREG_ENTRY_INIT_PID(CLIENT_PORT,
                                                           sched_active_pid);
    dtls_init();

    if (gnrc_netreg_register(GNRC_NETTYPE_UDP, &entry)) {
        puts("Unable to register ports");
        /*FIXME: Release memory?*/
        return;
    }

    if (strlen(data) > DTLS_MAX_BUF) {
        puts("Data too long ");
        return;
    }

    init_dtls(&dst, addr_str);
    if (!dtls_context) {
        dtls_emerg("cannot create context\n");
        puts("Client unable to load context!");
        return;
    }

    /* client_payload is global due to the SIGNAL function send_to_peer  */
    client_payload = data;
    buflen =  strlen(client_payload);
    iWatch = MAX_TIMES_TRY_TO_SEND;

    /*
     * dtls_connect is the one who begin all the process.
     * However, do it too fast, and the node will attend it before having a
     * valid IPv6 or even a route to the destiny (This could be verified as the
     * sequence number of the first DTLS Hello message will be greater than
     * zero).
     */
    //connected = dtls_connect(dtls_context, &dst) >= 0;

    /*
     * Until all the data is not sent we remains trying to connect to the
     * server. Plus a small watchdog.
     */
    while ((buflen > 0) && (iWatch > 0)) {

        /*
         * NOTE: I (rfuentess) personally think this should be until this point
         * instead before (that is why the previous  dtls_connect is by defualt
         * commented..
         */
        if (!connected) {
            connected = dtls_connect(dtls_context, &dst);
        }
        else if (connected < 0) {
            puts("Client DTLS was unable to establish a channel!\n");
            /*NOTE: Not sure what to do in this scenario (if can happens)*/
        }
        else {
            /*TODO: must happens always or only when connected?*/
            try_send(dtls_context, &dst);
        }

        /*
         * WARNING: The delay is KEY HERE!  Too fast, and we can kill the
         * DTLS state machine.  Another alternative is change to
         * blocking states (making the watchdog useless)
         *
         * msg_receive(&msg);
         */

        xtimer_usleep(delay);

        if (msg_try_receive(&msg) == 1) {
            dtls_handle_read(dtls_context, (gnrc_pktsnip_t *)(msg.content.ptr));
        }

        iWatch--;
    } /*END while*/

    dtls_free_context(dtls_context);
    /* unregister our UDP listener on this thread */
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &entry);
    connected = 0; /*Probably this should be removed or global */

    /* Permanent or not permanent? */
    DEBUG("DTLS-Client: DTLS session finished\n");
}

int udp_client_cmd(int argc, char **argv)
{
    uint32_t delay = 1000000;

    if (argc < 3) {
        printf("usage: %s <addr> <data> [<delay in us>]\n", argv[0]);
        return 1;
    }
    else if (argc > 3) {
        delay = atoi(argv[3]);
    }
    client_send(argv[1], argv[2],  delay);


    return 0;
}
