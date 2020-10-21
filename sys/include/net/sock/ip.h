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
 * @defgroup    net_sock_ip     Raw IPv4/IPv6 sock API
 * @ingroup     net_sock
 * @brief       Sock submodule for raw IPv4/IPv6
 *
 * How To Use
 * ----------
 * First you need to @ref including-modules "include" a module that implements
 * this API in your application's Makefile. For example the implementation for
 * @ref net_gnrc "GNRC" is called `gnrc_sock_ip`.
 *
 * ### A Simple IPv6 Server
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "net/protnum.h"
 * #include "net/sock/ip.h"
 *
 * uint8_t buf[128];
 *
 * int main(void)
 * {
 *     sock_ip_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_ip_t sock;
 *
 *     if (sock_ip_create(&sock, &local, NULL, PROTNUM_IPV6_NONXT, 0) < 0) {
 *         puts("Error creating raw IP sock");
 *         return 1;
 *     }
 *
 *     while (1) {
 *         sock_ip_ep_t remote;
 *         ssize_t res;
 *
 *         if ((res = sock_ip_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
 *                                 &remote)) >= 0) {
 *             puts("Received a message");
 *             if (sock_ip_send(&sock, buf, res, 0, &remote) < 0) {
 *                 puts("Error sending reply");
 *             }
 *         }
 *     }
 *
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above you see a simple IPv6 server. Don't forget to also
 * @ref including-modules "include" the IPv6 module of your networking
 * implementation (e.g. `gnrc_ipv6_default` for @ref net_gnrc "GNRC") and at
 * least one network device.
 *
 * After including header files for the @ref net_af "address families",
 * @ref net_protnum "protocol numbers" and the @ref net_sock_ip "raw `sock`s"
 * themselves, we create some buffer space `buf` to store the data received by
 * the server:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/af.h"
 * #include "net/protnum.h"
 * #include "net/sock/ip.h"
 *
 * uint8_t buf[128];
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To be able to listen for incoming packets we bind the `sock` by setting a
 * local end point (even if we just state here, that we just want to bind it to
 * any IPv6 address).
 *
 * We then proceed to create the `sock`. It is bound to `local` and listens for
 * IPv6 packets with @ref ipv6_hdr_t::nh "next header field"
 * @ref PROTNUM_IPV6_NONXT. Since we don't need any further configuration we set
 * the flags to 0. In case of an error we stop the program:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     sock_ip_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_ip_t sock;
 *
 *     if (sock_ip_create(&sock, &local, NULL, PROTNUM_IPV6_NONXT, 0) < 0) {
 *         puts("Error creating raw IP sock");
 *         return 1;
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The application then waits indefinitely for an incoming message in
 * `buf` from `remote`. If we want to timeout this wait period we could
 * alternatively set the `timeout` parameter of @ref sock_ip_recv() to a
 * value `!= SOCK_NO_TIMEOUT`. If an error occurs on receive we just ignore it
 * and continue looping.
 *
 * If we receive a message we use its `remote` to reply. Note since the `proto`
 * was already set during @ref sock_ip_create() we can just leave `proto` for
 * the @ref sock_ip_send() set to 0 (it is ignored by that function in that case
 * anyway). In case of an error on send we print an according message:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     while (1) {
 *         sock_ip_ep_t remote;
 *         ssize_t res;
 *
 *         if ((res = sock_ip_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
 *                                 &remote)) >= 0) {
 *             puts("Received a message");
 *             if (sock_ip_send(&sock, buf, res, 0, &remote) < 0) {
 *                 puts("Error sending reply");
 *             }
 *         }
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ### A Simple IPv6 Client
 * There are two kinds of clients. Those that do expect a reply and those who
 * don't. A client that does not require a reply is very simple to implement in
 * one line:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * res = sock_ip_send(NULL, data, data_len, PROTNUM, &remote);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * With `data` being the data sent, `data_len` the length of `data`, `PROTNUM`
 * the next header number for the sent packet and `remote` the remote end point
 * the packet that is to be sent.
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
 * #include "net/sock/ip.h"
 * #include "xtimer.h"
 *
 * uint8_t buf[7];
 *
 * int main(void)
 * {
 *     sock_ip_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_ip_t sock;
 *
 *     if (sock_ip_create(&sock, &local, NULL, PROTNUM_IPV6_NONXT, 0) < 0) {
 *         puts("Error creating raw IP sock");
 *         return 1;
 *     }
 *
 *     while (1) {
 *         sock_ip_ep_t remote = { .family = AF_INET6 };
 *         ssize_t res;
 *
 *         ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
 *                                           IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
 *
 *         if (sock_ip_send(&sock, "Hello!", sizeof("Hello!"), 0, &remote) < 0) {
 *             puts("Error sending message");
 *             sock_ip_close(&sock);
 *             return 1;
 *         }
 *         if ((res = sock_ip_recv(&sock, buf, sizeof(buf), 1 * US_PER_SEC,
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
 * address. Note that we also could specify the remote end point here and not
 * use it with @ref sock_ip_send().
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     sock_ip_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_ip_t sock;
 *
 *     if (sock_ip_create(&sock, &local, NULL, PROTNUM_IPV6_NONXT, 0) < 0) {
 *         puts("Error creating raw IP sock");
 *         return 1;
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We then create a remote end point for the [link-local all nodes multicast
 * address](https://tools.ietf.org/html/rfc4291#page-16) (`ff02::1`) and send
 * a "Hello!" message to that end point.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *         sock_ip_ep_t remote = { .family = AF_INET6 };
 *         ssize_t res;
 *
 *         ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
 *                                           IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
 *
 *         if (sock_ip_send(&sock, "Hello!", sizeof("Hello!"), 0, &remote) < 0) {
 *             puts("Error sending message");
 *             sock_ip_close(&sock);
 *             return 1;
 *         }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We then wait a second for a reply and print it when it is received.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *         if ((res = sock_ip_recv(&sock, buf, sizeof(buf), 1 * US_PER_SEC,
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
 * @brief   Raw IPv4/IPv6 sock definitions
 *
 * @author  Alexander Aring <aar@pengutronix.de>
 * @author  Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef NET_SOCK_IP_H
#define NET_SOCK_IP_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

/* net/sock/async/types.h included by net/sock.h needs to re-typedef the
 * `sock_ip_t` to prevent cyclic includes */
#if defined (__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif

#include "net/sock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type for a raw IPv4/IPv6 sock object
 *
 * @note API implementors: `struct sock_ip` needs to be defined by
 *       implementation-specific `sock_types.h`.
 */
typedef struct sock_ip sock_ip_t;

#if defined (__clang__)
# pragma clang diagnostic pop
#endif

/**
 * @brief   Creates a new raw IPv4/IPv6 sock object
 *
 * @pre `(sock != NULL)`
 *
 * @param[out] sock     The resulting sock object.
 * @param[in] local     Local end point for the sock object.
 *                      May be NULL. sock_ip_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to sock_ip_ep_t::netif
 *                      of @p remote if `remote != NULL`.
 *                      If NULL @ref sock_ip_send() may bind implicitly.
 * @param[in] remote    Remote end point for the sock object.
 *                      May be `NULL` but then the `remote` parameter of
 *                      @ref sock_ip_send() may not be `NULL` or it will always
 *                      error with return value -ENOTCONN.
 *                      sock_ip_ep_t::port may not be 0 if `remote != NULL`.
 *                      sock_ip_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to sock_ip_ep_t::netif
 *                      of @p local if `local != NULL`.
 * @param[in] proto     Protocol to use in the raw IPv4/IPv6 sock object
 *                      (the `protocol` header field in IPv4 and the `next_header`
 *                      field in IPv6).
 * @param[in] flags     Flags for the sock object. See also
 *                      [sock flags](net_sock_flags). May be 0.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `local != NULL` and @p local is already used
 *          elsewhere
 * @return  -EAFNOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          sock_ip_ep_t::family of @p local or @p remote is not supported.
 * @return  -EINVAL, if sock_ip_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_ip_ep_t::netif of @p local or @p remote are not
 *          valid interfaces or contradict each other
 *          (i.e. `(local->netif != remote->netif) &&
 *          ((local->netif != SOCK_ADDR_ANY_NETIF) ||
 *          (remote->netif != SOCK_ADDR_ANY_NETIF))` if neither is `NULL`).
 * @return  -ENOMEM, if not enough resources can be provided for `sock` to be
 *          created.
 * @return  -EPROTONOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          proto is not supported by sock_ip_ep_t::family of @p local or @p
 *          remote.
 */
int sock_ip_create(sock_ip_t *sock, const sock_ip_ep_t *local,
                   const sock_ip_ep_t *remote, uint8_t proto, uint16_t flags);

/**
 * @brief   Closes a raw IPv4/IPv6 sock object
 *
 * @pre `(sock != NULL)`
 *
 * @param[in] sock  A raw IPv4/IPv6 sock object.
 */
void sock_ip_close(sock_ip_t *sock);

/**
 * @brief   Gets the local end point of a raw IPv4/IPv6 sock object
 *
 * This gets the local end point of a raw IPv4/IPv6 sock object. Note that this
 * might not be the same end point you added in @ref sock_ip_create(), but an
 * end point more suitable for the implementation. Examples for this might be
 * that if sock_ip_ep_t::netif is given in @ref sock_ip_create(), the
 * implementation might choose to return the address on this interface the
 * @p sock is bound to in @p ep's sock_ip_ep_t::addr.
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock  A raw IPv4/IPv6 sock object.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p sock has no end point bound to it.
 */
int sock_ip_get_local(sock_ip_t *sock, sock_ip_ep_t *ep);

/**
 * @brief   Gets the remote end point of a raw IPv4/IPv6 sock object
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * This gets the remote end point of a raw IPv4/IPv6 sock object. Note that this
 * might not be the same end point you added in @ref sock_ip_create(), but an
 * end point more suitable for the implementation. Examples for this might be
 * that if sock_ip_ep_t::netif is given in @ref sock_ip_create(), the
 * implementation might choose to return the address on this interface the
 * @p sock is bound to in @p ep's sock_ip_ep_t::addr.
 *
 * @param[in] sock  A raw IPv4/IPv6 sock object.
 * @param[out] ep   The remote end point.
 *
 * @return  0 on success.
 * @return  -ENOTCONN, when @p sock has no remote end point bound to it.
 */
int sock_ip_get_remote(sock_ip_t *sock, sock_ip_ep_t *ep);

/**
 * @brief   Receives a message over IPv4/IPv6 from remote end point
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
 *                      May be NULL, if it is not required by the application.
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  -EADDRNOTAVAIL, if local of @p sock is not given.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_ip_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_ip_recv(sock_ip_t *sock, void *data, size_t max_len,
                     uint32_t timeout, sock_ip_ep_t *remote);

/**
 * @brief   Provides stack-internal buffer space containing an IPv4/IPv6
 *          message from remote end point
 *
 * @pre `(sock != NULL) && (data != NULL) && (buf_ctx != NULL)`
 *
 * @param[in] sock      A raw IPv4/IPv6 sock object.
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
 *                      May be NULL, if it is not required by the application.
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
 *          initialized (or closed while sock_ip_recv() blocks).
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_ip_recv_buf(sock_ip_t *sock, void **data, void **buf_ctx,
                         uint32_t timeout, sock_ip_ep_t *remote);

/**
 * @brief   Sends a message over IPv4/IPv6 to remote end point
 *
 * @pre `((sock != NULL || remote != NULL)) && (if (len != 0): (data != NULL))`
 *
 * @param[in] sock      A raw IPv4/IPv6 sock object. May be NULL.
 *                      A sensible local end point should be selected by the
 *                      implementation in that case.
 * @param[in] data      Pointer where the received data should be stored.
 *                      May be `NULL` if `len == 0`.
 * @param[in] len       Maximum space available at @p data.
 * @param[in] proto     Protocol to use in the packet sent, in case
 *                      `sock == NULL`. If `sock != NULL` this parameter will be
 *                      ignored.
 * @param[in] remote    Remote end point for the sent data.
 *                      May be `NULL`, if @p sock has a remote end point.
 *                      sock_ip_ep_t::family may be AF_UNSPEC, if local
 *                      end point of @p sock provides this information.
 *
 * @return  The number of bytes sent on success.
 * @return  -EAFNOSUPPORT, if `remote != NULL` and sock_ip_ep_t::family of
 *          @p remote is != AF_UNSPEC and not supported.
 * @return  -EINVAL, if sock_ip_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_ip_ep_t::netif of @p remote is not a
 *          valid interface or contradicts the local interface of @p sock.
 * @return  -EHOSTUNREACH, if @p remote or remote end point of @p sock is not
 *          reachable.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if `remote == NULL`, but @p sock has no remote end point.
 * @return  -EPROTOTYPE, if `sock == NULL` and @p proto is not by
 *          sock_ip_ep_t::family of @p remote.
 */
ssize_t sock_ip_send(sock_ip_t *sock, const void *data, size_t len,
                     uint8_t proto, const sock_ip_ep_t *remote);

#include "sock_types.h"

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_IP_H */
/** @} */
