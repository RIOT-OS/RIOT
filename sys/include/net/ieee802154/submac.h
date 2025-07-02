/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @defgroup     net_ieee802154_submac IEEE802.15.4 SubMAC layer
 * @ingroup      net_ieee802154
 * @experimental This API is experimental and in an early state - expect
 *               changes!

 * @brief        This module defines a common layer for handling the lower
 * part of the IEEE 802.15.4 MAC layer.
 *
 * This layer is responsible for:
 * - Handling CSMA-CA and retransmissions.
 * - Maintaining part of the MAC Information Base, e.g IEEE 802.15.4 addresses,
 *   channel settings, CSMA-CA params, etc.
 *
 * The SubMAC defines the following state machine:
 *
 * ```
 *  +--------+        +--------+     +--------+
 *  |        |------->|        |     |        |
 *  |   RX   |        |PREPARE |<--->|   TX   |
 *  |        |   +--->|        |     |        |
 *  +--------+   |    +--------+     +--------+
 *      ^        |        ^              |
 *      |        |        |              |
 *      |        |        |              |
 *      |        |    +--------+         |
 *      |        |    |        |         v
 *      |        |    |WAIT FOR|<--------+
 *      |        |    |  ACK   |         |
 *      |        |    +--------+         |
 *      |        |         |             |
 *      |        |         |             |
 *      |        |         v             |
 *      |        |     +--------+        |
 *      |        +-----|        |        |
 *      |              |  IDLE  |        |
 *      +------------->|        |<-------+
 *                     +--------+
 * ```
 *
 * - IDLE: The transceiver is off and therefore cannot receive frames. Sending
 *   frames might be triggered using @ref ieee802154_send. The next SubMAC
 *   state would be PREPARE.
 * - RX: The device is ready to receive frames. In case the SubMAC receives a
 *   frame it will call @ref ieee802154_submac_cb_t::rx_done and immediately go
 *   to IDLE. Same as the IDLE state, it's possible
 *   to trigger frames using @ref ieee802154_send.
 * - PREPARE: The frame is already in the framebuffer and waiting to be
 *   transmitted.  This state might handle CSMA-CA backoff timer in case the
 *   device doesn't support it. The SubMAC will then request the transmission
 *   and go immediately to the TX state.
 * - TX: The frame was already sent and it's waiting for the TX DONE event from
 *   the radio. The SubMAC might call @ref ieee802154_submac_cb_t::tx_done if
 *   any of the following criteria are meet:
 *   - The transmitted frame didn't request ACK
 *   - The radio already handles retransmissions
 * - WAIT FOR ACK: The SubMAC is waiting for an ACK frame.
 *   In case a valid ACK frame is received, the SubMAC will
 *   either to IDLE.
 *   In case the ACK frame is invalid or there's an ACK timeout event
 *   (either triggered by the radio or a timer), the SubMAC goes to either
 *   IDLE if there are no more retransmissions left or no more CSMA-CA
 *   retries or PREPARE otherwise.
 *
 * The events that trigger state machine changes are defined in
 * @ref ieee802154_fsm_state_cb_t
 *
 * The following events are valid for each state:
 *
 *  Event/State  | RX | IDLE  | PREPARE | TX | WAIT FOR ACK
 * --------------|----|-------|---------|----|-------------
 * TX_DONE       | -  | -     | -       | X  | -
 * RX_DONE       | X  | X*    | X*      | X* | X
 * CRC_ERROR     | X  | X*    | X*      | X* | X
 * ACK_TIMEOUT   | -  | -     | -       | -  | X
 * REQ_TX        | X  | X     | -       | -  | -
 * REQ_SET_RX_ON | -  | X     | -       | -  | -
 * REQ_SET_IDLE  | X  | -     | -       | -  | -
 *
 * *: RX_DONE and CRC_ERROR during these events might be a race condition
 *    between the ACK Timer and the radios RX_DONE event. If this happens, the
 *    SubMAC will react accordingly
 *
 * Unexpected events will be reported and asserted.
 *
 * The upper layer needs to implement the following callbacks:
 *
 * - @ref ieee802154_submac_cb_t::rx_done.
 * - @ref ieee802154_submac_cb_t::tx_done.
 * - @ref ieee802154_submac_ack_timer_set
 * - @ref ieee802154_submac_ack_timer_cancel
 *
 * @{
 *
 * @author       José I. Alamos <jose.alamos@haw-hamburg.de>
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"

#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

/**
 * @brief IEEE 802.15.4 SubMAC forward declaration
 */
typedef struct ieee802154_submac ieee802154_submac_t;

/**
 * @brief IEEE 802.15.4 SubMAC callbacks.
 */
typedef struct {
    /**
     * @brief RX done event
     *
     * This function is called from the SubMAC to indicate a IEEE 802.15.4
     * frame is ready to be fetched from the device. Use @ref
     * ieee802154_read_frame and/or @ref ieee802154_get_frame_length for this
     * purpose.
     *
     * The SubMAC will automatically go to IDLE.
     *
     * @note ACK frames are automatically handled and discarded by the SubMAC.
     * @param[in] submac pointer to the SubMAC descriptor
     */
    void (*rx_done)(ieee802154_submac_t *submac);
    /**
     * @brief TX done event
     *
     * This function is called from the SubMAC to indicate that the TX
     * procedure finished.
     *
     * The SubMAC will automatically go to IDLE.
     *
     * @param[in] submac pointer to the SubMAC descriptor
     * @param[out] info TX information associated to the transmission (status,
     *                 number of retransmissions, pending bit, etc).
     */
    void (*tx_done)(ieee802154_submac_t *submac, int status,
                    ieee802154_tx_info_t *info);
} ieee802154_submac_cb_t;

/**
 * @brief Internal SubMAC FSM state machine events
 */
typedef enum {
    IEEE802154_FSM_EV_ENTRY,                /**< FSM calls entry function of a state */
    IEEE802154_FSM_EV_EXIT,                 /**< FSM calls exit function of a state */
    IEEE802154_FSM_EV_TX_DONE,              /**< Radio reports frame was sent */
    IEEE802154_FSM_EV_RX_DONE,              /**< Radio reports frame was received */
    IEEE802154_FSM_EV_CRC_ERROR,            /**< Radio reports frame was received but CRC failed */
    IEEE802154_FSM_EV_ACK_TIMEOUT,          /**< ACK timer fired */
    IEEE802154_FSM_EV_REQUEST_TX,           /**< The upper layer requested to transmit a frame */
    IEEE802154_FSM_EV_REQUEST_SET_RX_ON,    /**< The upper layer requested to go to RX */
    IEEE802154_FSM_EV_REQUEST_SET_IDLE,     /**< The upper layer requested to go to IDLE */
    IEEE802154_FSM_EV_NUMOF,                /**< Number of SubMAC FSM events */
} ieee802154_fsm_ev_t;

/**
 * @brief Internal SubMAC FSM process Event return status
 */
typedef enum {
    IEEE802154_SUBMAC_FSM_RETURN_HANDLED,       /**< Event was handled */
    IEEE802154_SUBMAC_FSM_RETURN_TRANSITION,    /**< Event has caused a transition */
    IEEE802154_SUBMAC_FSM_RETURN_IGNORED,       /**< Event was ignored, no action was performed*/
} ieee802154_submac_fsm_return_t;

/**
 * @brief Internal SubMAC FSM state
 */
typedef ieee802154_submac_fsm_return_t (*ieee802154_fsm_state_cb_t)(ieee802154_submac_t *submac,
                                                                      ieee802154_fsm_ev_t ev,
                                                                      void* ctx);

/**
 * @brief Internal SubMAC FSM
 */
typedef struct {
    ieee802154_fsm_state_cb_t fsm_state_cb;     /**< current state of the SubMAC FSM */
    uint8_t busy_status;                        /**< flag if the SubMAC is in use or not */
} ieee802154_fsm_t;

/**
 * @brief IEEE 802.15.4 SubMAC descriptor
 */
struct ieee802154_submac {
    ieee802154_dev_t dev;               /**< 802.15.4 HAL descriptor */
    eui64_t ext_addr;                   /**< IEEE 802.15.4 extended address */
    network_uint16_t short_addr;        /**< IEEE 802.15.4 short address */
    const ieee802154_submac_cb_t *cb;   /**< pointer to the SubMAC callbacks */
    ieee802154_csma_be_t be;            /**< CSMA-CA backoff exponent params */
    bool wait_for_ack;                  /**< SubMAC is waiting for an ACK frame */
    uint16_t ack_timeout_us;            /**< ACK timeout in µs */
    uint16_t csma_backoff_us;           /**< CSMA sender backoff period in µs */
    uint16_t panid;                     /**< IEEE 802.15.4 PAN ID */
    uint16_t channel_num;               /**< IEEE 802.15.4 channel number */
    uint8_t channel_page;               /**< IEEE 802.15.4 channel page */
    uint8_t retrans;                    /**< current number of retransmissions */
    uint8_t csma_retries_nb;            /**< current number of CSMA-CA retries */
    uint8_t backoff_mask;               /**< internal value used for random backoff calculation */
    uint8_t csma_retries;               /**< maximum number of CSMA-CA retries */
    int8_t tx_pow;                      /**< Transmission power (in dBm) */
    ieee802154_fsm_t fsm;               /**< FSM of the SubMAC */
    int32_t fsm_context_res;            /**< Result of the proceeded fsm action */
    ieee802154_phy_mode_t phy_mode;     /**< IEEE 802.15.4 PHY mode */
    const iolist_t *psdu;               /**< stores the current PSDU */
};

/**
 * @brief Transmit an IEEE 802.15.4 PSDU
 *
 * This function performs an IEEE 802.15.4 transmission, including CSMA-CA and
 * retransmissions (if ACK Request bit is set).  When the transmission finishes
 * an @ref ieee802154_submac_cb_t::tx_done event is issued.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] iolist pointer to the PSDU frame (without FCS)
 *
 * @return 0 on success
 * @return -EBUSY if the SubMAC is not in RX or IDLE state or if called inside
 *         @ref ieee802154_submac_cb_t::rx_done or
 *         @ref ieee802154_submac_cb_t::tx_done
 */
int ieee802154_send(ieee802154_submac_t *submac, const iolist_t *iolist);

/**
 * @brief Set the IEEE 802.15.4 short address
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] short_addr IEEE 802.15.4 short address
 *
 * @return 0 on success
 * @return negative errno on error
 */
static inline int ieee802154_set_short_addr(ieee802154_submac_t *submac,
                                            const network_uint16_t *short_addr)
{
    int res = ieee802154_radio_config_addr_filter(&submac->dev,
                                                  IEEE802154_AF_SHORT_ADDR,
                                                  short_addr);

    if (res >= 0) {
        memcpy(&submac->short_addr, short_addr, IEEE802154_SHORT_ADDRESS_LEN);
    }

    return res;
}

/**
 * @brief Set the IEEE 802.15.4 extended address
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] ext_addr IEEE 802.15.4 extended address
 *
 * @return 0 on success
 * @return negative errno on error
 */
static inline int ieee802154_set_ext_addr(ieee802154_submac_t *submac,
                                          const eui64_t *ext_addr)
{
    int res = ieee802154_radio_config_addr_filter(&submac->dev,
                                                  IEEE802154_AF_EXT_ADDR,
                                                  ext_addr);

    if (res >= 0) {
        memcpy(&submac->ext_addr, ext_addr, IEEE802154_LONG_ADDRESS_LEN);
    }
    return res;
}

/**
 * @brief Set the IEEE 802.15.4 PAN ID
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] panid IEEE 802.15.4 PAN ID
 *
 * @return 0 on success
 * @return negative errno on error
 */
static inline int ieee802154_set_panid(ieee802154_submac_t *submac,
                                       const uint16_t *panid)
{
    int res = ieee802154_radio_config_addr_filter(&submac->dev,
                                                  IEEE802154_AF_PANID,
                                                  panid);

    if (res >= 0) {
        submac->panid = *panid;
    }

    return res;
}

/**
 * @brief Get IEEE 802.15.4 PHY mode
 *
 * @param[in] submac pointer to the SubMAC descriptor
 *
 * @return PHY mode.
 */
static inline ieee802154_phy_mode_t ieee802154_get_phy_mode(
    ieee802154_submac_t *submac)
{
    return submac->phy_mode;
}

/**
 * @brief Set IEEE 802.15.4 PHY configuration (channel, TX power)
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] conf   pointer to the PHY configuration
 *
 * @return 0 on success
 * @return -ENOTSUP if the PHY settings are not supported
 * @return -EBUSY if the SubMAC is not in RX or IDLE state or if called inside
 *         @ref ieee802154_submac_cb_t::rx_done or
 *         @ref ieee802154_submac_cb_t::tx_done
 * @return negative errno on error
 */
int ieee802154_set_phy_conf(ieee802154_submac_t *submac, const ieee802154_phy_conf_t *conf);

/**
 * @brief Set IEEE 802.15.4 channel number
 *
 * This is a shortcut to @ref ieee802154_set_phy_conf
 *
 * @param[in] submac pointer the SubMAC descriptor
 * @param[in] channel_num channel number
 *
 * @return 0 on success
 * @return -ENOTSUP if the channel number is not supported
 * @return -EBUSY if the SubMAC is not in RX or IDLE state or if called inside
 *         @ref ieee802154_submac_cb_t::rx_done or
 *         @ref ieee802154_submac_cb_t::tx_done
 * @return negative errno on error
 */
static inline int ieee802154_set_channel_number(ieee802154_submac_t *submac,
                                                uint16_t channel_num)
{
    const ieee802154_phy_conf_t conf = {
        .phy_mode = IEEE802154_PHY_NO_OP,
        .channel = channel_num,
        .page = submac->channel_page,
        .pow = submac->tx_pow,
    };

    return ieee802154_set_phy_conf(submac, &conf);
}

/**
 * @brief Set IEEE 802.15.4 channel page
 *
 * This is a shortcut to @ref ieee802154_set_phy_conf
 *
 * @param[in] submac pointer the SubMAC descriptor
 * @param[in] channel_page channel page
 *
 * @return 0 on success
 * @return -ENOTSUP if the channel page is not supported
 * @return -EBUSY if the SubMAC is not in RX or IDLE state or if called inside
 *         @ref ieee802154_submac_cb_t::rx_done or
 *         @ref ieee802154_submac_cb_t::tx_done
 * @return negative errno on error
 */
static inline int ieee802154_set_channel_page(ieee802154_submac_t *submac,
                                              uint16_t channel_page)
{
    const ieee802154_phy_conf_t conf = {
        .phy_mode = IEEE802154_PHY_NO_OP,
        .channel = submac->channel_num,
        .page = channel_page,
        .pow = submac->tx_pow,
    };

    return ieee802154_set_phy_conf(submac, &conf);
}

/**
 * @brief Set IEEE 802.15.4 transmission power
 *
 * This is a shortcut to @ref ieee802154_set_phy_conf
 *
 * @param[in] submac pointer the SubMAC descriptor
 * @param[in] tx_pow transmission power (in dBm)
 *
 * @return 0 on success
 * @return -ENOTSUP if the transmission power is not supported
 * @return -EBUSY if the SubMAC is not in RX or IDLE state or if called inside
 *         @ref ieee802154_submac_cb_t::rx_done or
 *         @ref ieee802154_submac_cb_t::tx_done
 * @return negative errno on error
 */
static inline int ieee802154_set_tx_power(ieee802154_submac_t *submac,
                                          int8_t tx_pow)
{
    const ieee802154_phy_conf_t conf = {
        .phy_mode = IEEE802154_PHY_NO_OP,
        .channel = submac->channel_num,
        .page = submac->channel_page,
        .pow = tx_pow,
    };

    return ieee802154_set_phy_conf(submac, &conf);
}

/**
 * @brief Get the received frame length
 *
 * @pre this function MUST be called either inside @ref ieee802154_submac_cb_t::rx_done
 *      or in SLEEP state.
 *
 * @param[in] submac pointer to the SubMAC
 *
 * @return length of the PSDU (excluding FCS length)
 */
static inline int ieee802154_get_frame_length(ieee802154_submac_t *submac)
{
    return ieee802154_radio_len(&submac->dev);
}

/**
 * @brief Read the received frame
 *
 * This functions reads the received PSDU from the device (excluding FCS)
 *
 * @pre this function MUST be called either inside @ref ieee802154_submac_cb_t::rx_done
 *      or in SLEEP state.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[out] buf buffer to write into. If NULL, the packet is discarded
 * @param[in] len length of the buffer
 * @param[out] info RX information of the packet. If NULL, the information is not fetched.
 *
 * @return the number of bytes written to @p buf
 * @return negative errno on error
 */
static inline int ieee802154_read_frame(ieee802154_submac_t *submac, void *buf,
                                        size_t len, ieee802154_rx_info_t *info)
{
    return ieee802154_radio_read(&submac->dev, buf, len, info);
}

/**
 * @brief Set the SubMAC to IDLE state.
 *
 * Frames won't be received in this state. However, it's still possible to send
 * frames.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 *
 * @return success or error code.
 * @retval 0 on success
 * @retval -EBUSY if the SubMAC is currently busy
 */
int ieee802154_set_idle(ieee802154_submac_t *submac);

/**
 * @brief Set the SubMAC to RX state
 *
 * During this state the SubMAC accepts incoming frames.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 *
 * @return success or error code.
 * @retval 0 on success
 * @retval -EBUSY if the SubMAC is currently busy
 */
int ieee802154_set_rx(ieee802154_submac_t *submac);

/**
 * @brief Check whether the SubMAC is in RX state
 *
 * @param[in] submac pointer to the SubMAC descriptor
 *
 * @retval true if the SubMAC is in RX state
 * @retval false otherwise
 */
bool ieee802154_submac_state_is_rx(ieee802154_submac_t *submac);

/**
 * @brief Check whether the SubMAC is in IDLE state
 *
 * @param[in] submac pointer to the SubMAC descriptor
 *
 * @retval true if the SubMAC is in IDLE state
 * @retval false otherwise
 */
bool ieee802154_submac_state_is_idle(ieee802154_submac_t *submac);

/**
 * @brief Init the IEEE 802.15.4 SubMAC
 *
 * The SubMAC state machine starts in RX state.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] short_addr pointer to the IEEE 802.15.4 short address
 * @param[in] ext_addr pointer to the IEEE 802.15.4 extended address
 *
 * @return 0 on success
 * @return negative errno on error
 */
int ieee802154_submac_init(ieee802154_submac_t *submac, const network_uint16_t *short_addr,
                           const eui64_t *ext_addr);

/**
 * @brief Set the ACK timeout timer
 *
 * @note This function should be implemented by the user of the SubMAC.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
extern void ieee802154_submac_ack_timer_set(ieee802154_submac_t *submac);

/**
 * @brief Cancel the ACK timeout timer
 *
 * @note This function should be implemented by the user of the SubMAC.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
extern void ieee802154_submac_ack_timer_cancel(ieee802154_submac_t *submac);

/**
 * @brief Process an FSM event
 *
 * @internal
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] ev the event to be processed
 *
 * @return  Next FSM event
 */
int ieee802154_submac_process_ev(ieee802154_submac_t *submac,
                                 ieee802154_fsm_ev_t ev, void *ctx);

/**
 * @brief Indicate the SubMAC that the ACK timeout fired.
 *
 * This function must be called when the ACK timeout timer fires.
 *
 * @note this function should not be called inside ISR context and MUST NOT
 *       be invoked if @ref ieee802154_submac_ack_timer_cancel was already
 *       called.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
static inline void ieee802154_submac_ack_timeout_fired(ieee802154_submac_t *submac)
{
    ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_ACK_TIMEOUT, 0);
}

/**
 * @brief Indicate the SubMAC that the device received a frame.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
static inline void ieee802154_submac_rx_done_cb(ieee802154_submac_t *submac)
{
    ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_RX_DONE, 0);
}

/**
 * @brief Indicate the SubMAC that a frame with invalid CRC was received.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
static inline void ieee802154_submac_crc_error_cb(ieee802154_submac_t *submac)
{
    ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_CRC_ERROR, 0);
}

/**
 * @brief Indicate the SubMAC that the device finished the transmission procedure.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
static inline void ieee802154_submac_tx_done_cb(ieee802154_submac_t *submac)
{
    ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_TX_DONE, 0);
}

#ifdef __cplusplus
}
#endif

/** @} */
