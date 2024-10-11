/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan    6LoWPAN
 * @ingroup     net_gnrc
 * @brief       GNRC's 6LoWPAN implementation
 *
 * # Internal API and sub-modules
 *
 * Internally, @ref net_gnrc_sixlowpan is sub-divided into several sub-modules.
 * They implement certain features of the 6LoWPAN standard. Currently
 * implemented are
 *
 * - [Fragmentation](https://tools.ietf.org/html/rfc4944#section-5.3)
 *   ([gnrc_sixlowpan_frag](@ref net_gnrc_sixlowpan_frag))
 * - [Uncompressed IPv6](https://tools.ietf.org/html/rfc4944#section-5.1)
 *   (as part of the main @ref net_gnrc_sixlowpan module)
 * - IPv6 datagram compression according to [RFC 6282](https://tools.ietf.org/html/rfc6282)
 *   aka IPHC ([gnrc_sixlowpan_iphc](@ref net_gnrc_sixlowpan_iphc), IPv6
 *   extension header NHC currently missing)
 *
 * Each sub-module has a `send` and `recv` function prefixed by their
 * respective sub-module name with the following signatures
 *
 * ~~~~~~~~~~~~~~~~~~~~~ {.c}
 * void send(gnrc_pktsnip_t *pkt, void *ctx, uint8_t page);
 * void recv(gnrc_pktsnip_t *pkt, void *ctx, uint8_t page);
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * A 6LoWPAN frame `pkt` must pass the sub-modules sequentially in the order of
 * its dispatches on receive or the step that makes most sense next on send.
 * After it was passed into another sub-module using the respective
 * `send`/`recv` function a sub-module must not operate on the `pkt` anymore.
 *
 * The `ctx` parameter can be used to provide data structures of a sub-module to
 * the next sub-module if that needs to modify or read them (e.g. reassembly
 * buffer state for IPHC) otherwise, leave it `NULL`.
 *
 * Finally, the `page` parameter is to provide a sub-module the current parsing
 * page context according to [RFC 8025](https://tools.ietf.org/html/rfc8025).
 *
 * # Supported NETAPI commands
 *
 * To interact with other modules this module handles the following
 * @ref net_gnrc_netapi message types:
 *
 * ## `GNRC_NETAPI_MSG_TYPE_RCV`
 *
 * @ref GNRC_NETAPI_MSG_TYPE_RCV expects a @ref net_gnrc_pkt (referred to as "packet" in the
 * following) in receive order (link-layer payload first, @ref net_gnrc_netif_hdr last) with the
 * payload marked with @ref GNRC_NETTYPE_SIXLOWPAN. Every other packet will be silently discarded.
 *
 * If the first byte of the payload is @ref SIXLOWPAN_UNCOMP this byte will be removed by marking
 * it with a new @ref gnrc_pktsnip_t and then removing this @ref gnrc_pktsnip_t, the remaining
 * payload will be marked with @ref GNRC_NETTYPE_IPV6 and this module will issue a
 * @ref GNRC_NETAPI_MSG_TYPE_RCV to all subscribers registered to @ref GNRC_NETTYPE_IPV6 with
 * demultiplex context @ref GNRC_NETREG_DEMUX_CTX_ALL in the @ref net_gnrc_netreg with the packet.
 * Otherwise, the packet will be silently discarded (except for the cases documented below).
 *
 * Depending on which other modules are included the behavior can be a little different:
 *
 *  1.  If @ref net_gnrc_sixlowpan_frag is included and @ref sixlowpan_frag_is() is true for the
 *      packet, the fragmentation header will be removed and its remaining data will be added to
 *      the reassembly buffer (using @ref gnrc_sixlowpan_frag_rb_add()) in accordance to the
 *      fragmentation header. The packet containing the fragment will be discarded. When the
 *      fragmented datagram is complete, its payload will be marked with @ref GNRC_NETTYPE_IPV6 and
 *      issued via a @ref GNRC_NETAPI_MSG_TYPE_RCV to all subscribers registered to @ref
 *      GNRC_NETTYPE_IPV6 with demultiplex context @ref GNRC_NETREG_DEMUX_CTX_ALL in the @ref
 *      net_gnrc_netreg. If reassembly times out or if fragments overlap the datagram will be
 *      silently discarded.
 *  2.  If @ref net_gnrc_sixlowpan_iphc is included the packet will not be send to the subscribers
 *      to @ref GNRC_NETTYPE_IPV6 with demultiplex context @ref GNRC_NETREG_DEMUX_CTX_ALL
 *      immediately, but it will be checked first if @ref sixlowpan_iphc_is() is true for its
 *      payload. If false it will be send to the @ref GNRC_NETTYPE_IPV6 subscribers as usual. If
 *      true the IPHC dispatch will be decompressed to a full IPv6 header first. The IPHC dispatch
 *      will be replaced by the uncompressed IPHC header, any NHC dispatch will be replaced by their
 *      respective uncompressed header. The resulting packet will then be issued to the
 *      @ref GNRC_NETTYPE_IPV6 subscribers as usual.
 *  3.  If both @ref net_gnrc_sixlowpan_frag and @ref net_gnrc_sixlowpan_iphc are included the
 *      and @ref sixlowpan_frag_is() is true for the packet, the fragmented datagram will be
 *      reassembled as described in (1). If for the remainder (after removal of the fragment
 *      header) of the first fragment @ref sixlowpan_iphc_is() is true, it will be decompressed as
 *      described in (2), with the exception that the packet will only be send to all receivers of
 *      @ref GNRC_NETTYPE_IPV6 as soon as the datagram was completely reassembled and not after
 *      directly after decompression. If @ref sixlowpan_iphc_is() is false, reassembly is handled
 *      completely as described in (1). It is assumed that a fragment can fit a full compression
 *      header (including inlined fields and possibly NHC/GHC headers) as specified in
 *      [RFC 6282, section 2](https://tools.ietf.org/html/rfc6282#section-2).
 *
 * ## `GNRC_NETAPI_MSG_TYPE_SND`
 *
 * @ref GNRC_NETAPI_MSG_TYPE_SND expects a @ref net_gnrc_pkt (referred to as "packet" in the
 * following) in send order (headers first, payload last) with the first header being a valid
 * @ref GNRC_NETTYPE_NETIF header and the second a @ref GNRC_NETTYPE_IPV6 header. Every other
 * packet will be silently discarded.
 *
 * The @ref GNRC_NETTYPE_SIXLOWPAN header must at least have the gnrc_netif_hdr_t::if_pid field
 * set to a legal, 6LoWPAN compatible interface referred to as `netif` in the
 * following, otherwise the packet will be discarded.
 *
 * If @ref net_gnrc_sixlowpan_iphc is included and @ref GNRC_NETIF_FLAGS_6LO_HC is enabled for
 * `netif` the @ref GNRC_NETTYPE_IPV6 header will be compressed according to
 * [RFC 6282, section 3](https://tools.ietf.org/html/rfc6282#section-3). If it is false the
 * @ref SIXLOWPAN_UNCOMP dispatch will be appended as a new @ref gnrc_pktsnip_t to the packet.
 * The false case also applies if @ref net_gnrc_sixlowpan_iphc is not included.
 *
 * If the packet without @ref GNRC_NETTYPE_NETIF header is shorter than
 * gnrc_netif_t::sixlo::max_frag_size of `netif` the packet will be send to the `netif`'s
 * thread. Otherwise if @ref net_gnrc_sixlowpan_frag is included the packet will be fragmented
 * according to [RFC 4944, section 5.3](https://tools.ietf.org/html/rfc4944#section-5.3) if the
 * packet is without @ref GNRC_NETTYPE_NETIF header shorter than @ref SIXLOWPAN_FRAG_MAX_LEN. If
 * none of these cases apply, the packet will be discarded silently.
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
 * @brief   Definitions for 6LoWPAN
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_H
#define NET_GNRC_SIXLOWPAN_H

#include <stdbool.h>

#include "sched.h"

#include "net/gnrc/sixlowpan/config.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/internal.h"
#include "net/gnrc/sixlowpan/iphc.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialization of the 6LoWPAN thread.
 *
 * @details If 6LoWPAN was already initialized, it will just return the PID of
 *          the 6LoWPAN thread.
 *
 * @return  The PID to the 6LoWPAN thread, on success.
 * @return  -EINVAL, if @ref GNRC_SIXLOWPAN_PRIO was greater than or equal to
 *          @ref SCHED_PRIO_LEVELS
 * @return  -EOVERFLOW, if there are too many threads running already in general
 */
kernel_pid_t gnrc_sixlowpan_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_SIXLOWPAN_H */
