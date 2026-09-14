/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_gnrc_ipv6 IPv6
 * @ingroup     net_gnrc
 * @brief       GNRC's IPv6 implementation
 *
 * This module is for usage with the @ref net_gnrc_netapi
 *
 * # Supported NETAPI commands
 *
 * This module handles the following @ref net_gnrc_netapi message types:
 *
 * ## `GNRC_NETAPI_MSG_TYPE_RCV`
 *
 * @ref GNRC_NETAPI_MSG_TYPE_RCV expects a @ref net_gnrc_pkt (referred to as
 * "packet" in the following) in receive order (payload first, headers ordered
 * down the stack). It must at least contain a link-layer payload starting
 * with a valid @ref net_ipv6_hdr. If it contains a @ref net_gnrc_netif_hdr this
 * header will be taken into account.
 *
 * If the link-layer payload is not an IPv6 packet (starting with a 0x6
 * half-byte) or the module is not able to parse it correctly it will be
 * dropped. If the packet can't be forwarded (either to another node or an upper
 * layer) it will be dropped. Otherwise, there are two scenarios of success:
 *
 * 1. If the destination address is an address on this host, and a suitable
 *    upper layer is registered the packet will be forwarded to that upper
 *    layer. An upper layer is suitable with its registration if
 *
 *      - it is registered with a tuple (@ref GNRC_NETTYPE_IPV6, `nh`), or
 *      - it is registered with a tuple (@ref gnrc_nettype_from_protnum(`nh`),
 *        @ref GNRC_NETREG_DEMUX_CTX_ALL).
 *
 *    In both cases `nh` is the [next header field of one of the IPv6 header]
 *    (@ref ipv6_hdr_t::nh) or the [next header field of one of the IPv6
 *    extension headers](@ref ipv6_ext_t::nh) within the packet. The IPv6 header
 *    and every IPv6 extension header between the IPv6 header and the payload
 *    will be marked as separate @ref gnrc_pktsnip_t (i.e. the IPv6 payload will
 *    be the first snip due to receive order). Note, that IPv6-internal headers
 *    (such as ICMPv6 and extension headers) are handled within this module. If
 *    the [`gnrc_ipv6_ext`](@ref net_gnrc_ipv6_ext) module is not present, IPv6
 *    extension headers can't be parsed, so the packet might not be read
 *    properly.
 * 2. If the receiving network interface (indicated by the
 *    gnrc_netif_hdr_t::if_pid if provided or the destination address in the
 *    IPv6 header) is configured as a forwarding interface, the destination is
 *    not an address on this host, and a route is configured for the destination
 *    the packet will be forwarded. In consequence, the module will emit a
 *    @ref GNRC_NETAPI_MSG_TYPE_SND with the packet reversed into send order and
 *    the (if necessary prepended) gnrc_netif_hdr_t::if_pid has the appropriate
 *    link-layer destination addresses to the next hop towards the destination.
 *
 * ## `GNRC_NETAPI_MSG_TYPE_SND`
 *
 * @ref GNRC_NETAPI_MSG_TYPE_SND expects a @ref net_gnrc_pkt (referred to as
 * "packet" in the following) in send order (headers ordered up the stack,
 * payload last). It must at least contain a snip of type @ref GNRC_NETTYPE_IPV6
 * as its first or second snip. If the first snip is not of type
 * @ref GNRC_NETTYPE_IPV6, it must be of type @ref GNRC_NETTYPE_NETIF.
 *
 * If the destination address within the @ref GNRC_NETTYPE_IPV6 snip is an
 * address on this host or the loopback address `::1`, the packet will be
 * [reversed](@ref gnrc_pktbuf_reverse_snips()) and
 * [merged](@ref gnrc_pktbuf_merge()) so that it has a format as though it came
 * from a network interface. It will then be handled as a received packet
 * ("looped back") by the IPv6 module (see previous section).
 *
 * Otherwise, if a route (or neighbor cache entry) to the IPv6 destination
 * address exists, the IPv6 header will be filled for fields that were not set
 * by upper layers and handed over to the link-layer (L2) with the L2
 * destination address set to the L2 address associated to that IPv6 destination
 * address.
 *
 * ## `GNRC_NETAPI_MSG_TYPE_SET`
 *
 * `GNRC_NETAPI_MSG_TYPE_SET` is not supported.
 *
 * ## `GNRC_NETAPI_MSG_TYPE_GET`
 *
 * `GNRC_NETAPI_MSG_TYPE_GET` is not supported.
 *
 * @{
 *
 * @file
 * @brief       Definitions for GNRC's IPv6 implementation
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "sched.h"
#include "thread.h"

#include "net/ipv6.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib.h"

#ifdef MODULE_FIB
#include "net/fib.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup    net_gnrc_ipv6_conf  GNRC IPv6 compile configurations
 * @ingroup     net_gnrc_ipv6
 * @ingroup     net_gnrc_conf
 * @{
 */
/**
 * @brief   Default stack size to use for the IPv6 thread
 *
 * @note    The message queue was previously allocated on the stack.
 *          The default number of messages is 2³.
 *          Given sizeof(msg_t) == 8, the stack size is reduced by 64 bytes.
 */
#ifndef GNRC_IPV6_STACK_SIZE
#define GNRC_IPV6_STACK_SIZE        ((THREAD_STACKSIZE_DEFAULT) - 64)
#endif

/**
 * @brief   Default priority for the IPv6 thread
 */
#ifndef GNRC_IPV6_PRIO
#define GNRC_IPV6_PRIO              (THREAD_PRIORITY_MAIN - 3)
#endif

/**
 * @brief   Default message queue size to use for the IPv6 thread (as exponent
 *          of 2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_GNRC_IPV6_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_IPV6_MSG_QUEUE_SIZE_EXP    (3U)
#endif

#ifdef DOXYGEN
/**
 * @brief   Add a static IPv6 link local address to any network interface
 *
 * This macro allows to specify a certain link local IPv6 address to be assigned
 * to a network interface on startup, which might be handy for testing.
 * Note: a) a interface will keep its auto-generated link local address, too
 *       b) the address is incremented by the interface PID unless
            `CONFIG_GNRC_IPV6_STATIC_LLADDR_IS_FIXED` is set.
 *
 * To use the macro just add it to `CFLAGS` in the application's Makefile, like:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
 * CFLAGS += -DCONFIG_GNRC_IPV6_STATIC_LLADDR=$(STATIC_IPV6_LLADDR)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define CONFIG_GNRC_IPV6_STATIC_LLADDR
#endif /* DOXYGEN */
/** @} */

/**
 * @brief   Use the same static IPv6 link local address on every network interface
 *
 * @deprecated  Will be removed after release 2025.07 - after this the static link-local
 *              address will always be fixed unless a use-case for the auto-increment
 *              can be found.
 *
 * When CONFIG_GNRC_IPV6_STATIC_LLADDR is used, to not add the interface pid to the
 * set static address but use the same static link local address for all interfaces.
 */
#ifndef CONFIG_GNRC_IPV6_STATIC_LLADDR_IS_FIXED
#define CONFIG_GNRC_IPV6_STATIC_LLADDR_IS_FIXED 1
#endif

/**
 * @brief   Select interfaces by driver types for setting static link local
 *          addresses
 *
 * This option allows to explicitly include interfaces by matching their
 * netdev driver types, encoded in a bitmask.
 * See @ref netdev_type_t for possible values of netdev driver types.
 * Matching NETDEV_ANY will include all netdev driver types.
 *
 * Example usage, includes AT86RF215 and MRF24J40 driver types:
 *
 * @code{.c}
 * #define CONFIG_GNRC_IPV6_STATIC_LLADDR_NETDEV_MASK \
 *    ((1UL << NETDEV_AT86RF215) | (1UL << NETDEV_MRF24J40))
 * @endcode
 *
 * A value of 0 will switch this selection feature off.
 */
#ifndef CONFIG_GNRC_IPV6_STATIC_LLADDR_NETDEV_MASK
#define CONFIG_GNRC_IPV6_STATIC_LLADDR_NETDEV_MASK 0ULL
#endif

/**
 * @brief Message queue size to use for the IPv6 thread.
 */
#ifndef GNRC_IPV6_MSG_QUEUE_SIZE
#define GNRC_IPV6_MSG_QUEUE_SIZE    (1 << CONFIG_GNRC_IPV6_MSG_QUEUE_SIZE_EXP)
#endif

/**
 * @brief   The PID to the IPv6 thread.
 *
 * @note    Use @ref gnrc_ipv6_init() to initialize. **Do not set by hand**.
 *
 * @details This variable is preferred for IPv6 internal communication *only*.
 *          Please use @ref net_gnrc_netreg for external communication.
 */
extern kernel_pid_t gnrc_ipv6_pid;

#ifdef MODULE_FIB

/**
 * @brief   Maximum number of entries in the IPv6 FIB table.
 */
#ifndef GNRC_IPV6_FIB_TABLE_SIZE
#   ifdef MODULE_GNRC_RPL
#       define GNRC_IPV6_FIB_TABLE_SIZE     (20)
#   else
#       define GNRC_IPV6_FIB_TABLE_SIZE     (5)
#   endif
#endif

/**
 * @brief   The forwarding information base (FIB) for the IPv6 stack.
 *
 * @see @ref net_fib
 */
extern fib_table_t gnrc_ipv6_fib_table;
#endif

/**
 * @brief   Initialization of the IPv6 thread.
 *
 * @return  The PID to the IPv6 thread, on success.
 * @return  a negative errno on error.
 * @return  -EOVERFLOW, if there are too many threads running already
 * @return  -EEXIST, if IPv6 was already initialized.
 */
kernel_pid_t gnrc_ipv6_init(void);

/**
 * @brief   Get the IPv6 header from a given list of @ref gnrc_pktsnip_t
 *
 *          This function may be used with e.g. a pointer to a (full) UDP datagram.
 *
 * @pre Any @ref GNRC_NETTYPE_IPV6 snip in pkt is contains a full IPv6 header.
 *
 * @param[in] pkt    The pointer to the first @ref gnrc_pktsnip_t of the
 *                   packet.
 *
 * @return A pointer to the @ref ipv6_hdr_t of the packet.
 * @return NULL if the packet does not contain an IPv6 header.
 */
ipv6_hdr_t *gnrc_ipv6_get_header(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
