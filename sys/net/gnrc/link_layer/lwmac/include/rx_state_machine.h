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
 * @brief       Implementation of RX state machine
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#ifndef LWMAC_RX_STATE_MACHINE_H_
#define LWMAC_RX_STATE_MACHINE_H_

#include "net/gnrc/pkt.h"
#include "lwmac_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void lwmac_rx_start(lwmac_t* lwmac);

void lwmac_rx_stop(lwmac_t* lwmac);

void lwmac_rx_update(lwmac_t* lwmac);

#ifdef __cplusplus
}
#endif

#endif /* LWMAC_RX_STATE_MACHINE_H_ */
