/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_cc1xxx CC1100/CC1100e/CC1101/CC1200 common code
 * @ingroup     drivers_netdev
 * @ingroup     net_gnrc_netif
 * @{
 *
 * @file
 * @brief   CC110x/CC1200 adaption for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * Supported Transceivers
 * ======================
 *
 * This adaption layer is written to be used by CC110x and CC1200 transceivers,
 * but any transceiver using layer 2 addresses which are 1 byte in size would
 * likely be able to use it. Keep in mind that both CC110x and CC1200 are able
 * to transmit frames of up to 255 bytes (thus 253 bytes of payload, as the
 * layer 2 header is 2 bytes in size). Other transceivers only supporting
 * smaller frames may not be able to use all the upper layer protocols supported
 * by the CC110x and CC1200 transceivers.
 *
 * Frame Format
 * ============
 *
 *      0                   1                   2                   3
 *      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |  Destination  |    Source     |  Payload...
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * | Field       | Description                     |
 * |-------------|---------------------------------|
 * | Destination | The layer 2 destination address |
 * | Source      | The layer 2 source address      |
 * | Payload     | The payload (variable size)     |
 *
 * Layer 2 Broadcast
 * =================
 *
 * This adaption layer assumes that the layer 2 address `0x00` (see
 * @ref CC1XXX_BCAST_ADDR) is reserved for layer 2 broadcast, which is true for
 * CC110x and CC1200 transceivers (provided they are configured accordingly). If
 * more users of this adaption layer are added, this behaviour might needs to be
 * more generalized.
 */

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default protocol for data that is coming in
 */
#ifdef MODULE_GNRC_SIXLOWPAN
#define CC1XXX_DEFAULT_PROTOCOL         (GNRC_NETTYPE_SIXLOWPAN)
#else
#define CC1XXX_DEFAULT_PROTOCOL         (GNRC_NETTYPE_UNDEF)
#endif

/**
 * @brief Size of a layer 2 address on CC110x/CC1200 transceivers
 */
#define CC1XXX_ADDR_SIZE                (1)

/**
 * @brief Special layer 2 address reserved for broadcast frames
 */
#define CC1XXX_BCAST_ADDR               (0x00)

/**
 * @brief Layer 2 header used in CC1xxx frames
 *
 * This structure has the same memory layout as the data send in the frames.
 */
typedef struct __attribute__((packed)) {
    uint8_t dest_addr;      /**< Destination layer 2 address */
    uint8_t src_addr;       /**< Source layer 2 address */
} cc1xxx_l2hdr_t;

/**
 * @brief Users of the CC110x/CC1200 adaption layer have to overlap their
 *        device handle with this structure.
 *
 * The first two fields of the device structure of any transceiver driver using
 * this adaption layer have to be equal to the `cc1xxx_t` structure. This allows
 * efficient access to the current layer 2 address of the device from the
 * adaption layer.
 */
typedef struct {
    netdev_t netdev;        /**< RIOT's interface to this driver */
    uint8_t addr;           /**< Layer 2 address of this device */
} cc1xxx_t;

/**
 * @brief Statistics for one received frame
 */
typedef struct netdev_radio_rx_info cc1xxx_rx_info_t;

/**
 * @brief   Creates a CC110x/CC1200 network interface
 *
 * @param[out] netif    The interface. May not be `NULL`.
 * @param[in] stack     The stack for the network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the network interface's thread.
 * @param[in] name      Name for the network interface. May be NULL.
 * @param[in] dev       Device for the interface.
 *
 * @see @ref gnrc_netif_create()
 *
 * @return  0 on success
 * @return  negative number on error
 */
int gnrc_netif_cc1xxx_create(gnrc_netif_t *netif, char *stack, int stacksize,
                             char priority, char *name, netdev_t *dev);

/**
 * @brief   Retrieve a unique layer-2 address for a cc1xxx instance
 *
 * @note    This function has __attribute__((weak)) so you can override this, e.g.
 *          to construct an address. By default @ref luid_get is used.
 *
 * @param[in]   dev     The device descriptor of the transceiver
 * @param[out]  eui     Destination to write the address to
 */
void cc1xxx_eui_get(const netdev_t *dev, uint8_t *eui);

#ifdef __cplusplus
}
#endif

/** @} */
