/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_async_msg  Asynchronous sock with messaging / IPC
 * @ingroup     net_sock
 * @brief       Provides an implementation of asynchronous sock for
 *              @ref core_msg
 *
 * How To Use
 * ----------
 *
 * You need to [include](@ref including-modules) at least one module that
 * implements a [`sock` API](@ref net_sock) and the module `sock_async_event` in
 * your application's Makefile.
 *
 * For the following example [`sock_udp`](@ref net_sock_udp) is used. It is
 * however easily adaptable for other `sock` types:
 *
 * ### An asynchronous UDP Echo server using the event API
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "msg.h"
 * #include "thread.h"
 * #include "net/sock/udp.h"
 * #include "net/sock/async/msg.h"
 *
 * msg_t queue[8];
 * uint8_t buf[128];
 *
 * int main(void)
 * {
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *
 *     local.port = 12345;
 *
 *     if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
 *         puts("Error creating UDP sock");
 *         return 1;
 *     }
 *
 *     msg_init_queue(&queue, 8);
 *     sock_udp_msg_init(&sock, thread_getpid());
 *     while (1) {
 *         msg_t msg;
 *
 *         msg_receive(&msg);
 *         if (sock_async_msg_type(&msg) && (msg.type & SOCK_EVENT_MSG_RECV)) {
 *             sock_udp_t *s = msg.content.ptr;
 *             sock_udp_ep_t remote;
 *             ssize_t res;
 *
 *             if ((res = sock_udp_recv(s, buf, sizeof(buf), 0,
 *                                      &remote)) >= 0) {
 *                 puts("Received a message");
 *                 if (sock_udp_send(s, buf, res, &remote) < 0) {
 *                     puts("Error sending reply");
 *                 }
 *             }
 *         }
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above you see a simple UDP echo server. Don't forget to also
 * @ref including-modules "include" the IPv6 module of your networking
 * implementation (e.g. `gnrc_ipv6_default` for @ref net_gnrc GNRC) and at least
 * one network device.
 *
 * After including the header file for @ref net_sock_udp "UDP sock" and
 * @ref net_sock_async_msg "asynchronous handling", we create a message queue
 * `queue` (note: [must be of a size equal to a power of two]
  * (@ref msg_init_queue)) and allocate some buffer space `buf` to store the
 * data received by the server:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/sock/udp.h"
 * #include "net/sock/async/msg.h"
 *
 * msg_t queue[8];
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
 * The application then blocks to wait for an IPC message and check if the event
 * triggering the IPC message was a receive event by checking the flags provided
 * with msg_t::type.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *         msg_t msg;
 *
 *         msg_receive(&msg);
 *         if (sock_async_msg_type(&msg) && (msg.type & SOCK_EVENT_MSG_RECV)) {
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * If it is a receive event it copies the incoming message to `buf` and its
 * sender into `remote` using @ref sock_udp_recv(). Note, that we use @ref
 * sock_udp_recv() non-blocking by setting `timeout` to 0. If an error occurs on
 * receive, we just ignore it and continue looping.
 *
 * If we receive a message we use its `remote` to reply. In case of an error on
 * send we print an according message:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *             sock_udp_t *s = msg.content.ptr;
 *             sock_udp_ep_t remote;
 *             ssize_t res;
 *
 *             if ((res = sock_udp_recv(s, buf, sizeof(buf), 0,
 *                                      &remote)) >= 0) {
 *                 puts("Received a message");
 *                 if (sock_udp_send(s, buf, res, &remote) < 0) {
 *                     puts("Error sending reply");
 *                 }
 *             }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_SOCK_ASYNC_MSG_H
#define NET_SOCK_ASYNC_MSG_H

#include "kernel_types.h"
#include "msg.h"
#include "net/sock/async.h"
#ifdef MODULE_SOCK_DTLS
#include "net/sock/dtls.h"
#endif  /* MODULE_SOCK_DTLS */
#include "net/sock/ip.h"
#include "net/sock/tcp.h"
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type mask for all messages notifying events on the sock.
 *
 * The event flags of a sock event are identified by checking the respective
 * bits in the message type:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * if (sock_async_msg_type(&msg) && msg.type & SOCK_EVENT_CONN_RDY) {
 *     // ... the connection of msg.content.ptr is ready
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define SOCK_ASYNC_MSG_TYPE_MASK    (0xff00)

/**
 * @brief   Message type ID for all messages notifying events on the sock.
 */
#define SOCK_ASYNC_MSG_TYPE_ID      (0x8500)

/**
 * @brief   Checks if a given message is a @ref net_sock_async_msg message
 *
 * @param[in] msg   A message.
 */
static inline sock_async_flags_t sock_async_msg_type(msg_t *msg)
{
    return (msg->type & SOCK_ASYNC_MSG_TYPE_MASK) == SOCK_ASYNC_MSG_TYPE_ID;
}

#if defined(MODULE_SOCK_DTLS) || defined(DOXYGEN)
/**
 * @brief   Makes a DTLS sock able to handle asynchronous events using
 *          @ref core_msg.
 *
 * @param[in] sock      A DTLS sock object.
 * @param[in] target    Thread to send the events to.
 *
 * @note    Only available with module `sock_dtls`.
 */
void sock_dtls_msg_init(sock_dtls_t *sock, kernel_pid_t target);
#endif  /* defined(MODULE_SOCK_DTLS) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_IP) || defined(DOXYGEN)
/**
 * @brief   Makes a raw IPv4/IPv6 sock able to handle asynchronous events using
 *          @ref core_msg.
 *
 * @param[in] sock      A raw IPv4/IPv6 sock object.
 * @param[in] target    Thread to send the events to.
 *
 * @note    Only available with module `sock_ip`.
 */
void sock_ip_msg_init(sock_ip_t *sock, kernel_pid_t target);
#endif  /* defined(MODULE_SOCK_IP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_TCP) || defined(DOXYGEN)
/**
 * @brief   Makes a TCP sock able to handle asynchronous events using
 *          @ref core_msg.
 *
 * @param[in] sock      A TCP sock object.
 * @param[in] target    Thread to send the events to.
 *
 * @note    Only available with module `sock_tcp`.
 */
void sock_tcp_msg_init(sock_tcp_t *sock, kernel_pid_t target);

/**
 * @brief   Makes a TCP listening queue able to handle asynchronous events using
 *          @ref core_msg.
 *
 * @param[in] sock      A TCP listening queue.
 * @param[in] target    Thread to send the events to.
 *
 * @note    Only available with module `sock_tcp`.
 */
void sock_tcp_queue_msg_init(sock_tcp_queue_t *sock, kernel_pid_t target);
#endif  /* defined(MODULE_SOCK_TCP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
/**
 * @brief   Makes a UDP sock able to handle asynchronous events using
 *          @ref core_msg.
 *
 * @param[in] sock      A UDP sock object.
 * @param[in] target    Thread to send the events to.
 *
 * @note    Only available with module `sock_udp`.
 */
void sock_udp_msg_init(sock_udp_t *sock, kernel_pid_t target);
#endif  /* defined(MODULE_SOCK_UDP) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_ASYNC_MSG_H */
/** @} */
