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

#pragma once

/**
 * @defgroup    net_sock_tcp    TCP sock API
 * @ingroup     net_sock
 * @brief       Sock submodule for TCP
 *
 * How To Use
 * ----------
 * First you need to @ref including-modules "include" a module that implements
 * this API in your application's Makefile. For example the implementation for
 * @ref net_gnrc "GNRC" is called `gnrc_sock_tcp`.
 *
 * ### A Simple TCP Echo Server
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/sock/tcp.h"
 *
 * #define SOCK_QUEUE_LEN  (1U)
 *
 * sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
 * uint8_t buf[128];
 *
 * int main(void)
 * {
 *     sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_tcp_queue_t queue;
 *
 *     local.port = 12345;
 *
 *     if (sock_tcp_listen(&queue, &local, sock_queue, SOCK_QUEUE_LEN, 0) < 0) {
 *         puts("Error creating listening queue");
 *         return 1;
 *     }
 *     puts("Listening on port 12345");
 *     while (1) {
 *         sock_tcp_t *sock;
 *
 *         if (sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT) < 0) {
 *             puts("Error accepting new sock");
 *         }
 *         else {
 *             int read_res = 0;
 *
 *             puts("Reading data");
 *             while (read_res >= 0) {
 *                 read_res = sock_tcp_read(sock, &buf, sizeof(buf),
 *                                          SOCK_NO_TIMEOUT);
 *                 if (read_res <= 0) {
 *                     puts("Disconnected");
 *                     break;
 *                 }
 *                 else {
 *                     int write_res;
 *                     printf("Read: \"");
 *                     for (int i = 0; i < read_res; i++) {
 *                         printf("%c", buf[i]);
 *                     }
 *                     puts("\"");
 *                     if ((write_res = sock_tcp_write(sock, &buf,
 *                                                     read_res)) < 0) {
 *                         puts("Errored on write, finished server loop");
 *                         break;
 *                     }
 *                 }
 *             }
 *             sock_tcp_disconnect(sock);
 *         }
 *     }
 *     sock_tcp_stop_listen(&queue);
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above you see a simple TCP echo server. Don't forget to also
 * @ref including-modules "include" the IPv6 module of your networking
 * implementation (e.g. `gnrc_ipv6_default` for @ref net_gnrc GNRC) and at least
 * one network device.
 *
 *
 * After including header files for the @ref net_af "address families" and
 * the @ref net_sock_tcp "TCP `sock`s and `queue`s" themselves, we create an
 * array of @ref sock_tcp_t "sock" objects `sock_queue` as our listen queue (for
 * simplicity of length 1 in our example) and some buffer space `buf` to store
 * the data received by the server:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/af.h"
 * #include "net/sock/tcp.h"
 *
 * #define SOCK_QUEUE_LEN  (1U)
 *
 * sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
 * uint8_t buf[128];
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We want to listen for incoming connections on a specific port, so we set a
 * local end point with that port (`12345` in this case).
 *
 * We then proceed to creating the listen queue `queue`. Since it is bound to
 * `local` it waits for incoming connections to port `12345`. We don't need any
 * further configuration so we set the flags to 0. In case of an error we stop
 * the program:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
 *     sock_tcp_queue_t queue;
 *
 *     local.port = 12345;
 *
 *     if (sock_tcp_listen(&queue, &local, sock_queue, SOCK_QUEUE_LEN, 0) < 0) {
 *         puts("Error creating listening queue");
 *         return 1;
 *     }
 *     puts("Listening on port 12345");
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The application then waits indefinitely for an incoming connection with
 * `sock_tcp_accept()`. If we want to timeout this wait period we could
 * alternatively set the `timeout` parameter of @ref sock_tcp_accept() to a
 * value != @ref SOCK_NO_TIMEOUT. If an error occurs during that we print an
 * error message but proceed waiting.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     while (1) {
 *         sock_tcp_t *sock;
 *
 *         if (sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT) < 0) {
 *             puts("Error accepting new sock");
 *         }
 *         else {
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * On successful connection establishment with a client we get a connected
 * `sock` object and we try to read the incoming stream into `buf` using
 * `sock_tcp_read()` on that `sock`. Again, we could use another timeout period
 * than @ref SOCK_NO_TIMEOUT with this function. If we error we break the read
 * loop and disconnect the `sock`.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *             int read_res = 0;
 *
 *             puts("Reading data");
 *             while (read_res >= 0) {
 *                 read_res = sock_tcp_read(sock, &buf, sizeof(buf),
 *                                          SOCK_NO_TIMEOUT);
 *                 if (read_res <= 0) {
 *                     puts("Disconnected");
 *                     break;
 *                 }
 *                 else {
 *                     ...
 *                 }
 *             }
 *             sock_tcp_disconnect(sock);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Otherwise, we print the received message and write it back to the connected
 * `sock` (an again breaking the loop on error).
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *                     int write_res;
 *                     printf("Read: \"");
 *                     for (int i = 0; i < read_res; i++) {
 *                         printf("%c", buf[i]);
 *                     }
 *                     puts("\"");
 *                     if ((write_res = sock_tcp_write(sock, &buf,
 *                                                     read_res)) < 0) {
 *                         puts("Errored on write, finished server loop");
 *                         break;
 *                     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * In the case of we somehow manage to break the infinite accepting loop we stop
 * the listening queue appropriately.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     sock_tcp_stop_listen(queue);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ### A Simple TCP Echo Client
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/af.h"
 * #include "net/ipv6/addr.h"
 * #include "net/sock/tcp.h"
 *
 * uint8_t buf[128];
 * sock_tcp_t sock;
 *
 * int main(void)
 * {
 *     int res;
 *     sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;
 *
 *     remote.port = 12345;
 *     ipv6_addr_from_str((ipv6_addr_t *)&remote.addr,
 *                        "fe80::d8fa:55ff:fedf:4523");
 *     if (sock_tcp_connect(&sock, &remote, 0, 0) < 0) {
 *         puts("Error connecting sock");
 *         return 1;
 *     }
 *     puts("Sending \"Hello!\"");
 *     if ((res = sock_tcp_write(&sock, "Hello!", sizeof("Hello!"))) < 0) {
 *         puts("Errored on write");
 *     }
 *     else {
 *         if ((res = sock_tcp_read(&sock, &buf, sizeof(buf),
 *                                  SOCK_NO_TIMEOUT)) <= 0) {
 *             puts("Disconnected");
 *         }
 *         printf("Read: \"");
 *         for (int i = 0; i < res; i++) {
 *             printf("%c", buf[i]);
 *         }
 *         puts("\"");
 *     }
 *     sock_tcp_disconnect(&sock);
 *     return res;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above you see a simple TCP echo client. Again: Don't forget to also
 * @ref including-modules "include" the IPv6 module of your networking
 * implementation (e.g. `gnrc_ipv6_default` for @ref net_gnrc "GNRC") and at
 * least one network device. Ad0)ditionally, for the IPv6 address parsing you need
 * the @ref net_ipv6_addr "IPv6 address module".
 *
 * This time instead of creating a listening queue we create a connected `sock`
 * object directly. To connect it to a port at a host we setup a remote
 * end-point first (with port `12345` and address `fe80::d8fa:55ff:fedf:4523` in
 * this case; your IP address may differ of course) and connect to it using
 * `sock_tcp_connect()`. We neither care about the local port nor additional
 * configuration so we set both the `local_port` and `flags` parameter of
 * `sock_tcp_connect()` to `0`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;
 *
 *     remote.port = 12345;
 *     ipv6_addr_from_str((ipv6_addr_t *)&remote.addr,
 *                        "fe80::d8fa:55ff:fedf:4523");
 *     if (sock_tcp_connect(&sock, &remote, 0, 0) < 0) {
 *         puts("Error connecting sock");
 *         return 1;
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * On error we just terminate the program, on success we send a message
 * (`Hello!`) and again terminate the program on error:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     if ((res = sock_tcp_write(&sock, "Hello!", sizeof("Hello!"))) < 0) {
 *         puts("Errored on write");
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Otherwise, we wait for the reply and print it in case of success (and
 * terminate in case of error):
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     else {
 *         if ((res = sock_tcp_read(&sock, &buf, sizeof(buf),
 *                                  SOCK_NO_TIMEOUT)) <= 0) {
 *             puts("Disconnected");
 *         }
 *         printf("Read: \"");
 *         for (int i = 0; i < res; i++) {
 *             printf("%c", buf[i]);
 *         }
 *         puts("\"");
 *     }
 *     sock_tcp_disconnect(&sock);
 *     return res;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief   TCP sock definitions
 *
 * @author  Alexander Aring <aar@pengutronix.de>
 * @author  Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

/* net/sock/async/types.h included by net/sock.h needs to re-typedef the
 * `sock_tcp_t` and `sock_tcp_queue_t` to prevent cyclic includes */
#if defined (__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif

#include "net/sock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sock_tl_ep sock_tcp_ep_t;   /**< An end point for a TCP sock object */

/**
 * @brief   Type for a TCP sock object
 *
 * @note API implementers: `struct sock_tcp` needs to be defined by
 *       implementation-specific `sock_types.h`.
 */
typedef struct sock_tcp sock_tcp_t;

/**
 * @brief   Type for a TCP listening queue
 *
 * @note API implementers: `struct sock_tcp_queue` needs to be defined by
 *                          implementation-specific `sock_types.h`.
 */
typedef struct sock_tcp_queue sock_tcp_queue_t;

#if defined (__clang__)
# pragma clang diagnostic pop
#endif

/**
 * @brief   Establishes a new TCP sock connection
 *
 * @pre `sock != NULL`
 * @pre `(remote != NULL) && (remote->port != 0)`
 *
 * @param[out] sock         The resulting sock object.
 * @param[in] remote        Remote end point for the sock object.
 * @param[in] local_port    Local port for the connection. May be 0. *
 *                          If `local_port == 0` the connection is bound to a
 *                          random port.
 * @param[in] flags         Flags for the sock object. See also
 *                          @ref net_sock_flags. May be 0.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `(flags & SOCK_FLAGS_REUSE_EP) == 0` and
 *          @p local_port is already used elsewhere
 * @return  -EAFNOSUPPORT, if sock_tcp_ep_t::family of @p remote is not
 *          supported.
 * @return  -ECONNREFUSED, if no-one is listening on the @p remote end point.
 * @return  -EINVAL, if sock_tcp_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_tcp_ep_t::netif of @p remote is not a valid
 *          interface.
 * @return  -ENETUNREACH, if network defined by @p remote is not reachable.
 * @return  -ENOMEM, if system was not able to allocate sufficient memory to
 *          establish connection.
 * @return  -EPERM, if connections to @p remote are not permitted on the system
 *          (e.g. by firewall rules).
 * @return  -ETIMEDOUT, if the connection attempt to @p remote timed out.
 */
int sock_tcp_connect(sock_tcp_t *sock, const sock_tcp_ep_t *remote,
                     uint16_t local_port, uint16_t flags);

/**
 * @brief   Listen for an incoming connection request on @p local end point
 *
 * @pre `queue != NULL`
 * @pre `(local != NULL) && (local->port != 0)`
 * @pre `(queue_array != NULL) && (queue_len != 0)`
 *
 * @param[in] queue         The resulting listening queue.
 * @param[in] local         Local end point to listen on.
 * @param[in] queue_array   Array of sock objects.
 * @param[in] queue_len     Length of @p queue_array.
 * @param[in] flags         Flags for the listening queue. See also
 *                          @ref net_sock_flags. May be 0.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `(flags & SOCK_FLAGS_REUSE_EP) == 0` and
 *          @p local is already used elsewhere
 * @return  -EAFNOSUPPORT, if sock_tcp_ep_t::family of @p local is not
 *          supported.
 * @return  -EINVAL, if sock_tcp_ep_t::netif of @p local is not a valid
 *          interface.
 * @return  -ENOMEM, if no memory was available to listen on @p queue.
 */
int sock_tcp_listen(sock_tcp_queue_t *queue, const sock_tcp_ep_t *local,
                    sock_tcp_t *queue_array, unsigned queue_len,
                    uint16_t flags);

/**
 * @brief   Disconnects a TCP connection
 *
 * @pre `sock != NULL` If we want to timeout this wait period we could
 * alternatively set the `timeout` parameter of @ref sock_tcp_accept() to a
 * value != @ref SOCK_NO_TIMEOUT.
 *
 * @param[in] sock  A TCP sock object.
 */
void sock_tcp_disconnect(sock_tcp_t *sock);

/**
 * @brief   Stops listening on TCP listening queue
 *
 * @pre `queue != NULL`
 *
 * @param[in] queue A TCP listening queue.
 */
void sock_tcp_stop_listen(sock_tcp_queue_t *queue);

/**
 * @brief   Gets the local end point of a TCP sock object
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock  A TCP sock object.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p sock has no local end point.
 */
int sock_tcp_get_local(sock_tcp_t *sock, sock_tcp_ep_t *ep);

/**
 * @brief   Gets the remote end point of a TCP sock object
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock  A TCP sock object.
 * @param[out] ep   The remote end point.
 *
 * @return  0 on success.
 * @return  -ENOTCONN, when @p sock is not connected to a remote end point.
 */
int sock_tcp_get_remote(sock_tcp_t *sock, sock_tcp_ep_t *ep);

/**
 * @brief   Gets the local end point of a TCP sock queue object
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] queue A TCP sock queue object.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p queue has no local end point.
 */
int sock_tcp_queue_get_local(sock_tcp_queue_t *queue, sock_tcp_ep_t *ep);

/**
 * @brief   Receives and handles TCP connection requests from other peers
 *
 * @pre `(queue != NULL) && (sock != NULL)`
 *
 * @param[in] queue     A TCP listening queue.
 * @param[out] sock     A new TCP sock object for the established sock object.
 * @param[in] timeout   Timeout for accept in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be @ref SOCK_NO_TIMEOUT for no timeout (wait until
 *                      data is available).
 *
 * @return  0 on success.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -ECONNABORTED, if the connection to @p sock has been aborted while
 *          in this function
 * @return  -EINVAL, if @p queue was not initialized using
 *          @ref sock_tcp_listen().
 * @return  -ENOMEM, if system was not able to allocate sufficient memory to
 *          establish connection.
 * @return  -EPERM, if connections on local end point of @p queue are not
 *          permitted on this system (e.g. by firewall rules).
 * @return  -ETIMEDOUT, if the operation timed out internally.
 */
int sock_tcp_accept(sock_tcp_queue_t *queue, sock_tcp_t **sock,
                    uint32_t timeout);

/**
 * @brief   Reads data from an established TCP stream
 *
 * @pre `(sock != NULL) && (data != NULL) && (max_len > 0)`
 *
 * @param[in] sock      A TCP sock object.
 * @param[out] data     Pointer where the read data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 *                      If read data exceeds @p max_len the data is
 *                      truncated and the remaining data can be retrieved
 *                      later on.
 * @param[in] timeout   Timeout for receive in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be @ref SOCK_NO_TIMEOUT for no timeout (wait until
 *                      data is available).
 *
 * @note    Function may block.
 *
 * @return  The number of bytes read on success.
 * @return  0, if no read data is available or the connection was orderly closed
 *          by the remote host.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -ECONNABORTED, if the connection is aborted while waiting for the
 *          next data.
 * @return  -ECONNRESET, if the connection was forcibly closed by remote end
 *          point of @p sock.
 * @return  -ENOTCONN, when @p sock is not connected to a remote end point.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_tcp_read(sock_tcp_t *sock, void *data, size_t max_len,
                      uint32_t timeout);

/**
 * @brief   Writes data to an established TCP stream
 *
 * @pre `(sock != NULL)`
 * @pre `if (len != NULL): (data != NULL)`
 *
 * @param[in] sock  A TCP sock object.
 * @param[in] data  Pointer to the data to be written to the stream.
 * @param[in] len   Maximum space available at @p data.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes written on success.
 * @return  -ECONNABORTED, if the connection is aborted while waiting for the
 *          next data.
 * @return  -ECONNRESET, if the connection was forcibly closed by remote end
 *          point of @p sock.
 * @return  -ENOMEM, if no memory was available to written @p data.
 * @return  -ENOTCONN, if @p sock is not connected to a remote end point.
 */
ssize_t sock_tcp_write(sock_tcp_t *sock, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#include "sock_types.h"

/** @} */
