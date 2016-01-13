/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netreg  Network protocol registry
 * @ingroup     net_gnrc
 * @brief       Registry to receive messages of a specified protocol type by GNRC.
 * @{
 *
 * @file
 * @brief   Definitions to register network protocol PIDs to use with
 *          @ref net_gnrc_netapi.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NETREG_H_
#define NETREG_H_

#include <inttypes.h>

#include "kernel_types.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Demux context value to get all packets of a certain type.
 *
 * @see gnrc_netreg_entry_t::demux_ctx
 */
#define GNRC_NETREG_DEMUX_CTX_ALL   (0xffff0000)

/**
 * @brief   Entry to the @ref net_gnrc_netreg
 */
typedef struct gnrc_netreg_entry {
    /**
     * @brief next element in list
     *
     * @internal
     */
    struct gnrc_netreg_entry *next;

    /**
     * @brief   The demultiplexing context for the registering thread.
     *
     * @details This can be defined by the network protocol themselves.
     *          E. g. protocol numbers / next header numbers in IPv4/IPv6,
     *          ports in UDP/TCP, or similar.
     */
    uint32_t demux_ctx;
    kernel_pid_t pid;       /**< The PID of the registering thread */
} gnrc_netreg_entry_t;

/**
 * @brief   Initializes module.
 */
void gnrc_netreg_init(void);

/**
 * @brief   Registers a thread to the registry.
 *
 * @details The semantics are: Thread gnrc_netreg_entry_t::pid is interested in
 *          packets of protocol @p type with context gnrc_netreg_entry_t::demux_ctx.
 *
 * @param[in] type      Type of the protocol. Must not be < GNRC_NETTYPE_UNDEF or
 *                      >= GNRC_NETTYPE_NUMOF.
 * @param[in] entry     An entry you want to add to the registry with
 *                      gnrc_netreg_entry_t::pid and gnrc_netreg_entry_t::demux_ctx set.
 *
 * @warning Call gnrc_netreg_unregister() *before* you leave the context you
 *          allocated @p entry in. Otherwise it might get overwritten.
 *
 * @pre The calling thread must provide a message queue.
 *
 * @return  0 on success
 * @return  -EINVAL if @p type was < GNRC_NETTYPE_UNDEF or >= GNRC_NETTYPE_NUMOF
 */
int gnrc_netreg_register(gnrc_nettype_t type, gnrc_netreg_entry_t *entry);

/**
 * @brief   Removes a thread from the registry.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] entry     An entry you want to remove from the registry.
 */
void gnrc_netreg_unregister(gnrc_nettype_t type, gnrc_netreg_entry_t *entry);

/**
 * @brief   Searches for entries with given parameters in the registry and
 *          returns the first found.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See gnrc_netreg_entry_t::demux_ctx.
 *
 * @return  The first entry fitting the given parameters on success
 * @return  NULL if no entry can be found.
 */
gnrc_netreg_entry_t *gnrc_netreg_lookup(gnrc_nettype_t type, uint32_t demux_ctx);

/**
 * @brief   Returns number of entries with the same gnrc_netreg_entry_t::type and
 *          gnrc_netreg_entry_t::demux_ctx.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See gnrc_netreg_entry_t::demux_ctx.
 *
 * @return  Number of entries with the same gnrc_netreg_entry_t::type and
 *          gnrc_netreg_entry_t::demux_ctx as the given parameters.
 */
int gnrc_netreg_num(gnrc_nettype_t type, uint32_t demux_ctx);

/**
 * @brief   Returns the next entry after @p entry with the same
 *          gnrc_netreg_entry_t::type and gnrc_netreg_entry_t::demux_ctx as the
 *          given entry.
 *
 * @param[in] entry     A registry entry retrieved by gnrc_netreg_lookup() or
 *                      gnrc_netreg_getnext(). Must not be NULL.
 *
 * @return  The next entry after @p entry fitting the given parameters on success
 * @return  NULL if no entry new entry can be found.
 */
gnrc_netreg_entry_t *gnrc_netreg_getnext(gnrc_netreg_entry_t *entry);

/**
 * @brief   Calculates the checksum for a header.
 *
 * @param[in] hdr           The header the checksum should be calculated
 *                          for.
 * @param[in] pseudo_hdr    The header the pseudo header shall be generated
 *                          from. NULL if none is needed.
 *
 * @return  0, on success.
 * @return  -EINVAL, if @p pseudo_hdr is NULL but a pseudo header was required.
 * @return  -ENOENT, if @ref net_gnrc_netreg does not know how to calculate checksum
 *          for gnrc_pktsnip_t::type of @p hdr.
 */

int gnrc_netreg_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr);

/**
 * @brief   Builds a header for sending and adds it to the packet buffer.
 *
 * @param[in] type      Type of the header.
 * @param[in] payload   Payload for the packet.
 * @param[in] src       Source address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] src_len   Length of @p src. Can be 0 if not known or required.
 * @param[in] dst       Destination address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] dst_len   Length of @p dst. Can be 0 if not known or required.
 *
 * @return  The header for the protocol on success.
 * @return  NULL on error.
 */
gnrc_pktsnip_t *gnrc_netreg_hdr_build(gnrc_nettype_t type, gnrc_pktsnip_t *payload,
                                      uint8_t *src, uint8_t src_len,
                                      uint8_t *dst, uint8_t dst_len);

#ifdef __cplusplus
}
#endif

#endif /* NETREG_H_ */
/** @} */
