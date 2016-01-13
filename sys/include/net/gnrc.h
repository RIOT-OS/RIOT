/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc    Generic (GNRC) network stack
 * @ingroup     net
 * @brief       RIOT's modular default IP network stack.
 *
 * About
 * =====
 *
 * This module is currently the default network stack for RIOT and includes
 * many components ranging from a @ref net_gnrc_nomac "simple MAC protocol"
 * through a fully-featured @ref net_gnrc_ipv6 implementation with @ref
 * net_gnrc_sixlowpan "6LowPAN" extensions to an @ref net_gnrc_udp "UDP"
 * implementation and @ref net_gnrc_rpl.
 *
 * A list of all features contained in the @ref net_gnrc is available in the
 * `Modules` section above.
 *
 * Integration into RIOT
 * =====================
 *
 * From the application layer the @ref net_gnrc can be accessed through the
 * @ref net_conn, while the interface to the @ref drivers_netdev_netdev2 is
 * defined by the @ref net_gnrc_netdev2.
 *
 * Architecture
 * ============
 *
 * ![GNRC Network Stack](riot-gnrc.svg)
 *
 * Each layer of the network stack runs in its own thread and each lower layer
 * thread has a higher priority than any upper layer thread. In this regard,
 * the thread of the MAC layer implementation has the highest priority and
 * threads on the application layer have the lowest priority. The communication
 * between threads is handled by the kernel's @ref core_msg functionality and
 * by the @ref net_gnrc_netapi. Most of the times IPC will take place between
 * threads of neighboring layers for packets that traverse the network stack up
 * or down.
 *
 * Due to the design of @ref net_gnrc "GNRC" and the nature of inter-process
 * communication, it is crucial for a new module that introduces a new thread
 * to follow a certain programming construct if it desires to interact with
 * other threads without blocking the system: Utilizing an `event loop`.
 *
 * Hence, a thread for @ref net_gnrc "GNRC" will usually consist of four basic
 * steps.
 *
 * 1. Initialize a message queue
 * 2. register for a @ref net_gnrc_nettype
 * 3. wait for a message
 * 4. react appropriately to a message and return to 3.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * void *_event_loop(void *arg)
 * {
 *     static msg_t _msg_q[Q_SZ];
 *     (void)arg;
 *     msg_init_queue(_msg_q, Q_SZ);
 *     gnrc_netreg_entry me_reg = {
 *             .demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL,
 *             .pid = thread_getpid()
 *         };
 *     gnrc_netreg_register(GNRC_NETTYPE_IPV6, &me_reg);
 *     while (1) {
 *         msg_receive(&msg);
 *         switch (msg.type) {
 *             case TYPE1:
 *                 callback1();
 *                 break;
 *             ...
 *         }
 *     }
 *
 *     return NULL;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note As an example have a look at the event loops of @ref net_gnrc_ipv6 and
 * @ref net_gnrc_rpl
 *
 * Receiving / Transmitting Packets
 * ================================
 *
 * Packets can be received or transmitted by interacting with the @ref
 * net_gnrc_netapi.
 *
 * Receiving Packets
 * -----------------
 *
 * The reception of a @ref net_gnrc_pkt from another thread is handled by the
 * @ref net_gnrc_netapi module. In order to receive a @ref net_gnrc_pkt of a
 * specific type, it is necessary to register for the appropriate @ref
 * net_gnrc_nettype first. Your thread will then be able to receive certain
 * commands defined in the @ref net_gnrc_netapi module (e.g. @ref
 * net_gnrc_netapi::GNRC_NETAPI_MSG_TYPE_RCV) for all @ref net_gnrc_pkt
 * "Packets" that your thread registered for.
 *
 * The following example will sketch how to receive incoming and outgoing UDP
 * traffic on port 80.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * void *_event_loop(void *arg)
 * {
 *     static msg_t _msg_q[Q_SZ];
 *     msg_t msg, reply;
 *     reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
 *     reply.content.value = -ENOTSUP;
 *     msg_init_queue(_msg_q, Q_SZ);
 *     gnrc_pktsnip_t *pkt = NULL;
 *     gnrc_netreg_entry me_reg = { .demux_ctx = 80, .pid = thread_getpid() };
 *     gnrc_netreg_register(GNRC_NETTYPE_UDP, &me_reg);
 *
 *     while (1) {
 *         msg_receive(&msg);
 *         switch (msg.type) {
 *             case GNRC_NETAPI_MSG_TYPE_RCV:
 *                 pkt = (gnrc_pktsnip_t *) msg.content.ptr;
 *                 _handle_incoming_pkt(pkt);
 *                 break;
 *             case GNRC_NETAPI_MSG_TYPE_SND:
 *                 pkt = (gnrc_pktsnip_t *) msg.content.ptr;
 *                 _handle_outgoing_pkt(pkt);
 *                 break;
 *              case GNRC_NETAPI_MSG_TYPE_SET:
 *              case GNRC_NETAPI_MSG_TYPE_GET:
 *                 msg_reply(&msg, &reply);
 *                 break;
 *             default:
 *                 break;
 *         }
 *     }
 *
 *     return NULL;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note It is necessary to react with a reply message to the other thread for
 *       @ref GNRC_NETAPI_MSG_TYPE_SET and @ref GNRC_NETAPI_MSG_TYPE_GET
 *       commands
 *
 * @note Do not forget to unregister with @ref gnrc_netreg_unregister() if you
 *       leave the function
 *       context
 *
 * Transmitting Packets
 * --------------------
 *
 * To relay the @ref net_gnrc_pkt to interested threads the @ref
 * net_gnrc_netapi offers a dispatch functionality. The following example
 * sketches the usage and assumes a valid @ref net_gnrc_pkt named `pkt`.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, 80, pkt)) {
 *      puts("Error: no thread is interested");
 *      gnrc_pktbuf_release(pkt);
 *      return;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The @ref net_gnrc_pkt "pkt" will be send to all threads that registered for
 * @ref GNRC_NETTYPE_UDP and the demux context `80`. Every registered thread
 * will receive a @ref GNRC_NETAPI_MSG_TYPE_SND command and can access the @ref
 * net_gnrc_pkt. If @ref gnrc_netapi_dispatch_send() is replaced by @ref
 * gnrc_netapi_dispatch_receive() then threads will receive the @ref
 * GNRC_NETAPI_MSG_TYPE_RCV command instead, again with access to the @ref
 * net_gnrc_pkt.
 *
 * How To Use
 * ==========
 * @ref net_gnrc is highly modular and can be adjusted to include only the
 * desired features. In the following several of the available modules will be
 * stated that you can include in your application's Makefile.
 *
 * - To include the default network device(s) on your board:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_netif_default
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - To auto-initialize these network devices as GNRC network interfaces
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += auto_init_gnrc_netif
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - You may choose to build either as an IPv6 Node
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_ipv6_default
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   or as an IPv6 Router
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_ipv6_router_default
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   An IPv6 Router can forward packets, while an IPv6 Node will simply drop
 *   packets not targeted to it. If an IEEE 802.15.4 network device is present
 *   @ref net_gnrc_sixlowpan (with @ref net_gnrc_sixlowpan_frag and @ref
 *   net_gnrc_sixlowpan_iphc) will be included automatically.

 * - For basic IPv6 (and 6LoWPAN) functionalities choose instead
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_ipv6
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   or
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_ipv6_router
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   respectively. Those modules provide the bare minimum of IPv6
 *   functionalities (no @ref net_gnrc_icmpv6, no @ref net_gnrc_ndp). Because
 *   of that, the @ref net_gnrc_ipv6_nc needs to be configured manually. If an
 *   IEEE 802.15.4 device is present @ref net_gnrc_sixlowpan will be included
 *   automatically, but no fragmentation or header compression support will be
 *   provided.
 *
 * - For @ref net_gnrc_icmpv6_echo "ICMPv6 echo request/reply (ping)"
 *   functionality:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_icmpv6_echo
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - For @ref net_gnrc_udp support include
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_udp
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - To use @ref net_conn_udp with GNRC include
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_conn_udp
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - To include the @ref net_gnrc_rpl module
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_rpl
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   This will include the @ref net_gnrc_rpl module. To provide forwarding
 *   capabilities it is necessary to build the application with
 *   `gnrc_ipv6_router_default` (or `gnrc_ipv6_router`), not
 *   `gnrc_ipv6_default` (or `gnrc_ipv6`).
 *
 * @{
 *
 * @file
 * @brief       Includes all essential GNRC network stack base modules
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef GNRC_NETBASE_H_
#define GNRC_NETBASE_H_

#include "net/netopt.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* this file does not provide anything on it's own */

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETBASE_H_ */
/** @} */
