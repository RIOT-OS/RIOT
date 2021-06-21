/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

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
 * @{
 *
 * @author       José I. Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef NET_IEEE802154_SUBMAC_H
#define NET_IEEE802154_SUBMAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#define IEEE802154_SUBMAC_MAX_RETRANSMISSIONS (4U)  /**< maximum number of frame retransmissions */

/**
 * @brief IEEE 802.15.4 SubMAC forward declaration
 */
typedef struct ieee802154_submac ieee802154_submac_t;

/**
 * @brief SubMAC states
 */
typedef enum {
    /**
     * @brief SubMAC and network devices are off.
     *
     * The corresponding network device is put in a state with the
     * lowest energy consumption.
     */
    IEEE802154_STATE_OFF,

    /**
     * @brief SubMAC is ready to be used.
     */
    IEEE802154_STATE_IDLE,

    /**
     * @brief SubMAC is ready to be used and listening to incoming frames.
     */
    IEEE802154_STATE_LISTEN,
} ieee802154_submac_state_t;

/**
 * @brief IEEE 802.15.4 SubMAC callbacks.
 */
typedef struct {
    /**
     * @brief RX done event
     *
     * This function is called from the SubMAC to indicate a IEEE 802.15.4
     * frame is ready to be fetched from the device.
     *
     * @post If @ref ieee802154_submac_t::state is @ref IEEE802154_STATE_LISTEN, the
     * SubMAC is ready to receive frames
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
     * @pre If @ref ieee802154_submac_t::state is @ref IEEE802154_STATE_LISTEN, the
     * SubMAC is ready to receive frames.
     *
     * @param[in] submac pointer to the SubMAC descriptor
     * @param[out] info TX information associated to the transmission (status,
     *                 number of retransmissions, pending bit, etc).
     */
    void (*tx_done)(ieee802154_submac_t *submac, int status,
                    ieee802154_tx_info_t *info);
} ieee802154_submac_cb_t;

/**
 * @brief IEEE 802.15.4 SubMAC descriptor
 */
struct ieee802154_submac {
    eui64_t ext_addr;                   /**< IEEE 802.15.4 extended address */
    network_uint16_t short_addr;        /**< IEEE 802.15.4 short address */
    ieee802154_dev_t *dev;              /**< pointer to the 802.15.4 HAL descriptor */
    const ieee802154_submac_cb_t *cb;   /**< pointer to the SubMAC callbacks */
    ieee802154_csma_be_t be;            /**< CSMA-CA backoff exponent params */
    bool wait_for_ack;                  /**< SubMAC is waiting for an ACK frame */
    bool tx;                            /**< SubMAC is currently transmitting a frame */
    uint16_t panid;                     /**< IEEE 802.15.4 PAN ID */
    uint16_t channel_num;               /**< IEEE 802.15.4 channel number */
    uint8_t channel_page;               /**< IEEE 802.15.4 channel page */
    uint8_t retrans;                    /**< current number of retransmissions */
    uint8_t csma_retries_nb;            /**< current number of CSMA-CA retries */
    uint8_t backoff_mask;               /**< internal value used for random backoff calculation */
    uint8_t csma_retries;               /**< maximum number of CSMA-CA retries */
    int8_t tx_pow;                      /**< Transmission power (in dBm) */
    ieee802154_submac_state_t state;    /**< State of the SubMAC */
    ieee802154_phy_mode_t phy_mode;     /**< IEEE 802.15.4 PHY mode */
};

/**
 * @brief Get the internal state of the SubMAC
 *
 * @param[in] submac pointer to the SubMAC descriptor
 *
 * @return the SubMAC state
 */
static inline ieee802154_submac_state_t ieee802154_get_state(ieee802154_submac_t *submac)
{
    return submac->state;
}

/**
 * @brief Set the internal state of the SubMAC
 *
 * @param[in] submac pointer to the SubMAC descriptor
 * @param[in] state the desired state
 *
 * @return 0 on success
 * @return negative errno on error.
 */
int ieee802154_set_state(ieee802154_submac_t *submac, ieee802154_submac_state_t state);

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
 * @return negative errno on error
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

    int res = ieee802154_radio_config_addr_filter(submac->dev, IEEE802154_AF_SHORT_ADDR, short_addr);

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
    int res = ieee802154_radio_config_addr_filter(submac->dev, IEEE802154_AF_EXT_ADDR, ext_addr);

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
    int res = ieee802154_radio_config_addr_filter(submac->dev, IEEE802154_AF_PANID, panid);

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
 * @param[in] channel_num channel number
 * @param[in] channel_page channel page
 * @param[in] tx_pow transmission power (in dBm)
 *
 * @return 0 on success
 * @return -ENOTSUP if the PHY settings are not supported
 * @return negative errno on error
 */
int ieee802154_set_phy_conf(ieee802154_submac_t *submac, uint16_t channel_num,
                            uint8_t channel_page, int8_t tx_pow);

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
 * @return negative errno on error
 */
static inline int ieee802154_set_channel_number(ieee802154_submac_t *submac,
                                                uint16_t channel_num)
{
    return ieee802154_set_phy_conf(submac, channel_num, submac->channel_page,
                                   submac->tx_pow);
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
 * @return negative errno on error
 */
static inline int ieee802154_set_channel_page(ieee802154_submac_t *submac,
                                              uint16_t channel_page)
{
    return ieee802154_set_phy_conf(submac, submac->channel_num, channel_page,
                                   submac->tx_pow);
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
 * @return negative errno on error
 */
static inline int ieee802154_set_tx_power(ieee802154_submac_t *submac,
                                          int8_t tx_pow)
{
    return ieee802154_set_phy_conf(submac, submac->channel_num,
                                   submac->channel_page, tx_pow);
}

/**
 * @brief Get the received frame length
 *
 * @param[in] submac pointer to the SubMAC
 *
 * @return length of the PSDU (excluding FCS length)
 */
static inline int ieee802154_get_frame_length(ieee802154_submac_t *submac)
{
    return ieee802154_radio_len(submac->dev);
}

/**
 * @brief Read the received frame
 *
 * This functions reads the received PSDU from the device (excluding FCS)
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
    return ieee802154_radio_read(submac->dev, buf, len, info);
}

/**
 * @brief Init the IEEE 802.15.4 SubMAC
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
 * @param[in] us microseconds until the ACK timeout timer is fired
 */
extern void ieee802154_submac_ack_timer_set(ieee802154_submac_t *submac,
                                            uint16_t us);

/**
 * @brief Cancel the ACK timeout timer
 *
 * @note This function should be implemented by the user of the SubMAC.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
extern void ieee802154_submac_ack_timer_cancel(ieee802154_submac_t *submac);

/**
 * @brief Indicate the SubMAC that the ACK timeout fired.
 *
 * This function must be called when the ACK timeout timer fires.
 *
 * @note this function should not be called inside ISR context.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
void ieee802154_submac_ack_timeout_fired(ieee802154_submac_t *submac);

/**
 * @brief Indicate the SubMAC that the device received a frame.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
void ieee802154_submac_rx_done_cb(ieee802154_submac_t *submac);

/**
 * @brief Indicate the SubMAC that a frame with invalid CRC was received.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
void ieee802154_submac_crc_error_cb(ieee802154_submac_t *submac);

/**
 * @brief Indicate the SubMAC that the device finished the transmission procedure.
 *
 * @param[in] submac pointer to the SubMAC descriptor
 */
void ieee802154_submac_tx_done_cb(ieee802154_submac_t *submac);

#ifdef __cplusplus
}
#endif

#endif /* NET_IEEE802154_SUBMAC_H */
/** @} */
