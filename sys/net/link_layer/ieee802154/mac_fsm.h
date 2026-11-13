/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 *
 * @file
 * @author Moritz Holzer <moritz.holzer@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ieee802154/mac.h"


/**
 * @brief IEEE 802.15.4 MAC FSM events.
 */
typedef enum {
    IEEE802154_MAC_FSM_EV_SCAN_START,
    IEEE802154_MAC_FSM_EV_SCAN_TIMER,
    IEEE802154_MAC_FSM_EV_SCAN_DONE,
    IEEE802154_MAC_FSM_EV_ASSOC_REQ_RX,
    IEEE802154_MAC_FSM_EV_ASSOC_RES_RX,
    IEEE802154_MAC_FSM_EV_DISASSOC_RX,
    IEEE802154_MAC_FSM_EV_COORD_START,
    IEEE802154_MAC_FSM_EV_MLME_ASSOC_REQ,
    IEEE802154_MAC_FSM_EV_MLME_POLL,
    IEEE802154_MAC_FSM_EV_MLME_ASSOC_RES,
    IEEE802154_MAC_FSM_EV_ASSOC_TIMEOUT,
    IEEE802154_MAC_FSM_EV_TX_REQUEST,
    IEEE802154_MAC_FSM_EV_SLEEP,
    IEEE802154_MAC_FSM_EV_WAKE,
    IEEE802154_MAC_FSM_EV_RX_BEACON,
    IEEE802154_MAC_FSM_EV_RX_DATA,
    IEEE802154_MAC_FSM_EV_RX_CMD_DATA_REQ,
    IEEE802154_MAC_FSM_EV_RX_CMD_BEACON_REQ,
    IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_REQ,
    IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_RES,
    IEEE802154_MAC_FSM_EV_RX_CMD_DISASSOC,
    IEEE802154_MAC_FSM_EV_MCPS_DATA_REQ,
} ieee802154_mac_fsm_ev_t;

typedef struct {
    iolist_t *buf;
    const ieee802154_rx_info_t *info;
    eui64_t src_addr;
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t src_pan;
    int src_len;
    uint8_t frame_type;
    uint8_t cmd_type;
    uint8_t assoc_status;
    uint16_t assoc_short_addr;
    const void *dst_addr;
    const void *data_dst_addr;
    iolist_t *msdu;
    uint8_t msdu_handle;
    ieee802154_addr_mode_t src_mode;
    ieee802154_addr_mode_t dst_mode;
    uint16_t dst_panid;
    ieee802154_assoc_capability_t capability;
    bool ack_req;
    bool indirect;
    int *result;
} ieee802154_mac_fsm_ctx_t;

/**
 * @brief Handle a MAC FSM event and apply state transitions.
 */
int ieee802154_mac_fsm_process_ev_ctx(ieee802154_mac_t *mac, ieee802154_mac_fsm_ev_t ev,
                                      const ieee802154_mac_fsm_ctx_t *ctx);

int ieee802154_mac_fsm_request(ieee802154_mac_t *mac, ieee802154_mac_fsm_ev_t ev,
                               const ieee802154_mac_fsm_ctx_t *ctx);


#ifdef __cplusplus
}
#endif

/** @} */

