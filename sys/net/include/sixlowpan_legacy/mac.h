/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_sixlowpan_legacy_lowpan
 * @{
 *
 * @file        sixlowpan_legacy/mac.h
 * @brief       6LoWPAN data structs, and prototypes for MAC layer
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_LEGACY_MAC_H
#define SIXLOWPAN_LEGACY_MAC_H

#include <stdint.h>

#include "transceiver.h"

#include "sixlowpan_legacy/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum length of a IEEE 802.15.4 long address represented as string.
 */
#define IEEE_802154_MAX_ADDR_STR_LEN   (12)

/**
 * @brief   Send an IEEE 802.15.4 frame to a long address.
 *
 * @param[in]   if_id       The interface to send over (will be ignored if
 *                          *mcast* is 1).
 * @param[in]   dest        The destination address of the frame (will be
 *                          ignored if *mcast* is 1).
 * @param[in]   dest_len    The lengts of the destination address in byte.
 * @param[in]   payload     The payload of the frame.
 * @param[in]   length      The length of the payload.
 * @param[in]   mcast       send frame as multicast frame (*addr* and *if_id*
 *                          will be ignored).
 *
 * @return Length of transmitted data in byte
 */
int sixlowpan_legacy_mac_send_ieee802154_frame(int if_id, const void *dest,
                                        uint8_t dest_len, const void *payload, uint8_t length, uint8_t mcast);

/**
 * @brief   Initialise 6LoWPAN MAC layer and register it to interface layer
 *
 * @return  PID of the MAC receiver thread.
 */
kernel_pid_t sixlowpan_legacy_mac_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SIXLOWPAN_LEGACY_MAC_H */
