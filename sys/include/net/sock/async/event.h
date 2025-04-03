/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_SOCK_ASYNC_EVENT_H
#define NET_SOCK_ASYNC_EVENT_H
/**
 * @defgroup        net_sock_async_event    Asynchronous sock with event API
 * @ingroup         net_sock
 * @brief           Provides an implementation of asynchronous sock for
 *                  @ref sys_event
 * @experimental    This API is still under development and should not be used
 *                  in production yet.
 *
 * How To Use
 * ----------
 *
 * You need to [include](@ref including-modules) at least one module that
 * implements a [`sock` API](@ref net_sock) (e.g. `gnrc_sock_udp` and
 * `gnrc_sock_async` for the [GNRC](@ref net_gnrc) implementation using UDP) and
 * the module `sock_async_event` in your application's Makefile.
 *
 * For the following example [`sock_udp`](@ref net_sock_udp) is used. It is
 * however easily adaptable for other `sock` types:
 *
 * @warning An async socket may only be closed from the same thread that
 *          processes the queue. This is because there is no way to prevent the
 *          networking subsystem from posting socket events other than calling
 *          sock_*_close(), which would then race against these events. If
 *          unsure, use sock_*_event_close(), which will close the socket on
 *          the correct thread.
 *
 * ### An asynchronous UDP Echo server using the event API
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "net/sock/udp.h"
 * #include "net/sock/async/event.h"
 *
 * event_queue_t queue;
 * uint8_t buf[128];
 *
 * void handler(sock_udp_t *sock, sock_async_flags_t type, void *arg)
 * {
 *     (void)arg;
 *     if (type & SOCK_ASYNC_MSG_RECV) {
 *         sock_udp_ep_t remote;
 *         ssize_t res;
 *
 *         if ((res = sock_udp_recv(sock, buf, sizeof(buf), 0,
 *                                  &remote)) >= 0) {
 *             puts("Received a message");
 *             if (sock_udp_send(sock, buf, res, &remote) < 0) {
 *                 puts("Error sending reply");
 *             }
 *         }
 *     }
 * }
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
 *     event_queue_init(&queue);
 *     sock_udp_event_init(&sock, &queue, handler, NULL);
 *     event_loop(&queue);
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above you see a simple UDP echo server using @ref sys_event. Don't forget to
 * also @ref including-modules "include" the IPv6 module of your networking
 * implementation (e.g. `gnrc_ipv6_default` for @ref net_gnrc GNRC) and at least
 * one network device.
 *
 * After including the header file for @ref net_sock_udp "UDP sock" and
 * @ref net_sock_async_event "asynchronous handling", we create the event queue
 * `queue` and allocate some buffer space `buf` to store the data received by
 * the server:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/sock/udp.h"
 * #include "net/sock/async/event.h"
 *
 * event_queue_t queue;
 * uint8_t buf[128];
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We then define an event handler in form of a function. The event handler
 * checks if the triggering event was a receive event by checking the flags
 * provided with sock_event_t::type. If it is a receive event it copies the
 * incoming message to `buf` and its sender into `remote` using @ref
 * sock_udp_recv(). Note, that we use @ref sock_udp_recv() non-blocking by
 * setting `timeout` to 0. If an error occurs on receive, we just ignore it and
 * return from the event handler.
 *
 * If we receive a message we use its `remote` to reply. In case of an error on
 * send, we print an according message:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * void handler(sock_udp_t *sock, sock_async_flags_t type, void *arg)
 * {
 *     (void)arg;
 *     if (type & SOCK_ASYNC_MSG_RECV) {
 *         sock_udp_ep_t remote;
 *         ssize_t res;
 *
 *         if ((res = sock_udp_recv(sock, buf, sizeof(buf), 0,
 *                                  &remote)) >= 0) {
 *             puts("Received a message");
 *             if (sock_udp_send(sock, buf, res, &remote) < 0) {
 *                 puts("Error sending reply");
 *             }
 *         }
 *     }
 * }
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
 * Finally, we initialize the event queue `queue`, initialize asynchronous event
 * handling for `sock` using it and the previously defined event handler, and go
 * into an endless loop to handle all occurring events on `queue`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     event_queue_init(&queue);
 *     sock_udp_event_init(&sock, &queue, handler, NULL);
 *     event_loop(&queue);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief   Asynchronous sock using @ref sys_event definitions.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "event.h"
/* guard required since `sock_dtls_types.h` might not be provided */
#ifdef MODULE_SOCK_DTLS
#include "net/sock/dtls.h"
#endif  /* MODULE_SOCK_DTLS */
#include "net/sock/ip.h"
#include "net/sock/tcp.h"
#include "net/sock/udp.h"
#include "net/sock/async.h"

#ifdef __cplusplus
extern "C" {
#endif

/* guard required since `sock_dtls_types.h` might not be provided */
#if defined(MODULE_SOCK_DTLS) || defined(DOXYGEN)
/**
 * @brief   Makes a DTLS sock able to handle asynchronous events using
 *          @ref sys_event.
 *
 * @param[in] sock          A DTLS sock object.
 * @param[in] ev_queue      The queue the events on @p sock will be added to.
 * @param[in] handler       The event handler function to call on an event on
 *                          @p sock.
 * @param[in] handler_arg   Argument to provided to @p handler.
 *
 * @note    Only available with module `sock_dtls`.
 */
void sock_dtls_event_init(sock_dtls_t *sock, event_queue_t *ev_queue,
                          sock_dtls_cb_t handler, void *handler_arg);

/**
 * @brief Close a possibly async DTLS socket
 *
 * Helper function that closes a possibly async socket on the event thread.
 *
 * @param sock socket
 */
void sock_dtls_event_close(sock_dtls_t *sock);
#endif  /* defined(MODULE_SOCK_DTLS) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_IP) || defined(DOXYGEN)
/**
 * @brief   Makes a raw IPv4/IPv6 sock able to handle asynchronous events using
 *          @ref sys_event.
 *
 * @param[in] sock          A raw IPv4/IPv6 sock object.
 * @param[in] ev_queue      The queue the events on @p sock will be added to.
 * @param[in] handler       The event handler function to call on an event on
 *                          @p sock.
 * @param[in] handler_arg   Argument to provided to @p handler.
 *
 * @note    Only available with module `sock_ip`.
 */
void sock_ip_event_init(sock_ip_t *sock, event_queue_t *ev_queue,
                        sock_ip_cb_t handler, void *handler_arg);

/**
 * @brief Close a possibly async IP socket
 *
 * Helper function that closes a possibly async socket on the event thread.
 *
 * @param sock socket
 */
void sock_ip_event_close(sock_ip_t *sock);
#endif  /* defined(MODULE_SOCK_IP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_TCP) || defined(DOXYGEN)
/**
 * @brief   Makes a TCP sock able to handle asynchronous events using
 *          @ref sys_event.
 *
 * @param[in] sock          A TCP sock object.
 * @param[in] ev_queue      The queue the events on @p sock will be added to.
 * @param[in] handler       The event handler function to call on an event on
 *                          @p sock.
 * @param[in] handler_arg   Argument to provided to @p handler.
 *
 * @note    Only available with module `sock_tcp`.
 */
void sock_tcp_event_init(sock_tcp_t *sock, event_queue_t *ev_queue,
                         sock_tcp_cb_t handler, void *handler_arg);

/**
 * @brief Close a possibly async TCP socket
 *
 * Helper function that closes a possibly async socket on the event thread.
 *
 * @param sock socket
 */
void sock_tcp_event_close(sock_tcp_t *sock);

/**
 * @brief   Makes a TCP listening queue able to handle asynchronous events using
 *          @ref sys_event.
 *
 * @param[in] queue         A TCP listening queue.
 * @param[in] ev_queue      The queue the events on @p sock will be added to.
 * @param[in] handler       The event handler function to call on an event on
 *                          @p sock.
 * @param[in] handler_arg   Argument to provided to @p handler.
 *
 * @note    Only available with module `sock_tcp`.
 */
void sock_tcp_queue_event_init(sock_tcp_queue_t *queue, event_queue_t *ev_queue,
                               sock_tcp_queue_cb_t handler, void *handler_arg);
#endif  /* defined(MODULE_SOCK_TCP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
/**
 * @brief   Makes a UDP sock able to handle asynchronous events using
 *          @ref sys_event.
 *
 * @param[in] sock          A UDP sock object.
 * @param[in] ev_queue      The queue the events on @p sock will be added to.
 * @param[in] handler       The event handler function to call on an event on
 *                          @p sock.
 * @param[in] handler_arg   Argument to provided to @p handler.
 *
 * @note    Only available with module `sock_udp`.
 */
void sock_udp_event_init(sock_udp_t *sock, event_queue_t *ev_queue,
                         sock_udp_cb_t handler, void *handler_arg);

/**
 * @brief Close a possibly async UDP socket
 *
 * Helper function that closes a possibly async socket on the event thread.
 *
 * @param sock socket
 */
void sock_udp_event_close(sock_udp_t *sock);

#endif  /* defined(MODULE_SOCK_UDP) || defined(DOXYGEN) */

/**
 * @brief clear any pending socket async events
 *
 * @warning Do not call this in the application, it is automatically called by
 * sock_*_close().
 *
 * @param[in] async_ctx socket async context
 */
void sock_event_close(sock_async_ctx_t *async_ctx);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_SOCK_ASYNC_EVENT_H */
