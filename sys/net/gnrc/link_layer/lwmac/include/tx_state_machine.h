/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_lwmac
 * @file
 * @brief       Implementation of TX state machine
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#ifndef LWMAC_TX_STATE_MACHINE_H_
#define LWMAC_TX_STATE_MACHINE_H_

#include "net/gnrc/pkt.h"
#include "lwmac_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void lwmac_tx_start(lwmac_t* lwmac, gnrc_pktsnip_t* pkt, lwmac_tx_neighbour_t* neighbour);

void lwmac_tx_stop(lwmac_t* lwmac);

void lwmac_tx_update(lwmac_t* lwmac);

#ifdef __cplusplus
}
#endif

#endif /* LWMAC_TX_STATE_MACHINE_H_ */
