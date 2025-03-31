/*
 * Copyright (C) 2016 Alexander Aring <aar@pengutronix.de>
 *                    Freie Universität Berlin
 *                    HAW Hamburg
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_SOCK_UDP_H
#define NET_SOCK_UDP_H
/**
 * @defgroup    net_sock_udp    UDP sock API
 * @ingroup     net_sock
 * @brief       Sock submodule for UDP
 *
 * How To Use
 * ----------
 * First you need to @ref including-modules "include" a module that implements
 * this API in your application's Makefile. For example the implementation for
 * @ref net_gnrc "GNRC" is called `gnrc_sock_udp`.
 *
 * ### A Simple UDP Echo Server
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "net/sock/udp.h"
 *
 * uint8_t buf[128];
 *
 * int main(void)
 * {
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_udp_t sock;
 *
 *     local.port = 12345;
 *
 *     if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
 *         puts("Error creating UDP sock");
 *         return 1;
 *     }
 *
 *     while (1) {
 *         sock_udp_ep_t remote;
 *         ssize_t res;
 *
 *         if ((res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
 *                                  &remote)) >= 0) {
 *             puts("Received a message");
 *             if (sock_udp_send(&sock, buf, res, &remote) < 0) {
 *                 puts("Error sending reply");
 *             }
 *         }
 *     }
 *
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above you see a simple UDP echo server. Don't forget to also
 * @ref including-modules "include" the IPv6 module of your networking
 * implementation (e.g. `gnrc_ipv6_default` for @ref net_gnrc GNRC) and at least
 * one network device.
 *
 * After including the header file for @ref net_sock_udp "UDP sock", we create some
 * buffer space `buf` to store the data received by the server:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/sock/udp.h"
 *
 * uint8_t buf[128];
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To be able to listen for incoming packets we bind the `sock` by setting a
 * local end point with a port (`12345` in this case).
 *
 * We then proceed to create the `sock`. It is bound to `local` and thus listens
 * for UDP packets with @ref udp_hdr_t::dst_port "destination port" `12345`.
 * Since we don't need any further configuration we set the flags to 0.
 * In case of an error we stop the program:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_udp_t sock;
 *
 *     local.port = 12345;
 *
 *     if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
 *         puts("Error creating UDP sock");
 *         return 1;
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The application then waits indefinitely for an incoming message in `buf`
 * from `remote`. If we want to timeout this wait period we could alternatively
 * set the `timeout` parameter of @ref sock_udp_recv() to a value != @ref
 * SOCK_NO_TIMEOUT. If an error occurs on receive we just ignore it and
 * continue looping.
 *
 * If we receive a message we use its `remote` to reply. In case of an error on
 * send we print an according message:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     while (1) {
 *         sock_udp_ep_t remote;
 *         ssize_t res;
 *
 *         if ((res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
 *                                  &remote)) >= 0) {
 *             puts("Received a message");
 *             if (sock_udp_send(&sock, buf, res, &remote) < 0) {
 *                 puts("Error sending reply");
 *             }
 *         }
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ### A Simple UDP Echo Client
 * There are two kinds of clients. Those that do expect a reply and those who
 * don't. A client that does not require a reply is very simple to implement in
 * one line:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * res = sock_udp_send(NULL, data, data_len, &remote);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * With `data` being the data sent, `data_len` the length of `data` and `remote`
 * the remote end point the packet that is is to be sent.
 *
 * To see some other capabilities we look at a more complex example in form of
 * the counter of the echo server above:
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "net/af.h"
 * #include "net/protnum.h"
 * #include "net/ipv6/addr.h"
 * #include "net/sock/udp.h"
 * #include "xtimer.h"
 *
 * uint8_t buf[7];
 *
 * int main(void)
 * {
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_udp_t sock;
 *
 *     local.port = 0xabcd;
 *
 *     if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
 *         puts("Error creating UDP sock");
 *         return 1;
 *     }
 *
 *
 *     while (1) {
 *         sock_udp_ep_t remote = { .family = AF_INET6 };
 *         ssize_t res;
 *
 *         remote.port = 12345;
 *         ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
 *                                           IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
 *         if (sock_udp_send(&sock, "Hello!", sizeof("Hello!"), &remote) < 0) {
 *             puts("Error sending message");
 *             sock_udp_close(&sock);
 *             return 1;
 *         }
 *         if ((res = sock_udp_recv(&sock, buf, sizeof(buf), 1 * US_PER_SEC,
 *                                 NULL)) < 0) {
 *             if (res == -ETIMEDOUT) {
 *                 puts("Timed out");
 *             }
 *             else {
 *                 puts("Error receiving message");
 *             }
 *         }
 *         else {
 *             printf("Received message: \"");
 *             for (int i = 0; i < res; i++) {
 *                 printf("%c", buf[i]);
 *             }
 *             printf("\"\n");
 *         }
 *         xtimer_sleep(1);
 *     }
 *
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Again: Don't forget to also @ref including-modules "include" the IPv6 module
 * of your networking implementation (e.g. `gnrc_ipv6_default` for
 * @ref net_gnrc "GNRC") and at least one network device.
 *
 * We first create again a `sock` with a local end point bound to any IPv6
 * address and some port. Note that we also could specify the remote here and
 * not use it with @ref sock_udp_send().
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_udp_t sock;
 *
 *     local.port = 0xabcd;
 *
 *     if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
 *         puts("Error creating UDP sock");
 *         return 1;
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We then create a remote end point with the [link-local all nodes multicast
 * address](https://tools.ietf.org/html/rfc4291#page-16) (`ff02::1`) and port
 * `12345` and send a "Hello!" message to that end point.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *         sock_udp_ep_t remote = { .family = AF_INET6 };
 *         ssize_t res;
 *
 *         remote.port = 12345;
 *         ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
 *                                           IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
 *         if (sock_udp_send(&sock, "Hello!", sizeof("Hello!"), &remote) < 0) {
 *             puts("Error sending message");
 *             sock_udp_close(&sock);
 *             return 1;
 *         }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We then wait a second for a reply and print it when it is received.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *         if ((res = sock_udp_recv(&sock, buf, sizeof(buf), 1 * US_PER_SEC,
 *                                 NULL)) < 0) {
 *             if (res == -ETIMEDOUT) {
 *                 puts("Timed out");
 *             }
 *             else {
 *                 puts("Error receiving message");
 *             }
 *         }
 *         else {
 *             printf("Received message: \"");
 *             for (int i = 0; i < res; i++) {
 *                 printf("%c", buf[i]);
 *             }
 *             printf("\"\n");
 *         }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Finally, we wait a second before sending out the next "Hello!" with
 * `xtimer_sleep(1)`.
 *
 * @{
 *
 * @file
 * @brief   UDP sock definitions
 *
 * @author  Alexander Aring <aar@pengutronix.de>
 * @author  Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

/* net/sock/async/types.h included by net/sock.h needs to re-typedef the
 * `sock_ip_t` to prevent cyclic includes */
#if defined (__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif

#include "net/af.h"
#include "net/sock.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sock_tl_ep sock_udp_ep_t;   /**< An end point for a UDP sock object */

/**
 * @brief   Type for a UDP sock object
 *
 * @note    API implementers: `struct sock_udp` needs to be defined by
 *         implementation-specific `sock_types.h`.
 */
typedef struct sock_udp sock_udp_t;

#if defined (__clang__)
# pragma clang diagnostic pop
#endif

/**
 * @brief   Auxiliary data provided when receiving using an UDP sock object
 */
typedef struct {
#if defined(MODULE_SOCK_AUX_LOCAL) || defined(DOXYGEN)
    /**
     * @brief   The local endpoint the datagram was received on
     *
     * @see SOCK_AUX_GET_LOCAL
     */
    sock_udp_ep_t local;
#endif /* MODULE_SOCK_AUX_ENDPOINT */
#if defined(MODULE_SOCK_AUX_TIMESTAMP) || defined(DOXYGEN)
    /**
     * @brief   System time the datagram was received
     *
     * @see SOCK_AUX_GET_TIMESTAMP
     */
    uint64_t timestamp;
#endif /* MODULE_SOCK_AUX_TIMESTAP */
#if defined(MODULE_SOCK_AUX_RSSI) || defined(DOXYGEN)
    /**
     * @brief   RSSI value of the received frame
     *
     * @see SOCK_AUX_GET_RSSI
     */
    int16_t rssi;
#endif /* MODULE_SOCK_AUX_RSSI */
#if defined(MODULE_SOCK_AUX_TTL) || defined(DOXYGEN)
    /**
     * @brief   TTL value of the received frame
     *
     * @see SOCK_AUX_GET_TTL
     */
    uint8_t ttl;
#endif /* MODULE_SOCK_AUX_TTL */
    sock_aux_flags_t flags; /**< Flags used request information */
} sock_udp_aux_rx_t;

/**
 * @brief   Auxiliary data provided when sending using an UDP sock object
 */
typedef struct {
#if defined(MODULE_SOCK_AUX_LOCAL) || defined(DOXYGEN)
    /**
     * @brief   The local endpoint from which the datagram will be sent
     *
     * @see SOCK_AUX_SET_LOCAL
     */
    sock_udp_ep_t local;
#endif /* MODULE_SOCK_AUX_ENDPOINT */
#if defined(MODULE_SOCK_AUX_TIMESTAMP) || defined(DOXYGEN)
    /**
     * @brief   System time the datagram was send
     *
     * Add @ref SOCK_AUX_GET_TIMESTAMP to the bitmask in
     * @ref sock_udp_aux_tx_t::flags to request a transmission timestamp. This
     * bit will be cleared by @ref sock_udp_send_aux if and only if the
     * timestamp was provided. The module `sock_aux_timestamp` needs to be
     * selected to use this. The timestamp refers to the transmission of start
     * of frame delimiter or preamble of the frame carrying the datagram and is
     * given in nanoseconds since epoch, unless otherwise documented by the
     * underlying implementation.
     */
    uint64_t timestamp;
#endif /* MODULE_SOCK_AUX_TIMESTAP*/
    sock_aux_flags_t flags; /**< Flags used request information */
} sock_udp_aux_tx_t;

/**
 * @brief   Creates a new UDP sock object
 *
 * @pre `(sock != NULL)`
 * @pre `(remote == NULL) || (remote->port != 0)`
 *
 * @warning If you create a socket you are responsible for receiving messages
 *          sent to it by calling @ref sock_udp_recv.
 *          Otherwise, the packet queue of the @p sock may congest until the
 *          socket is closed.
 *          If you only want to send without receiving, use @ref sock_udp_send
 *          instead with `sock` set to NULL.
 *
 * @param[out] sock     The resulting sock object.
 * @param[in] local     Local end point for the sock object.
 *                      May be NULL.
 *                      sock_udp_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to
 *                      sock_udp_ep_t::netif of @p remote if `remote != NULL`.
 *                      If NULL @ref sock_udp_send() may bind implicitly.
 *                      sock_udp_ep_t::port may also be 0 to bind the `sock` to
 *                      an ephemeral port.
 * @param[in] remote    Remote end point for the sock object.
 *                      May be `NULL` but then the `remote` parameter of
 *                      @ref sock_udp_send() may not be `NULL` or it will
 *                      always error with return value -ENOTCONN.
 *                      sock_udp_ep_t::port must not be 0 if `remote != NULL`.
 *                      sock_udp_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to sock_udp_ep_t::netif
 *                      of @p local if `local != NULL`.
 * @param[in] flags     Flags for the sock object. See also
 *                      [sock flags](@ref net_sock_flags).
 *                      May be 0.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `local != NULL` and @p local is already used
 *          elsewhere or if `local->port == 0` but the pool of ephemeral ports
 *          is depleted
 * @return  -EAFNOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          sock_udp_ep_t::family of @p local or @p remote is not supported.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::netif of @p local or @p remote are not a
 *          valid interfaces or contradict each other (i.e.
 *          `(local->netif != remote->netif) &&
 *          ((local->netif != SOCK_ADDR_ANY_NETIF) ||
 *          (remote->netif != SOCK_ADDR_ANY_NETIF))` if neither is `NULL`).
 * @return  -ENOMEM, if not enough resources can be provided for `sock` to be
 *          created.
 */
int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags);

/**
 * @brief   Closes a UDP sock object
 *
 * @pre `(sock != NULL)`
 *
 * @param[in] sock  A UDP sock object.
 */
void sock_udp_close(sock_udp_t *sock);

/**
 * @brief   Gets the local end point of a UDP sock object
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock  A UDP sock object.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p sock has no local end point.
 */
int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *ep);

/**
 * @brief   Gets the remote end point of a UDP sock object
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock  A UDP sock object.
 * @param[out] ep   The remote end point.
 *
 * @return  0 on success.
 * @return  -ENOTCONN, when @p sock has no remote end point bound to it.
 */
int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *ep);

/**
 * @brief   Receives a UDP message from a remote end point
 *
 * @pre `(sock != NULL) && (data != NULL) && (max_len > 0)`
 *
 * @param[in] sock      A UDP sock object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 * @param[in] timeout   Timeout for receive in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be @ref SOCK_NO_TIMEOUT for no timeout (wait until
 *                      data is available).
 * @param[out] remote   Remote end point of the received data.
 *                      May be `NULL`, if it is not required by the application.
 * @param[out] aux      Auxiliary data about the received datagram.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  -EADDRNOTAVAIL, if local of @p sock is not given.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_udp_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_udp_recv_aux(sock_udp_t *sock, void *data, size_t max_len,
                          uint32_t timeout, sock_udp_ep_t *remote,
                          sock_udp_aux_rx_t *aux);

/**
 * @brief   Receives a UDP message from a remote end point
 *
 * @pre `(sock != NULL) && (data != NULL) && (max_len > 0)`
 *
 * @param[in] sock      A UDP sock object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 * @param[in] timeout   Timeout for receive in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be @ref SOCK_NO_TIMEOUT for no timeout (wait until
 *                      data is available).
 * @param[out] remote   Remote end point of the received data.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  -EADDRNOTAVAIL, if local of @p sock is not given.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_udp_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
static inline ssize_t sock_udp_recv(sock_udp_t *sock,
                                    void *data, size_t max_len,
                                    uint32_t timeout, sock_udp_ep_t *remote)
{
    return sock_udp_recv_aux(sock, data, max_len, timeout, remote, NULL);
}

/**
 * @brief   Provides stack-internal buffer space containing a UDP message from
 *          a remote end point
 *
 * @pre `(sock != NULL) && (data != NULL) && (buf_ctx != NULL)`
 *
 * @param[in] sock      A UDP sock object.
 * @param[out] data     Pointer to a stack-internal buffer space containing the
 *                      received data.
 * @param[in,out] buf_ctx  Stack-internal buffer context. If it points to a
 *                      `NULL` pointer, the stack returns a new buffer space
 *                      for a new packet. If it does not point to a `NULL`
 *                      pointer, an existing context is assumed to get a next
 *                      segment in a buffer.
 * @param[in] timeout   Timeout for receive in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be @ref SOCK_NO_TIMEOUT for no timeout (wait until
 *                      data is available).
 * @param[out] remote   Remote end point of the received data.
 *                      May be `NULL`, if it is not required by the application.
 * @param[out] aux      Auxiliary data about the received datagram.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @experimental    This function is quite new, not implemented for all stacks
 *                  yet, and may be subject to sudden API changes. Do not use in
 *                  production if this is unacceptable.
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success. May not be the complete
 *          payload. Continue calling with the returned `buf_ctx` to get more
 *          buffers until result is 0 or an error.
 * @return  0, if no received data is available, but everything is in order.
 *          If @p buf_ctx was provided, it was released.
 * @return  -EADDRNOTAVAIL, if local of @p sock is not given.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_udp_recv() blocks).
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_udp_recv_buf_aux(sock_udp_t *sock, void **data, void **buf_ctx,
                              uint32_t timeout, sock_udp_ep_t *remote,
                              sock_udp_aux_rx_t *aux);

/**
 * @brief   Provides stack-internal buffer space containing a UDP message from
 *          a remote end point
 *
 * @pre `(sock != NULL) && (data != NULL) && (buf_ctx != NULL)`
 *
 * @param[in] sock      A UDP sock object.
 * @param[out] data     Pointer to a stack-internal buffer space containing the
 *                      received data.
 * @param[in,out] buf_ctx  Stack-internal buffer context. If it points to a
 *                      `NULL` pointer, the stack returns a new buffer space
 *                      for a new packet. If it does not point to a `NULL`
 *                      pointer, an existing context is assumed to get a next
 *                      segment in a buffer.
 * @param[in] timeout   Timeout for receive in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be @ref SOCK_NO_TIMEOUT for no timeout (wait until
 *                      data is available).
 * @param[out] remote   Remote end point of the received data.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @experimental    This function is quite new, not implemented for all stacks
 *                  yet, and may be subject to sudden API changes. Do not use in
 *                  production if this is unacceptable.
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success. May not be the complete
 *          payload. Continue calling with the returned `buf_ctx` to get more
 *          buffers until result is 0 or an error.
 * @return  0, if no received data is available, but everything is in order.
 *          If @p buf_ctx was provided, it was released.
 * @return  -EADDRNOTAVAIL, if local of @p sock is not given.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_udp_recv() blocks).
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
static inline ssize_t sock_udp_recv_buf(sock_udp_t *sock,
                                        void **data, void **buf_ctx,
                                        uint32_t timeout,
                                        sock_udp_ep_t *remote)
{
    return sock_udp_recv_buf_aux(sock, data, buf_ctx, timeout, remote, NULL);
}

/**
 * @brief   Sends a UDP message to remote end point with non-continuous payload
 *
 * @pre `((sock != NULL || remote != NULL))`
 *
 * @param[in] sock      A UDP sock object. May be `NULL`.
 *                      A sensible local end point should be selected by the
 *                      implementation in that case.
 * @param[in] snips     List of payload chunks, will be processed in order.
 *                      May be `NULL`.
 * @param[in] remote    Remote end point for the sent data.
 *                      May be `NULL`, if @p sock has a remote end point.
 *                      sock_udp_ep_t::family may be AF_UNSPEC, if local
 *                      end point of @p sock provides this information.
 *                      sock_udp_ep_t::port may not be 0.
 * @param[out] aux      Auxiliary data about the transmission.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @return  The number of bytes sent on success.
 * @return  -EADDRINUSE, if `sock` has no local end-point or was `NULL` and the
 *          pool of available ephemeral ports is depleted.
 * @return  -EAFNOSUPPORT, if `remote != NULL` and sock_udp_ep_t::family of
 *          @p remote is != AF_UNSPEC and not supported.
 * @return  -EHOSTUNREACH, if @p remote or remote end point of @p sock is not
 *          reachable.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::netif of @p remote is not a valid
 *          interface or contradicts the given local interface (i.e.
 *          neither the local end point of `sock` nor remote are assigned to
 *          `SOCK_ADDR_ANY_NETIF` but are nevertheless different.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if `remote == NULL`, but @p sock has no remote end point.
 */
ssize_t sock_udp_sendv_aux(sock_udp_t *sock, const iolist_t *snips,
                           const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux);

/**
 * @brief   Sends a UDP message to remote end point
 *
 * @pre `((sock != NULL || remote != NULL)) && (if (len != 0): (data != NULL))`
 *
 * @param[in] sock      A UDP sock object. May be `NULL`.
 *                      A sensible local end point should be selected by the
 *                      implementation in that case.
 * @param[in] data      Pointer where the received data should be stored.
 *                      May be `NULL` if `len == 0`.
 * @param[in] len       Maximum space available at @p data.
 * @param[in] remote    Remote end point for the sent data.
 *                      May be `NULL`, if @p sock has a remote end point.
 *                      sock_udp_ep_t::family may be AF_UNSPEC, if local
 *                      end point of @p sock provides this information.
 *                      sock_udp_ep_t::port may not be 0.
 * @param[out] aux      Auxiliary data about the transmission.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @return  The number of bytes sent on success.
 * @return  -EADDRINUSE, if `sock` has no local end-point or was `NULL` and the
 *          pool of available ephemeral ports is depleted.
 * @return  -EAFNOSUPPORT, if `remote != NULL` and sock_udp_ep_t::family of
 *          @p remote is != AF_UNSPEC and not supported.
 * @return  -EHOSTUNREACH, if @p remote or remote end point of @p sock is not
 *          reachable.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::netif of @p remote is not a valid
 *          interface or contradicts the given local interface (i.e.
 *          neither the local end point of `sock` nor remote are assigned to
 *          `SOCK_ADDR_ANY_NETIF` but are nevertheless different.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if `remote == NULL`, but @p sock has no remote end point.
 */
static inline ssize_t sock_udp_send_aux(sock_udp_t *sock,
                                        const void *data, size_t len,
                                        const sock_udp_ep_t *remote,
                                        sock_udp_aux_tx_t *aux)
{
    const iolist_t snip = {
        NULL,
        (void *)data,
        len,
    };

    return sock_udp_sendv_aux(sock, &snip, remote, aux);
}

/**
 * @brief   Sends a UDP message to remote end point
 *
 * @pre `((sock != NULL || remote != NULL)) && (if (len != 0): (data != NULL))`
 *
 * @param[in] sock      A UDP sock object. May be `NULL`.
 *                      A sensible local end point should be selected by the
 *                      implementation in that case.
 * @param[in] data      Pointer where the received data should be stored.
 *                      May be `NULL` if `len == 0`.
 * @param[in] len       Maximum space available at @p data.
 * @param[in] remote    Remote end point for the sent data.
 *                      May be `NULL`, if @p sock has a remote end point.
 *                      sock_udp_ep_t::family may be AF_UNSPEC, if local
 *                      end point of @p sock provides this information.
 *                      sock_udp_ep_t::port may not be 0.
 *
 * @return  The number of bytes sent on success.
 * @return  -EADDRINUSE, if `sock` has no local end-point or was `NULL` and the
 *          pool of available ephemeral ports is depleted.
 * @return  -EAFNOSUPPORT, if `remote != NULL` and sock_udp_ep_t::family of
 *          @p remote is != AF_UNSPEC and not supported.
 * @return  -EHOSTUNREACH, if @p remote or remote end point of @p sock is not
 *          reachable.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::netif of @p remote is not a valid
 *          interface or contradicts the given local interface (i.e.
 *          neither the local end point of `sock` nor remote are assigned to
 *          `SOCK_ADDR_ANY_NETIF` but are nevertheless different.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if `remote == NULL`, but @p sock has no remote end point.
 */
static inline ssize_t sock_udp_send(sock_udp_t *sock,
                                    const void *data, size_t len,
                                    const sock_udp_ep_t *remote)
{
    return sock_udp_send_aux(sock, data, len, remote, NULL);
}

/**
 * @brief   Sends a UDP message to remote end point with non-continuous payload
 *
 * @pre `((sock != NULL || remote != NULL))`
 *
 * @param[in] sock      A UDP sock object. May be `NULL`.
 *                      A sensible local end point should be selected by the
 *                      implementation in that case.
 * @param[in] snips     List of payload chunks, will be processed in order.
 *                      May be `NULL`.
 * @param[in] remote    Remote end point for the sent data.
 *                      May be `NULL`, if @p sock has a remote end point.
 *                      sock_udp_ep_t::family may be AF_UNSPEC, if local
 *                      end point of @p sock provides this information.
 *                      sock_udp_ep_t::port may not be 0.
 *
 * @return  The number of bytes sent on success.
 * @return  -EADDRINUSE, if `sock` has no local end-point or was `NULL` and the
 *          pool of available ephemeral ports is depleted.
 * @return  -EAFNOSUPPORT, if `remote != NULL` and sock_udp_ep_t::family of
 *          @p remote is != AF_UNSPEC and not supported.
 * @return  -EHOSTUNREACH, if @p remote or remote end point of @p sock is not
 *          reachable.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::netif of @p remote is not a valid
 *          interface or contradicts the given local interface (i.e.
 *          neither the local end point of `sock` nor remote are assigned to
 *          `SOCK_ADDR_ANY_NETIF` but are nevertheless different.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if `remote == NULL`, but @p sock has no remote end point.
 */
static inline ssize_t sock_udp_sendv(sock_udp_t *sock,
                                     const iolist_t *snips,
                                     const sock_udp_ep_t *remote)
{
    return sock_udp_sendv_aux(sock, snips, remote, NULL);
}

/**
 * @brief   Checks if the IP address of an endpoint is multicast
 *
 * @param[in] ep end point to check
 *
 * @returns true if end point is multicast
 */
static inline bool sock_udp_ep_is_multicast(const sock_udp_ep_t *ep)
{
    switch (ep->family) {
#ifdef SOCK_HAS_IPV6
    case AF_INET6:
        return ipv6_addr_is_multicast((const ipv6_addr_t *)&ep->addr.ipv6);
#endif
#ifdef SOCK_HAS_IPV4
    case AF_INET:
        return ipv4_addr_is_multicast((const ipv4_addr_t *)&ep->addr.ipv4);
#endif
    default:
        assert(0);
    }

    return false;
}

/**
 * @brief   Checks if the IP address of an endpoint is an IPv6 address
 *
 * @param[in] ep end point to check
 *
 * @returns true if end point is IPv6
 */
static inline bool sock_udp_ep_is_v6(const sock_udp_ep_t *ep)
{
#if !defined(SOCK_HAS_IPV6)
    (void)ep;
    return false;
#elif !defined(SOCK_HAS_IPV4)
    (void)ep;
    return true;
#else
    return ep->family == AF_INET6;
#endif
}

#include "sock_types.h"

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_SOCK_UDP_H */
