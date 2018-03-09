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

/**
 * Our custom response handler to be sent to TLSMAN
 * It will be inkoved each time a DTLS data app record is retrieved
 *
 * NOTE (TODO): The (void) * argument is more like a comodin as TLSMAN can deliver
 * a sock, socket or other network stack.
 */
static void _resp_handler(uint8_t *data, size_t data_size, void *sock);

#define CALL(fn)            puts("Calling " # fn); fn;

#ifndef REMOTE_SERVER
#define REMOTE_SERVER "fd00:dead:beef::1"
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 20220
#endif

#ifndef CLIENT_PORT
#define CLIENT_PORT SERVER_PORT + 5
#endif

/* A small delay for debugging more easily network traffic */
#ifdef ENABLE_DEBUG
#define DBG_DELAY() xtimer_sleep(3)
#else
#define DBG_DELAY()
#endif

#ifdef SOCK_HAS_ASYNC
static event_queue_t sock_event_queue;
#endif

static uint8_t _upper_data[] = "Hello world!\n";

static void _resp_handler(uint8_t *data, size_t data_size, void *sock)
{

    (void) sock; /* For this test, our comodin (sock or socket) is irrelevant */

    DEBUG("Echo (string %i): %s\n", data_size, data);
    /* We are expecting an echo */
    assert((0 == strncmp((char *) data, (char *) _upper_data,
                         sizeof(_upper_data))) &&
           (sizeof(_upper_data) == data_size));
}

static void test_creating_context(sock_udp_ep_t *local, sock_udp_ep_t *remote,
                                  sock_udp_t *udp_sock,
                                  tlsman_session_t *dtls_session,
                                  uint8_t tlsman_flags)
{

    local->port = (unsigned short) CLIENT_PORT;
    remote->port = (unsigned short) SERVER_PORT;

    ssize_t res;

    /* Socks must be ready before step 3 */
    assert(ipv6_addr_from_str((ipv6_addr_t *)remote->addr.ipv6, REMOTE_SERVER));
    assert(0 == sock_udp_create(udp_sock, local, remote, 0));
    res = tlsman_init_context((tlsman_ep_t *) local, (tlsman_ep_t *) remote,
                              dtls_session, udp_sock, _resp_handler,
                              tlsman_flags);
    assert(res == 0);
}

static void test_creating_channel(tlsman_session_t *dtls_session,
                                  uint8_t tlsman_flags, uint8_t *packet_rcvd)
{
    tlsman_create_channel(dtls_session, tlsman_flags, packet_rcvd, DTLS_MAX_BUF);
    /* NOTE: Assert is not called as the next test is equivalent */
}

static void test_checking_errors(void)
{
    ssize_t res = tlsman_return_last_known_error();

    if (tlsman_process_is_error_code_nonfatal(res)) {
        puts("ERROR: Unable to start (D)TLS handhsake process!");
    }
    else if (res == TLSMAN_ERROR_HANDSHAKE_TIMEOUT) {
        /* NOTE: Handhsake timeout can be not fatal but is part of our test */
        puts("ERROR: (D)TLS handshake timeout!");
    }

    assert((res != TLSMAN_ERROR_HANDSHAKE_TIMEOUT) &&
           (!tlsman_process_is_error_code_nonfatal(res)));

}

static void test_sending_uper_data(tlsman_session_t *dtls_session)
{
    /*
     * For a blocking testing, this would be equivalent to test for
     * TLSMAN_ERROR_HANDSHAKE_TIMEOUT
     */
    assert(tlsman_is_channel_ready(dtls_session));

    /* NOTE: It's possible to share the buffer for sending and receiving */
    assert(0 == tlsman_send_data_app(dtls_session, _upper_data, sizeof(_upper_data)));
}

static void test_retrieve_data(tlsman_session_t *dtls_session, uint8_t *data,
                               size_t data_size)
{
    /* NOTE: This test is synchronous, but the sock_async has impact here */
    /* NOTE: This can retrieve other types of DTLS records (e.g. alerts) */
#ifdef SOCK_HAS_ASYNC
    /* NOTE: If tinydtls is running on it's own thread, tlsman_retrieve_data_app()
     * can be called first.
     * However, for this test is not the case.
     */
    tlsman_set_async_listening();
#endif

    assert(0 == tlsman_retrieve_data_app(dtls_session, data, data_size));

}

static void test_sending_uper_data_MULTIPLE(tlsman_session_t *dtls_session,
                                            uint8_t *data, size_t data_size)
{
    /* Testing encryption and not more renegotiation between peers */
    assert(tlsman_is_channel_ready(dtls_session));

    /* NOTE: It's possible to share the buffer for sending and receiving */
    assert(0 == tlsman_send_data_app(dtls_session, _upper_data, sizeof(_upper_data)));
    assert(0 == tlsman_send_data_app(dtls_session, _upper_data, sizeof(_upper_data)));
    assert(0 == tlsman_send_data_app(dtls_session, _upper_data, sizeof(_upper_data)));
    assert(0 == tlsman_send_data_app(dtls_session, _upper_data, sizeof(_upper_data)));

#ifdef SOCK_HAS_ASYNC
    tlsman_set_async_listening();
#endif

    assert(0 == tlsman_retrieve_data_app(dtls_session, data, data_size));
    assert(0 == tlsman_retrieve_data_app(dtls_session, data, data_size));
    assert(0 == tlsman_retrieve_data_app(dtls_session, data, data_size));
    assert(0 == tlsman_retrieve_data_app(dtls_session, data, data_size));

}

static void test_release_resources_IGNORING_ALERT(tlsman_session_t *dtls_session,
                                                  sock_udp_t *udp_sock,
                                                  uint8_t tlsman_flags, uint8_t *packet_rcvd)
{
    /* FIXME */
    (void) tlsman_flags;
    (void) packet_rcvd;
    (void) udp_sock;

    tlsman_release_resources(dtls_session);

}

static void test_reconnect_COMMON_ERROR(sock_udp_ep_t *local, sock_udp_ep_t *remote,
                                        sock_udp_t *udp_sock, tlsman_session_t *dtls_session,
                                        uint8_t tlsman_flags, uint8_t *packet, size_t pkt_size)
{
    /*
     * This test first attempts to retrieve any (D)TLS record avaiable.
     * With tinyDTLS therere is a high chanche that DTLS Alert record was received.
     * This record can interfere with tlsman_create_channel()
     */

    DBG_DELAY();

    assert(0 == tlsman_init_context((tlsman_ep_t *) local, (tlsman_ep_t *) remote,
                                    dtls_session, udp_sock, _resp_handler,
                                    tlsman_flags));

    ssize_t res = tlsman_create_channel(dtls_session, tlsman_flags, packet,
                                        pkt_size);

    assert(res == TLSMAN_ERROR_HANDSHAKE_TIMEOUT);

    /* This fail is due a DTLS Alert record sent by the remote node as result from the previous test.
       Therefore for this test, we only try to do a new handshake
     */

    /* This is for fixing our intenral chaos */
    tlsman_close_channel(dtls_session);
    tlsman_retrieve_data_app(dtls_session, packet, sizeof(pkt_size));

    sock_udp_close(udp_sock);
    sock_udp_create(udp_sock, (sock_udp_ep_t *)local, (sock_udp_ep_t *)remote, 0);

    res = tlsman_create_channel(dtls_session, tlsman_flags, packet,
                                pkt_size);

    assert((res != TLSMAN_ERROR_HANDSHAKE_TIMEOUT) &&
           (!tlsman_process_is_error_code_nonfatal(res)));

}

static void test_reconnect_CLOSING_SOCKETS(sock_udp_ep_t *local, sock_udp_ep_t *remote,
                                           sock_udp_t *udp_sock, tlsman_session_t *dtls_session,
                                           uint8_t tlsman_flags, uint8_t *packet, size_t pkt_size)
{
    DBG_DELAY();

    /* Equivalent to test_release_resources_IGNORING_ALERT */
    tlsman_release_resources(dtls_session);

    /*
     * This test tries more safe approach than test_reconnect_COMMON_ERROR()
     * By closing the udp sock we are also dropping any remaining DTLS record on the buffer */
    assert(0 == tlsman_init_context((tlsman_ep_t *) local, (tlsman_ep_t *) remote,
                                    dtls_session, udp_sock, _resp_handler,
                                    tlsman_flags));

    ssize_t res = tlsman_create_channel(dtls_session, tlsman_flags, packet,
                                        pkt_size);

    assert(res == TLSMAN_ERROR_HANDSHAKE_TIMEOUT);

    /* This fail is due a DTLS Alert record sent by the remote node as result from the previous test.
       Therefore for this test, we only try to do a new handshake
     */

    /* This is for fixing our intenral chaos */
    tlsman_close_channel(dtls_session);
    sock_udp_close(udp_sock);
    sock_udp_create(udp_sock, (sock_udp_ep_t *)local, (sock_udp_ep_t *)remote, 0);

    res = tlsman_create_channel(dtls_session, tlsman_flags, packet,
                                pkt_size);

    assert((res != TLSMAN_ERROR_HANDSHAKE_TIMEOUT) &&
           (!tlsman_process_is_error_code_nonfatal(res)));

}

static void test_retrieve_data_TIMEOUT(tlsman_session_t *dtls_session, uint8_t *data,
                                       size_t data_size)
{
    /* NOTE: This test is synchronous, but the sock_async has impact here */
    /* NOTE: This can retrieve other types of DTLS records (e.g. alerts) */
#ifdef SOCK_HAS_ASYNC
    /* NOTE: If tinydtls is running on it's own thread, tlsman_retrieve_data_app()
     * can be called first.
     * However, for this test is not the case.
     */
    tlsman_set_async_listening();
#endif

    /* This test should be called without additional calls */
    assert(0 == tlsman_retrieve_data_app(dtls_session, data, data_size));
}

static void test_renegotiation(tlsman_session_t *dtls_session,
                               uint8_t tlsman_flags, uint8_t *packet_rcvd)
{
    DBG_DELAY();

    /* NOTE: There is a known issue in tinydtls' commit for this test */
    assert(TLSMAN_ERROR_SESSION_REN !=
           tlsman_renegotiate_session(dtls_session, tlsman_flags, packet_rcvd,
                                      DTLS_MAX_BUF));
}

static void test_chanel_working(tlsman_session_t *dtls_session, uint8_t *data,
                                size_t data_size)
{
    /* This is equivalent to test_retrieve_data() and test_sending_uper_data() */

    assert(tlsman_is_channel_ready(dtls_session));
    assert(0 == tlsman_send_data_app(dtls_session, _upper_data, sizeof(_upper_data)));

#ifdef SOCK_HAS_ASYNC
    tlsman_set_async_listening();
#endif
    assert(0 == tlsman_retrieve_data_app(dtls_session, data, data_size));

}

static void test_rehandshake(tlsman_session_t *dtls_session,
                             uint8_t tlsman_flags, uint8_t *packet_rcvd)
{
    DBG_DELAY();

    assert(0 == tlsman_new_handshake(dtls_session, tlsman_flags,
                                     packet_rcvd, DTLS_MAX_BUF));
}

static void test_closing_channel(tlsman_session_t *dtls_session,
                                 uint8_t *packet, size_t pkt_size)
{
    assert(TLSMAN_ERROR_UNABLE_CLOSE != tlsman_close_channel(dtls_session));
    tlsman_retrieve_data_app(dtls_session, packet, sizeof(pkt_size));

    /* Release resources */
    tlsman_release_resources(dtls_session);
}

void client_side(void)
{

    /*
     * TLSMAN handles everything related to the (D)TLS stacks but not the
     * the network stack itself. Additionally, there are limits:
     * - tinyDTLS only supports socks or socket (TODO)
     * - WolfSSL: TODO
     */

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_t udp_sock;
    uint8_t tlsman_flags = TLSMAN_FLAG_STACK_UNIVERSAL | TLSMAN_FLAG_SIDE_CLIENT;

    /* Storage for the context and secure session (for all the (D)TLS stacks) */
    tlsman_session_t dtls_session;

    /* This is the buffer to be used for sending AND receiving DTLS Records */
    uint8_t packet_rcvd[DTLS_MAX_BUF];

    DEBUG("Remote server: [%s]:%u\n", REMOTE_SERVER, SERVER_PORT);

    /* 2nd step: Create the (D)TLS context and load the keys */
    /* NOTE: 1st step is located at main.c */
    CALL(test_creating_context(&local, &remote, &udp_sock, &dtls_session, tlsman_flags));

#ifdef SOCK_HAS_ASYNC
    /* This step is also required before tlsman_create_channel() */
    tlsman_set_async_parameters(&dtls_session, &sock_event_queue, packet_rcvd, sizeof(packet_rcvd));
    tlsman_flags |= TLSMAN_FLAG_NONBLOCKING;
#endif

    /* 3th step: Start the (D)TLS channel! (the handshake process) */
    CALL(test_creating_channel(&dtls_session, tlsman_flags, packet_rcvd));

    /* 4th step: The DTLS handshake was started correctly? */
    CALL(test_checking_errors());

    /* 5th step: Send and receiving data */
    CALL(test_sending_uper_data(&dtls_session));
    CALL(test_retrieve_data(&dtls_session, packet_rcvd, DTLS_MAX_BUF));

    /* 6th step: Wrongly releasing resources and reconnect */
    CALL(test_release_resources_IGNORING_ALERT(&dtls_session, &udp_sock, 0, NULL));
    CALL(test_reconnect_COMMON_ERROR(&local, &remote, &udp_sock, &dtls_session, \
                                     tlsman_flags, packet_rcvd, sizeof(packet_rcvd)));
    CALL(test_sending_uper_data_MULTIPLE(&dtls_session, packet_rcvd, DTLS_MAX_BUF));
    CALL(test_reconnect_CLOSING_SOCKETS(&local, &remote, &udp_sock, &dtls_session, \
                                        tlsman_flags, packet_rcvd, sizeof(packet_rcvd)));
    CALL(test_retrieve_data_TIMEOUT(&dtls_session, packet_rcvd, DTLS_MAX_BUF));

#if 0 /* WARNING: TinyDTLS has a known bug with this test */
      /* 6th step: renegotiation of (D)TLS session */
    CALL(test_renegotiation(&dtls_session, tlsman_flags, packet_rcvd));

    /* 7th step: Send/Retrieve data after the renegotiation (again) */
    CALL(test_chanel_working(&dtls_session, packet_rcvd, DTLS_MAX_BUF));
#else
    (void) test_renegotiation;
    (void) test_chanel_working;
#endif

#if 0 /* WARNING: TinyDTLS has a known issue with this test (memarray?) */
    /* 8th step: Renegotiation with existing parameters (RFC 6347 4.2.7) */
    CALL(test_rehandshake(&dtls_session, tlsman_flags, packet_rcvd));
    /* 9th step: Send/Retrieve data after the new contextfr (again) */
    CALL(test_chanel_working(&dtls_session, packet_rcvd, DTLS_MAX_BUF));
#else
    (void) test_rehandshake;;
    (void) test_chanel_working;
#endif

    /* 10th close connection */
    CALL(test_closing_channel(&dtls_session, packet_rcvd, DTLS_MAX_BUF));

    /* NOTE: This also drops any DTLS alert record */
    sock_udp_close(&udp_sock);

    puts("ALL TESTS SUCCESSFUL");

    return;
}
