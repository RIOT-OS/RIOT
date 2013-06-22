/*
 * 6LoWPAN MAC - layer 2 prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowmac.h
 * @brief   6lowpan link layer functions
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#ifndef SIXLOWMAC_H
#define SIXLOWMAC_H

#include <stdio.h>
#include <stdint.h>
#include "sixlowip.h"
#include "radio/radio.h"
#include <transceiver.h>

#define RADIO_STACK_SIZE            512
#define RADIO_RCV_BUF_SIZE          64
#define RADIO_SND_BUF_SIZE          100
#define RADIO_SENDING_DELAY         1000

extern uint16_t fragmentcounter;

uint8_t get_radio_address(void);
void set_radio_address(uint8_t addr);
void send_ieee802154_frame(ieee_802154_long_t *addr, uint8_t *payload,
                           uint8_t length, uint8_t mcast);
void init_802154_long_addr(ieee_802154_long_t *laddr);
void init_802154_short_addr(ieee_802154_short_t *saddr);
void sixlowmac_init(transceiver_type_t type);
ieee_802154_long_t *mac_get_eui(ipv6_addr_t *ipaddr);

#endif /* SIXLOWMAC_H*/
