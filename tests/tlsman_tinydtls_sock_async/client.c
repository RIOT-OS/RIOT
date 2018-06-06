/*
 * Copyright (C) 2018 Inria
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
 * @brief       tlsman test application (client side)
 *
 * Small test for TLSMAN. Many definitions defined here are also available at
 * sock_secure (and are intended to be used in standard applications)
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */


#include <stdio.h>
#include <assert.h>

#include "net/sock/udp.h"
#include "net/tlsman.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef REMOTE_SERVER
#define REMOTE_SERVER "fd00:dead:beef::1"
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 20220
#endif

#ifndef CLIENT_PORT
#define CLIENT_PORT SERVER_PORT + 5
#endif

#define CALL(fn)            puts("Calling " # fn); fn;

#define READER_CLIENT_QUEUE_SIZE (8U)

/* Expected IPC messages  */
enum {
    IPHC_MSG_TEST1 = 0x4000,
    IPHC_MSG_TEST2,           /**< First DTLS Client Hello sent */
    IPHC_MSG_TEST3,        /**< DTLS Handshake finished */
    IPHC_MSG_TEST4,      /**< Signals that a new handshake must take place */
    IPHC_MSG_TEST5,
    IPHC_MSG_TEST6,
    IPHC_MSG_TEST7,
    IPHC_MSG_TEST8,
    IPHC_MSG_TEST9,
    IPHC_MSG_TEST10,
    IPHC_MSG_TEST11
};


/* A small delay for debugging more easily network traffic */
#ifdef ENABLE_DEBUG
#define DBG_DELAY() xtimer_sleep(3)
#else
#define DBG_DELAY()
#endif


void * client_wrapper(void *arg);
extern tlsman_driver_t tlsman_session; /* Defined in main.c */

/**
 * Our custom response handler to be sent to TLSMAN
 * It will be invoked each time a DTLS data app record is retrieved
 *
 * a sock, socket or other network stack.
 */
static void _resp_handler(uint8_t *data, size_t data_size, void *sock);

char client_thread_stack[THREAD_STACKSIZE_MAIN +
                                THREAD_EXTRA_STACKSIZE_PRINTF];

static uint8_t _upper_data[] = "Hello world!\n";

static void _resp_handler(uint8_t *data, size_t data_size, void *sock)
{

    (void) sock;

    DEBUG("Echo (string %i): %s\n", data_size, data);
    /* We are expecting an echo */
    assert((0 == strncmp((char *) data, (char *) _upper_data,
                         sizeof(_upper_data))) &&
           (sizeof(_upper_data) == data_size));
}

static void test_creating_context(sock_udp_ep_t *local, sock_udp_ep_t *remote,
                                  sock_udp_t *udp_sock,
                                  tlsman_driver_t *dtls_session,
                                  uint8_t tlsman_flags)
{

    local->port = (unsigned short) CLIENT_PORT;
    remote->port = (unsigned short) SERVER_PORT;

    /* Socks must be ready before step 3 */
    assert(ipv6_addr_from_str((ipv6_addr_t *)remote->addr.ipv6, REMOTE_SERVER));
    assert(0 == sock_udp_create(udp_sock, local, remote, 0));

    /* NOTE tlsman_session_t must be populated according to the network stack */
    dtls_session->session.local = local;
    dtls_session->session.remote = remote;
    dtls_session->session.sock = udp_sock;

    assert(0 ==
           dtls_session->tlsman_init_context( &dtls_session->session, _resp_handler,
                                              tlsman_flags));


}

static void test_creating_channel(tlsman_driver_t *dtls_session,
                                  event_queue_t *sock_event_queue,
                                  uint8_t tlsman_flags, uint8_t *packet_rcvd)
{

    dtls_session->tlsman_set_async_parameters(&dtls_session->session, sock_event_queue, packet_rcvd, DTLS_MAX_BUF);

    /*
     *  NOTE: With sock_async, the packet_rcvd buffer is linked by the
     * call of tlsman_set_async_parameters()
     */
    dtls_session->tlsman_create_channel(&dtls_session->session, tlsman_flags, NULL, 0);

    /* NOTE: Assert is not called as the next test is equivalent */
}

static void test_sending_upper_data(tlsman_driver_t *dtls_session)
{
    /*
     * NOTE: The use of msg_send_receive()/msg_reply() makes this valid
     */
    assert(dtls_session->tlsman_is_channel_ready(&dtls_session->session));

    /* NOTE: It's possible to share the buffer for sending and receiving */
    assert(0 ==
           dtls_session->tlsman_send_data_app(&dtls_session->session, _upper_data,
                                              sizeof(_upper_data)));
}

static void test_release_resources_IGNORING_ALERT(tlsman_driver_t *dtls_session)
{
    dtls_session->tlsman_release_resources(&dtls_session->session);
}

static void test_reconnect_COMMON_ERROR(sock_udp_ep_t *local, sock_udp_ep_t *remote,
                                        sock_udp_t *udp_sock,
                                        event_queue_t *sock_event_queue,
                                        tlsman_driver_t *dtls_session,
                                        uint8_t tlsman_flags, uint8_t *packet, size_t pkt_size)
{
    DBG_DELAY();

    /* This fail is due a DTLS Alert record sent by the remote node as result from the previous test.
       Therefore for this test, we only try to do a new handshake.
       NOTE: This affects both way of operation (synchronous and asynchronous).
     */

    /* FIXME? tlsman_session_t must be populated according to the network stack */
    dtls_session->session.local = local;
    dtls_session->session.remote = remote;
    dtls_session->session.sock = udp_sock;
    assert(0 == dtls_session->tlsman_init_context( &dtls_session->session, _resp_handler,
                                                   tlsman_flags));

    /* With async this is strict order! */
    /* TODO FIXME tlsman_set_async_listening() should not be integrated into tlsman_retrieve_data_app()? */
    dtls_session->tlsman_set_async_listening();
    dtls_session->tlsman_retrieve_data_app(&dtls_session->session, packet, pkt_size);

    dtls_session->tlsman_set_async_parameters(&dtls_session->session, sock_event_queue, packet, pkt_size);
    ssize_t res = dtls_session->tlsman_create_channel(&dtls_session->session, tlsman_flags, packet,
                                              pkt_size);

    assert((res != TLSMAN_ERROR_HANDSHAKE_TIMEOUT) &&
           (!tlsman_process_is_error_code_nonfatal(res)));
}



static void test_sending_upper_data_MULTIPLE(tlsman_driver_t *dtls_session)
{
    /* Testing encryption and not more renegotiation between peers */
    assert(dtls_session->tlsman_is_channel_ready(&dtls_session->session));

    /* NOTE: It's possible to share the buffer for sending and receiving */
    assert(0 == dtls_session->tlsman_send_data_app(&dtls_session->session, _upper_data, sizeof(_upper_data)));
    assert(0 == dtls_session->tlsman_send_data_app(&dtls_session->session, _upper_data, sizeof(_upper_data)));
    assert(0 == dtls_session->tlsman_send_data_app(&dtls_session->session, _upper_data, sizeof(_upper_data)));
    assert(0 == dtls_session->tlsman_send_data_app(&dtls_session->session, _upper_data, sizeof(_upper_data)));
}

static void test_reconnect_CLOSING_SOCKETS(sock_udp_ep_t *local, sock_udp_ep_t *remote,
                                           sock_udp_t *udp_sock,
                                           event_queue_t *sock_event_queue,
                                           tlsman_driver_t *dtls_session,
                                           uint8_t tlsman_flags, uint8_t *packet, size_t pkt_size)
{
    DBG_DELAY();
    /* Equivalent to test_release_resources_IGNORING_ALERT */
    dtls_session->tlsman_release_resources(&dtls_session->session);

    /*
     * This test tries more safe approach than test_reconnect_COMMON_ERROR()
     * By closing the udp sock we are also dropping any remaining DTLS record on the buffer */
    /* FIXME? tlsman_session_t must be populated according to the network stack */
    dtls_session->session.local = local;
    dtls_session->session.remote = remote;
    dtls_session->session.sock = udp_sock;
    assert(0 == dtls_session->tlsman_init_context( &dtls_session->session,
                                                   _resp_handler,
                                                   tlsman_flags));

    ssize_t res = dtls_session->tlsman_create_channel(&dtls_session->session, tlsman_flags, packet,
                                                      pkt_size);

    assert(res == TLSMAN_ERROR_HANDSHAKE_TIMEOUT);

    /* This fail is due a DTLS Alert record sent by the remote node as result from the previous test.
       Therefore for this test, we only try to do a new handshake
     */

    /* This is for fixing our internal chaos */
    dtls_session->tlsman_close_channel(&dtls_session->session);
    sock_udp_close(udp_sock);
    sock_udp_create(udp_sock, (sock_udp_ep_t *)local, (sock_udp_ep_t *)remote, 0);

    /* Strict order with async */
    dtls_session->tlsman_set_async_parameters(&dtls_session->session, sock_event_queue, packet, pkt_size);
    res = dtls_session->tlsman_create_channel(&dtls_session->session, tlsman_flags, packet,
                                              pkt_size);

    assert((res != TLSMAN_ERROR_HANDSHAKE_TIMEOUT) &&
           (!tlsman_process_is_error_code_nonfatal(res)));

}

static void test_renegotiation(tlsman_driver_t *dtls_session,
                               uint8_t tlsman_flags,
                               uint8_t *packet, size_t pkt_size)
{
    DBG_DELAY();

    /* NOTE: There is a known issue in tinydtls' commit for this test */
    assert(TLSMAN_ERROR_SESSION_REN !=
           dtls_session->tlsman_renegotiate(&dtls_session->session, tlsman_flags, packet,
                                            pkt_size));
}


static void test_chanel_working(tlsman_driver_t *dtls_session)
{
    /* This is equivalent to test_sending_upper_data() */
    assert(dtls_session->tlsman_is_channel_ready(&dtls_session->session));
    assert(0 == dtls_session->tlsman_send_data_app(&dtls_session->session, _upper_data, sizeof(_upper_data)));
}

static void test_rehandshake(tlsman_driver_t *dtls_session,
                             uint8_t tlsman_flags,
                             uint8_t *packet, size_t pkt_size)
{
    DBG_DELAY();

    assert(0 == dtls_session->tlsman_rehandshake(&dtls_session->session, tlsman_flags,
                                                 packet, pkt_size));
}

static void test_closing_channel(tlsman_driver_t *dtls_session,
                                 uint8_t *packet, size_t pkt_size)
{
    assert(TLSMAN_ERROR_UNABLE_CLOSE != dtls_session->tlsman_close_channel(&dtls_session->session));
    dtls_session->tlsman_set_async_listening();
    dtls_session->tlsman_retrieve_data_app(&dtls_session->session, packet, sizeof(pkt_size));

    /* Release resources */
    dtls_session->tlsman_release_resources(&dtls_session->session);
}

void * client_wrapper(void *arg)
{
    uint8_t *tlsman_flags = (uint8_t *) arg;

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_t udp_sock;
    event_queue_t sock_event_queue;

    /* This is the buffer to be used for sending AND receiving DTLS Records */
    uint8_t packet_rcvd[DTLS_MAX_BUF];

    bool active = true;
    msg_t _reader_queue[READER_CLIENT_QUEUE_SIZE];
    msg_t msg;

    /* Prepare (thread) messages reception */
    msg_init_queue(_reader_queue, READER_CLIENT_QUEUE_SIZE);

    DEBUG("Remote server: [%s]:%u\n", REMOTE_SERVER, SERVER_PORT);

    while (active) {
      msg_receive(&msg);
      switch(msg.type) {
        /* Create the (D)TLS context and load the keys  */
        case IPHC_MSG_TEST1:
          CALL(test_creating_context(&local, &remote, &udp_sock,
                                     &tlsman_session, *tlsman_flags));
        break;

        /* Start the (D)TLS channel! (the handshake process) */
        case IPHC_MSG_TEST2:
          CALL(test_creating_channel(&tlsman_session,
                                     &sock_event_queue,
                                     *tlsman_flags, packet_rcvd));
        break;

        /* Sending (and receiving data in asynchronous) */
        case IPHC_MSG_TEST3:
          CALL(test_sending_upper_data(&tlsman_session));
        break;

        /* Wrongly releasing resources and reconnect */
        case IPHC_MSG_TEST4:
          CALL(test_release_resources_IGNORING_ALERT(&tlsman_session));
        break;

        case IPHC_MSG_TEST5:
          CALL(test_reconnect_COMMON_ERROR(&local, &remote, &udp_sock,   \
                                     &sock_event_queue, &tlsman_session, \
                                     *tlsman_flags, packet_rcvd, sizeof(packet_rcvd)));
        break;

        case IPHC_MSG_TEST6:
          CALL(test_sending_upper_data_MULTIPLE(&tlsman_session));
        break;

        case IPHC_MSG_TEST7:
          CALL(test_reconnect_CLOSING_SOCKETS(&local, &remote, &udp_sock,   \
                                     &sock_event_queue, &tlsman_session, \
                                     *tlsman_flags, packet_rcvd, sizeof(packet_rcvd)) );
        break;

        /*  renegotiation of (D)TLS session  */
        case IPHC_MSG_TEST8:
#if 0  /* WARNING: TinyDTLS has a known bug with this test */
        CALL(test_renegotiation(&tlsman_session, *tlsman_flags, packet_rcvd, sizeof(packet_rcvd)));
        CALL(test_chanel_working(&tlsman_session));
#else
        (void) test_renegotiation;
        (void) test_chanel_working;
#endif
        break;

        /*  Renegotiation with existing parameters (RFC 6347 4.2.7) */
        case IPHC_MSG_TEST9:
#if 0  /* WARNING: TinyDTLS has a known bug with this test */
        CALL(test_rehandshake(&tlsman_session, *tlsman_flags,  packet_rcvd, sizeof(packet_rcvd)));
        CALL(test_chanel_working(&tlsman_session));
#else
        (void) test_rehandshake;
        (void) test_chanel_working;
#endif
        break;

        case IPHC_MSG_TEST10:
          CALL(test_chanel_working(&tlsman_session));
        break;

        case IPHC_MSG_TEST11:
          CALL(test_closing_channel(&tlsman_session, packet_rcvd, DTLS_MAX_BUF));
          active = false;
        break;

        default:
          /* FIXME Remove this! (or don't use msg_try_receive()?) */
          DEBUG("unknown IPHC Message! %i\n", msg.type);
      } /* switch */

      msg_reply(&msg, &msg);
    } /* while */

    puts("ALL TESTS SUCCESSFUL");

    return (void *) NULL;
}
