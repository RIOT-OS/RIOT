/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     net_sixlowpan
 * @{
 *
 * @file        sixlowpan/mac.h
 * @brief       6LoWPAN data structs, and prototypes for MAC layer
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_MAC_H
#define SIXLOWPAN_MAC_H

#include <stdint.h>

#include "transceiver.h"

#include "sixlowpan/types.h"

/**
 * @brief   Maximum length of a IEEE 802.15.4 long address represented as string.
 */
#define IEEE_802154_MAX_ADDR_STR_LEN   (12)

/**
 * @brief   Gets current radio transmitter address.
 *
 * @return  Current radio address as 8-bit value.
 */
uint8_t sixlowpan_mac_get_radio_address(void);

/**
 * @brief   Sets radio transmitter address.
 *
 * @param[in]   addr    8-bit radio address.
 */
void sixlowpan_mac_set_radio_address(uint8_t addr);

/**
 * @brief   Generates EUI-64 from IEEE 802.15.4 PAN ID and
 *          radio transceiver address.
 *
 * @param[out]  laddr   The EUI-64 address of this node.
 */
void sixlowpan_mac_init_802154_long_addr(ieee_802154_long_t *laddr);

/**
 * @brief   Generates IEEE 802.15.4 16-bit short address from radio
 *          transceiver address.
 *
 * @param[out]  saddr   The IEEE 802.15.4 16-bit short address of this
 *                      node.
 */
void sixlowpan_mac_init_802154_short_addr(ieee_802154_short_t *saddr);

/**
 * @brief   Get pointer to potential EUI-64 bit of the IPv6 address.
 *
 * @param[in]   ipaddr  An IPv6 address of this node.
 *
 * @return      The EUI-64 address of this node.
 */
ieee_802154_long_t *sixlowpan_mac_get_eui64(const ipv6_addr_t *ipaddr);

/**
 * @brief   Send an IEEE 802.15.4 frame.
 *
 * @param[in]   addr    The destination address of the frame.
 * @param[in]   payload The payload of the frame.
 * @param[in]   length  The length of the payload.
 * @param[in]   mcast   send frame as multicast frame (identical to
 *                      give a destination address of 0).
 */
void sixlowpan_mac_send_ieee802154_frame(const ieee_802154_long_t *addr,
        const uint8_t *payload,
        uint8_t length, uint8_t mcast);

/**
 * @brief   Initialise 6LoWPAN MAC interface
 *
 * @param[in]   type    Type of transceiver.
 */
void sixlowpan_mac_init(transceiver_type_t type);

/**
 * @brief   Converts IEEE 802.15.4 long address into string.
 *          Note that addr_str must allocate at least
 *          IEEE_802154_MAX_ADDR_STR_LEN byte (12 byte).
 *
 * @param[out]  addr_str    The IEEE 802.15.4 long address as string. Must
 *                          allocate at least IEEE_802154_ADDR_STR_LEN byte (12
 *                          byte).
 * @param[in]   laddr       IEEE 802.15.4 address to be converted.
 *
 * @return  Pointer to addr_str.
 */
char *sixlowpan_mac_802154_long_addr_to_str(char *addr_str, const ieee_802154_long_t *laddr);

/** @} */
#endif /* SIXLOWPAN_MAC_H */
