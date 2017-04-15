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
 *             sock_udp_close();
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
#ifndef NET_SOCK_UDP_H
#define NET_SOCK_UDP_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "net/sock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sock_tl_ep sock_udp_ep_t;   /**< An end point for a UDP sock object */

/**
 * @brief   Type for a UDP sock object
 *
 * @note    API implementors: `struct sock_udp` needs to be defined by
 *         implementation-specific `sock_types.h`.
 */
typedef struct sock_udp sock_udp_t;

/**
 * @brief   Creates a new UDP sock object
 *
 * @pre `(sock != NULL)`
 * @pre `(local == NULL) || (local->port != 0)`
 * @pre `(remote == NULL) || (remote->port != 0)`
 *
 * @param[out] sock     The resulting sock object.
 * @param[in] local     Local end point for the sock object.
 *                      May be NULL.
 *                      sock_udp_ep_t::port may not be 0 if `local != NULL`.
 *                      sock_udp_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to
 *                      sock_udp_ep_t::netif of @p remote if `remote != NULL`.
 *                      If NULL @ref sock_udp_send() may bind implicitly.
 * @param[in] remote    Remote end point for the sock object.
 *                      May be `NULL` but then the `remote` parameter of
 *                      @ref sock_udp_send() may not be `NULL` and or it will
 *                      always error with return value -ENOTCONN.
 *                      sock_udp_ep_t::port may not be 0 if `remote != NULL`.
 *                      sock_udp_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to sock_udp_ep_t::netif
 *                      of @p local if `local != NULL`.
 * @param[in] flags     Flags for the sock object. See also
 *                      [sock flags](net_sock_flags).
 *                      May be 0.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `local != NULL` and @p local is already used
 *         elsewhere
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
 * @param[in] sock      A raw IPv4/IPv6 sock object.
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
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_udp_recv(sock_udp_t *sock, void *data, size_t max_len,
                      uint32_t timeout, sock_udp_ep_t *remote);

/**
 * @brief   Sends a UDP message to remote end point
 *
 * @pre `((sock != NULL || remote != NULL)) && (if (len != 0): (data != NULL))`
 *
 * @param[in] sock      A raw IPv4/IPv6 sock object. May be `NULL`.
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
ssize_t sock_udp_send(sock_udp_t *sock, const void *data, size_t len,
                      const sock_udp_ep_t *remote);

#include "sock_types.h"

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_UDP_H */
/** @} */
