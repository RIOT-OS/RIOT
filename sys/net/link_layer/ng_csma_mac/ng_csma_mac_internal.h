/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_csma_mac
 * @{
 * 
 * @file
 * @brief       Internal definitions of the CSMA MAC layer
 *
 * @author      Jonas Remmert <j.remmert@phytec.de>
 */

#ifndef NG_CSMA_MAC_INTERNAL_H_
#define NG_CSMA_MAC_INTERNAL_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CSMA_IDLE,
    CSMA_WAIT,
    CSMA_TX,
    CSMA_WAIT_FOR_ACK
} csma_mac_states_t;


static int _mac_send_statemachine(ng_netdev_t *dev, ng_netdev_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* NG_AT86RF2XX_INTERNAL_H_ */
/** @} */
