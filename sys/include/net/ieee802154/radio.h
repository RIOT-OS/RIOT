/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup     drivers_ieee802154_hal IEEE802.15.4 Radio Hardware Abstraction Layer
 * @ingroup      drivers
 * @experimental This API is experimental and in an early state - expect
 *               changes!

 * @brief        This is a Hardware Abstraction Layer for IEEE802.15.4 compatible
 *               radios.
 * @{
 *
 * @author       José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef NET_IEEE802154_RADIO_H
#define NET_IEEE802154_RADIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "iolist.h"
#include "sys/uio.h"
#include "byteorder.h"
#include "net/eui64.h"

/**
 * @brief Forward declaration of the radio ops structure.
 */
typedef struct ieee802154_radio_ops ieee802154_radio_ops_t;

/**
 * @brief IEEE802.15.4 Radio capabilities
 *
 * These flags represent the hardware capabilities of a given device.
 */
typedef enum {
    /**
     * @brief the device supports frame retransmissions with CSMA-CA
     *
     * The device supports sending with CSMA-CA and retransmissions.  If the
     * CSMA-CA fails, the device reports a @ref TX_STATUS_MEDIUM_BUSY when
     * calling @ref ieee802154_radio_ops::confirm_transmit. In case CSMA-CA
     * succeeds and the ACK frame is expected, the
     * device reports a @ref TX_STATUS_SUCCESS if the ACK frame is received
     * during any retransmission attempt. Otherwise, it reports a @ref
     * TX_STATUS_NO_ACK
     *
     * ACK frames are not indicated to the upper layer.
     *
     * @note it's implicit that a radio supports @ref
     * IEEE802154_CAP_AUTO_CSMA if this cap is available
     */
    IEEE802154_CAP_FRAME_RETRANS,
    /**
     * @brief the device supports Auto CSMA-CA
     *
     * The device supports performing CSMA-CA before transmitting a frame.  If
     * CSMA-CA procedure succeeds, the device sends the frame and reports a
     * @ref TX_STATUS_SUCCESS when calling @ref
     * ieee802154_radio_ops::confirm_transmit. If it fails, the device reports
     * @ref TX_STATUS_MEDIUM_BUSY.
     */
    IEEE802154_CAP_AUTO_CSMA,
    /**
     * @brief the device support ACK timeout interrupt
     *
     * The device will automatically attempt to receive and handle the ACK
     * frame if expected.
     * If the ACK frame is not received, the device reports @ref
     * TX_STATUS_NO_ACK when calling @ref ieee802154_radio_ops::confirm_transmit.
     * Otherwise, it reports @ref TX_STATUS_SUCCESS.
     *
     * The ACK frame is not indicated to the upper layer.
     */
    IEEE802154_CAP_IRQ_ACK_TIMEOUT,
    /**
     * @brief the device supports the IEEE802.15.4 2.4 GHz band
     *
     * It's assumed that @ref IEEE802154_CAP_IRQ_TX_DONE is present.
     */
    IEEE802154_CAP_24_GHZ,
    /**
     * @brief the device support the IEEE802.15.4 Sub GHz band
     */
    IEEE802154_CAP_SUB_GHZ,
    /**
     * @brief the device reports when the transmission is done
     */
    IEEE802154_CAP_IRQ_TX_DONE,
    /**
     * @brief the device reports the start of a frame (SFD) when received.
     */
    IEEE802154_CAP_IRQ_RX_START,
    /**
     * @brief the device reports the end of the CCA procedure
     */
    IEEE802154_CAP_IRQ_CCA_DONE,
    /**
     * @brief the device provides the number of retransmissions
     *
     * It's assumed that @ref IEEE802154_CAP_FRAME_RETRANS is present.
     */
    IEEE802154_CAP_FRAME_RETRANS_INFO,
    /**
     * @brief the device retains all register values when off.
     */
    IEEE802154_CAP_REG_RETENTION,
} ieee802154_rf_caps_t;

/**
 * @brief Transmission status
 */
typedef enum {
    /**
     * @brief the transceiver successfully sent a frame.
     *
     * Depending of the type of transmissions and available caps, this could
     * mean one of the following:
     *
     * If the device supports @ref IEEE802154_CAP_FRAME_RETRANS or
     * @ref IEEE802154_CAP_IRQ_ACK_TIMEOUT this means either:
     * - The frame was sent without ACK Req bit
     * - The frame was sent with the ACK Req bit and a valid ACK was received.
     *
     * Otherwise, this notifies that a frame was sent.
     */
    TX_STATUS_SUCCESS,
    /**
     * @brief the transceiver received a valid ACK with the frame pending bit
     *
     * This status is present only if the device supports @ref
     * IEEE802154_CAP_FRAME_RETRANS or @ref IEEE802154_CAP_IRQ_ACK_TIMEOUT.
     */
    TX_STATUS_FRAME_PENDING,
    /**
     * @brief the transceiver ran out of retransmission
     *
     * This status is present only if the device supports @ref
     * IEEE802154_CAP_FRAME_RETRANS or @ref IEEE802154_CAP_IRQ_ACK_TIMEOUT.
     */
    TX_STATUS_NO_ACK,
    /**
     * @brief the CSMA-CA algorithm or CCA failed to measure a clear channel
     */
    TX_STATUS_MEDIUM_BUSY,
} ieee802154_tx_status_t;

/**
 * @brief IEEE802.15.4 transceiver states (not to confuse with device states)
 */
typedef enum {
    /**
     * @brief the transceiver state is off
     */
    IEEE802154_TRX_STATE_TRX_OFF,
    /**
     * @brief the transceiver is ready to receive/receiving frames
     */
    IEEE802154_TRX_STATE_RX_ON,
    /**
     * @brief the transceiver is ready to transmit/transmitting a frame
     */
    IEEE802154_TRX_STATE_TX_ON,
} ieee802154_trx_state_t;

/**
 * @brief IEEE802.15.4 Radio HAL events
 *
 * To follow the IEEE802.15.4 convention, an event that responds to a Request
 * is a confirmation (Confirm). Otherwise an Indication.
 */
typedef enum {
    /**
     * @brief the transceiver detected a valid SFD
     *
     * This event is present if radio has @ref IEEE802154_CAP_IRQ_RX_START cap.
     */
    IEEE802154_RADIO_INDICATION_RX_START,

    /**
     * @brief the transceiver received a frame and lies in the
     *        internal framebuffer.
     *
     * This indication should be generated only if CRC is valid and the frame
     * passes the address matching filter (this includes ACK and Beacon frames).
     * The latter only applies if the radio is not in promiscuous mode.
     *
     * The transceiver or driver MUST handle the ACK reply if the Ack Request
     * bit is set in the received frame and promiscuous mode is disabled.
     *
     * The transceiver is in @ref IEEE802154_TRX_STATE_RX_ON state when
     * this function is called, but with framebuffer protection (either
     * dynamic framebuffer protection or disabled RX). Thus, the frame
     * won't be overwritten before calling the @ref ieee802154_radio_indication_rx
     * function. However, @ref ieee802154_radio_indication_rx MUST be called in
     * order to receive new frames. If there's no interest in the
     * frame, the function can be called with a NULL buffer to drop
     * the frame.
     */
    IEEE802154_RADIO_INDICATION_RX_DONE,

    /**
     * @brief the transceiver either finished sending a frame, the retransmission
     *        procedure or the channel activity detection prior transmission.
     *
     * This event is present if radio has @ref IEEE802154_CAP_IRQ_TX_DONE cap.
     * The upper layer should immediately call @ref
     * ieee802154_radio_ops::confirm_transmit when on this event.
     */
    IEEE802154_RADIO_CONFIRM_TX_DONE,
    /**
     * @brief the CCA procedure finished
     *
     * This event is present if radio has @ref IEEE802154_CAP_IRQ_CCA_DONE.
     */
    IEEE802154_RADIO_CONFIRM_CCA,
} ieee802154_trx_ev_t;

/**
 * @brief CSMA-CA exponential backoff parameters.
 */
typedef struct {
    uint8_t min;    /**< minimum value of the exponential backoff */
    uint8_t max;    /**< maximum value of the exponential backoff */
} ieee802154_csma_be_t;

/**
 * @brief RX information associated to a frame
 */
typedef struct {
    /**
     * @brief RSSI of the received frame.
     *
     * The RSSI is a measure of the RF power in dBm for the received frame.
     * The minimum and maximum values are 0 (-174 dBm) and 254 (80 dBm).
     */
    uint8_t rssi;
    uint8_t lqi;    /**< LQI of the received frame */
} ieee802154_rx_info_t;

/**
 * @brief TX information of the last transmitted frame.
 */
typedef struct {
    ieee802154_tx_status_t status;      /**< status of the last transmission */
    uint8_t retrans;                    /**< number of frame retransmissions of the last TX */
} ieee802154_tx_info_t;

/**
 * @brief Forward declaration of the IEEE802.15.4 device descriptor
 */
typedef struct ieee802154_dev ieee802154_dev_t;

/**
 * @brief Prototype of the IEEE802.15.4 device event callback
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] status the status
 */
typedef void (*ieee802154_cb_t)(ieee802154_dev_t *dev,
                                ieee802154_trx_ev_t status);

/**
 * @brief the IEEE802.15.4 device descriptor
 */
struct ieee802154_dev {
    /**
     * @brief pointer to the operations of the device
     */
    const ieee802154_radio_ops_t *driver;
    /**
     * @brief pointer to the context of the device
     */
    void *ctx;
    /**
     * @brief the event callback of the device
     */
    ieee802154_cb_t cb;
};

/**
 * @brief IEEE802.15.4 CCA modes
 */
typedef enum {
    /**
     * @brief CCA using first mode (energy detection)
     */
    IEEE802154_CCA_MODE_ED_THRESHOLD,
    /**
     * @brief CCA using second mode (carrier sensing)
     */
    IEEE802154_CCA_MODE_CARRIER_SENSING,
    /**
     * @brief CCA using third mode (energy detection AND carrier sensing)
     */
    IEEE802154_CCA_MODE_ED_THRESH_AND_CS,
    /**
     * @brief CCA using third mode (energy detection OR carrier sensing)
     */
    IEEE802154_CCA_MODE_ED_THRESH_OR_CS,
} ieee802154_cca_mode_t;

/**
 * @brief RX mode configuration
 */
typedef enum {
    /**
     * @brief Auto ACK is disabled
     */
    IEEE802154_RX_AACK_DISABLED,
    /**
     * @brief Auto ACK is enabled
     */
    IEEE802154_RX_AACK_ENABLED,
    /**
     * @brief Auto ACK is enabled and frame pending bit set in the next ACK frame
     */
    IEEE802154_RX_AACK_FRAME_PENDING,
    /**
     * @brief Radio is in promiscuous mode
     */
    IEEE802154_RX_PROMISC,
    /**
     * @brief Radio is ready to receive ACK frames
     *
     * This mode is optional. If a radio decides to implement it, the radio
     * should allow ACK frames (and block ACK frames in all other RX modes).
     * Note that this mode cannot guarantee that only ACK frames will be
     * received.
     *
     * Expected to be implemented when either @ref IEEE802154_CAP_FRAME_RETRANS
     * or @ref IEEE802154_CAP_IRQ_ACK_TIMEOUT is not there.
     */
    IEEE802154_RX_WAIT_FOR_ACK,
} ieee802154_rx_mode_t;

/**
 * @brief Holder of the PHY configuration
 */
typedef struct {
    uint16_t channel;   /**< IEEE802.15.4 channel number */
    uint8_t page;       /**< IEEE802.15.4 channel page */
    int8_t pow;         /**< TX power in dBm */
} ieee802154_phy_conf_t;

/**
 * @brief Radio ops struct declaration
 */
struct ieee802154_radio_ops {
    /**
     * @brief Write a frame into the framebuffer.
     *
     * This function shouldn't do any checks, so the frame MUST be valid.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] psdu PSDU frame to be sent
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*write)(ieee802154_dev_t *dev, const iolist_t *psdu);

    /**
     * @brief Request the transmission of a preloaded frame
     *
     * @ref ieee802154_radio_ops::confirm_transmit MUST be used to finish the
     * transmission.
     *
     * @pre the PHY state is @ref IEEE802154_TRX_STATE_TX_ON and the frame
     *      is already in the framebuffer.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*request_transmit)(ieee802154_dev_t *dev);

    /**
     * @brief Confirmation function for @ref ieee802154_radio_ops::request_transmit.
     *
     * This function must be called to finish the transmission procedure and
     * get the transmission status. This function should be called on @ref
     * IEEE802154_RADIO_CONFIRM_TX_DONE. If no interrupt is available, this
     * function can be polled.
     *
     * @pre the device is on
     * @pre call to @ref ieee802154_radio_ops::request_transmit was successful.
     *
     * @post the state is @ref IEEE802154_TRX_STATE_TX_ON.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[out] info the TX information. Pass NULL
     * if the information is not needed. If the radio supports AutoCCA, the
     * status should indicate transmission done or channel busy. If the radio
     * supports frame retransmissions, the status should indicate if medium
     * was busy, no ACK was received or transmission succeeded.
     *
     * @return 0 on success
     * @return -EAGAIN if the transmission has not finished yet.
     * @return negative errno on error
     */
    int (*confirm_transmit)(ieee802154_dev_t *dev, ieee802154_tx_info_t *info);

    /**
     * @brief Get the length of the received PSDU frame.
     *
     * @pre the device is on
     * @pre the radio already received a frame (e.g
     *      @ref ieee802154_dev::cb with @ref IEEE802154_RADIO_INDICATION_RX_DONE).
     *
     * @post the frame buffer is still protected against new frame arrivals.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return length of the frame
     */
    int (*len)(ieee802154_dev_t *dev);

    /**
     * @brief Process the RX done indication
     *
     * This function reads the received frame from the internal framebuffer.
     * It should try to copy the received frame into @p buf and
     * then unlock the framebuffer (in order to be able to receive more
     * frames).
     *
     * @pre the device is on and an @ref IEEE802154_RADIO_INDICATION_RX_DONE
     *      event was issued.
     *
     * @post the state is @ref IEEE802154_TRX_STATE_RX_ON
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[out] buf buffer to write the received PSDU frame into. If NULL,
     *             the frame is not copied.
     * @param[in] size size of @p buf
     * @param[in] info information of the received frame (LQI, RSSI). Can be
     *            NULL if this information is not needed.
     *
     * @return number of bytes written in @p buffer (0 if @p buf == NULL)
     * @return -ENOBUFS if the frame doesn't fit in @p
     */
    int (*indication_rx)(ieee802154_dev_t *dev, void *buf, size_t size,
                         ieee802154_rx_info_t *info);

    /**
     * @brief Turn off the device
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * When this function returns, the radio shall be off.
     *
     * @post the device is off (and thus, the transceiver state is @ref
     * IEEE802154_TRX_STATE_TRX_OFF)
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*off)(ieee802154_dev_t *dev);

    /**
     * @brief Request to turn on the device
     *
     * @note @ref ieee802154_radio_ops::confirm_on MUST be used to finish the
     * procedure.
     *
     * @pre the init function of the radio succeeded.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*request_on)(ieee802154_dev_t *dev);

    /**
     * @brief Confirmation function for @ref ieee802154_radio_ops::request_on.
     *
     * @pre call to @ref ieee802154_radio_ops::request_on was successful.
     *
     * @post the transceiver state is @ref IEEE802154_TRX_STATE_TRX_OFF
     * During boot or in case the radio doesn't support @ref
     * IEEE802154_CAP_REG_RETENTION when @ref off was called, the
     * Physical Information Base will be undefined. Thus, take into
     * consideration that the following functions should be called right after
     * the radio is turned on again:
     * - @ref set_cca_threshold
     * - @ref set_cca_mode
     * - @ref config_phy
     * - @ref set_csma_params
     * - @ref set_rx_mode
     * - @ref set_hw_addr_filter
     * - @ref set_frame_retrans (if available)
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return 0 if the device is on
     * @return -EAGAIN if the device is still busy turning on
     * @return negative errno on error
     */
    int (*confirm_on)(ieee802154_dev_t *dev);

    /**
     * @brief Request a PHY state change
     *
     * @note @ref ieee802154_radio_ops::confirm_set_trx_state MUST be used to
     * finish the state transition.
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] state the new state
     *
     * @return 0 on success
     * @return -EBUSY if the transceiver is busy
     * @return negative number on error
     */
    int (*request_set_trx_state)(ieee802154_dev_t *dev,
                                 ieee802154_trx_state_t state);

    /**
     * @brief Confirmation function for @ref
     * ieee802154_radio_ops::request_set_trx_state
     *
     * @pre call to @ref ieee802154_radio_ops::request_set_trx_state was
     * successful.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return 0 if the state transition was successful
     * @return -EAGAIN if the transition has not finished yet
     * @return negative errno on error
     */
    int (*confirm_set_trx_state)(ieee802154_dev_t *dev);

    /**
     * @brief Request Stand-Alone Clear Channel Assessment
     *
     * @pre the state is @ref IEEE802154_TRX_STATE_RX_ON
     *
     * @note @ref ieee802154_radio_ops::confirm_cca MUST be used to
     * finish the CCA procedure and get the channel status.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @post the state is @ref IEEE802154_TRX_STATE_RX_ON
     *
     * @return 0 if request was OK
     * @return -EAGAIN if the request cannot be performed immediately.
     * @return negative errno on error
     */
    int (*request_cca)(ieee802154_dev_t *dev);

    /**
     * @brief Confirmation function for @ref ieee802154_radio_ops::request_cca
     *
     * This function must be called to finish the CCA procedure.  This
     * function should be called on @ref IEEE802154_RADIO_CONFIRM_CCA,
     * If no interrupt is available, this function can be polled.
     *
     * @pre call to @ref ieee802154_radio_ops::request_cca was successful.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return positive number if the channel is clear
     * @return 0 if the channel is busy
     * @return -EAGAIN if the CCA procedure hasn't finished.
     * @return negative errno on error
     */
    int (*confirm_cca)(ieee802154_dev_t *dev);

    /**
     * @brief Get a cap from the radio
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param cap cap to be checked
     *
     * @return true if the radio supports the cap
     * @return false otherwise
     */
    bool (*get_cap)(ieee802154_dev_t *dev, ieee802154_rf_caps_t cap);

    /**
     * @brief Set the threshold for the Energy Detection (first mode of CCA)
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] threshold the threshold in dBm.
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_cca_threshold)(ieee802154_dev_t *dev, int8_t threshold);

    /**
     * @brief Set CCA mode
     *
     * All radios MUST at least implement the first CCA mode (ED Threshold).
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] mode the CCA mode
     *
     * @return 0 on success
     * @return -ENOTSUP if the mode is not supported
     * @return negative errno on error
     */
    int (*set_cca_mode)(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode);

    /**
     * @brief Set IEEE802.15.4 PHY configuration (channel, TX power)
     *
     * This function SHOULD NOT validate the PHY configurations unless
     * it's specific to the device. The upper layer is responsible of all kind
     * of validations.
     * In case a configuration is not valid (e.g parameters out of range), this
     * function should return -EINVAL
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] conf the PHY configuration
     *
     * @return 0 on success
     * @return -EINVAL if the configuration is not valid for the device.
     * @return negative errno on error
     */
    int (*config_phy)(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf);

    /**
     * @brief Set IEEE802.15.4 addresses in hardware address filter
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] short_addr the IEEE802.15.4 short address. If NULL, the short
     *            address is not altered..
     * @param[in] ext_addr the IEEE802.15.4 extended address (Network Byte Order).
     *            If NULL, the extended address is not altered.
     * @param[in] pan_id the IEEE802.15.4 PAN ID. If NULL, the PAN ID is not altered.
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_hw_addr_filter)(ieee802154_dev_t *dev,
                              const network_uint16_t *short_addr,
                              const eui64_t *ext_addr,
                              const uint16_t *pan_id);

    /**
     * @brief Set number of frame retransmissions
     *
     * @pre the device is on
     *
     * @note this function pointer can be NULL if the device doesn't support
     *       frame retransmissions
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] retrans the number of retransmissions attempts.
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_frame_retrans)(ieee802154_dev_t *dev, uint8_t retrans);

    /**
     * @brief Set the CSMA-CA parameters.
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] bd parameters of the exponential backoff. If NULL, the
     *               parameters are not altered.
     * @param[in] retries number of CSMA-CA retries. If @p retries < 0,
     *                    retransmissions with CSMA-CA MUST be disabled.
     *                    If @p retries == 0, the @ref
     *                    ieee802154_radio_ops::request_transmit function is
     *                    equivalent to CCA send.
     *
     * @return 0 on success
     * @return -EINVAL if the settings are not supported.
     * @return negative errno on error
     */
    int (*set_csma_params)(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd,
                           int8_t retries);

    /**
     * @brief Set the RX mode.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] mode RX mode
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_rx_mode)(ieee802154_dev_t *dev, ieee802154_rx_mode_t mode);
};

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::write
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] psdu PSDU frame to be sent
 *
 * @return result of @ref ieee802154_radio_ops::write
 */
static inline int ieee802154_radio_write(ieee802154_dev_t *dev, const iolist_t *psdu)
{
    return dev->driver->write(dev, psdu);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::request_transmit
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of @ref ieee802154_radio_ops::request_transmit
 */
static inline int ieee802154_radio_request_transmit(ieee802154_dev_t *dev)
{
    return dev->driver->request_transmit(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::confirm_transmit
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[out] info the TX information
 *
 * @return result of @ref ieee802154_radio_ops::confirm_transmit
 */
static inline int ieee802154_radio_confirm_transmit(ieee802154_dev_t *dev,
                                                    ieee802154_tx_info_t *info)
{
    return dev->driver->confirm_transmit(dev, info);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::len
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of @ref ieee802154_radio_ops::len
 */
static inline int ieee802154_radio_len(ieee802154_dev_t *dev)
{
    return dev->driver->len(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::indication_rx
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[out] buf buffer to write the received frame into. If NULL, the
 *             frame is not copied.
 * @param[in] size size of @p buf
 * @param[in] info information of the received frame (LQI, RSSI). Can be
 *            NULL if this information is not needed.
 *
 * @return result of @ref ieee802154_radio_ops::indication_rx
 */
static inline int ieee802154_radio_indication_rx(ieee802154_dev_t *dev,
                                                 void *buf,
                                                 size_t size,
                                                 ieee802154_rx_info_t *info)
{
    return dev->driver->indication_rx(dev, buf, size, info);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::set_cca_threshold
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] threshold the threshold in dBm
 *
 * @return result of @ref ieee802154_radio_ops::set_cca_threshold
 */
static inline int ieee802154_radio_set_cca_threshold(ieee802154_dev_t *dev,
                                                     int8_t threshold)
{
    return dev->driver->set_cca_threshold(dev, threshold);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::set_cca_mode
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] mode the CCA mode
 *
 * @return result of @ref ieee802154_radio_ops::set_cca_mode
 */
static inline int ieee802154_radio_set_cca_mode(ieee802154_dev_t *dev,
                                                ieee802154_cca_mode_t mode)
{
    return dev->driver->set_cca_mode(dev, mode);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::config_phy
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] conf the PHY configuration
 *
 * @return result of @ref ieee802154_radio_ops::config_phy
 */
static inline int ieee802154_radio_config_phy(ieee802154_dev_t *dev,
                                              const ieee802154_phy_conf_t *conf)
{
    return dev->driver->config_phy(dev, conf);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::off
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @post the transceiver state is @ref IEEE802154_TRX_STATE_TRX_OFF
 *
 * @return result of @ref ieee802154_radio_ops::off
 */
static inline int ieee802154_radio_off(ieee802154_dev_t *dev)
{
    return dev->driver->off(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::set_hw_addr_filter
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] short_addr the IEEE802.15.4 short address. If NULL, the short
 *            address is not altered..
 * @param[in] ext_addr the IEEE802.15.4 extended address (Network Byte Order).
 *            If NULL, the extended address is not altered.
 * @param[in] pan_id the IEEE802.15.4 PAN ID. If NULL, the PAN ID is not altered.
 *
 * @return result of @ref ieee802154_radio_ops::set_hw_addr_filter
 */
static inline int ieee802154_radio_set_hw_addr_filter(ieee802154_dev_t *dev,
                                                      const network_uint16_t *short_addr,
                                                      const eui64_t *ext_addr,
                                                      const uint16_t *pan_id)
{
    return dev->driver->set_hw_addr_filter(dev, short_addr, ext_addr, pan_id);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::set_frame_retrans
 *
 * @pre the device is on
 * @pre the device supports frame retransmissions
 *      (@ref ieee802154_radio_has_frame_retrans() == true)
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] retrans the number of retransmissions
 *
 * @return result of @ref ieee802154_radio_ops::set_frame_retrans
 */
static inline int ieee802154_radio_set_frame_retrans(ieee802154_dev_t *dev,
                                                     uint8_t retrans)
{
    return dev->driver->set_frame_retrans(dev, retrans);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::set_csma_params
 *
 * @pre the device is on
 * @pre the device supports frame retransmissions
 *      (@ref ieee802154_radio_has_frame_retrans() == true)
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] bd parameters of the exponential backoff
 * @param[in] retries number of CSMA-CA retries. If @p restries < 0,
 *                    retransmissions with CSMA-CA are disabled
 *
 * @return result of @ref ieee802154_radio_ops::set_csma_params
 */
static inline int ieee802154_radio_set_csma_params(ieee802154_dev_t *dev,
                                                   const ieee802154_csma_be_t *bd,
                                                   int8_t retries)
{
    return dev->driver->set_csma_params(dev, bd, retries);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::request_on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of @ref ieee802154_radio_ops::request_on
 */
static inline int ieee802154_radio_request_on(ieee802154_dev_t *dev)
{
    return dev->driver->request_on(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::confirm_on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of @ref ieee802154_radio_ops::confirm_on
 */
static inline int ieee802154_radio_confirm_on(ieee802154_dev_t *dev)
{
    return dev->driver->confirm_on(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::request_set_trx_state
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] state the new state
 *
 * @return result of @ref ieee802154_radio_ops::request_set_trx_state
 */
static inline int ieee802154_radio_request_set_trx_state(ieee802154_dev_t *dev,
                                                         ieee802154_trx_state_t state)
{
    return dev->driver->request_set_trx_state(dev, state);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::confirm_set_trx_state
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of @ref ieee802154_radio_ops::confirm_set_trx_state
 */
static inline int ieee802154_radio_confirm_set_trx_state(ieee802154_dev_t *dev)
{
    return dev->driver->confirm_set_trx_state(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::request_cca
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of @ref ieee802154_radio_ops::request_cca
 */
static inline int ieee802154_radio_request_cca(ieee802154_dev_t *dev)
{
    return dev->driver->request_cca(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::confirm_cca
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of @ref ieee802154_radio_ops::confirm_cca
 */
static inline int ieee802154_radio_confirm_cca(ieee802154_dev_t *dev)
{
    return dev->driver->confirm_cca(dev);
}

/**
 * @brief Check if the device supports ACK timeout
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_IRQ_ACK_TIMEOUT.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_ack_timeout(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_IRQ_ACK_TIMEOUT);
}

/**
 * @brief Check if the device supports frame retransmissions (with CSMA-CA).
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_FRAME_RETRANS.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_frame_retrans(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_FRAME_RETRANS);
}

/**
 * @brief Check if the device supports Auto CSMA-CA for transmissions.
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_AUTO_CSMA.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_auto_csma(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_AUTO_CSMA);
}

/**
 * @brief Check if the device supports the IEEE802.15.4 Sub-GHz band
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_SUB_GHZ.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_sub_ghz(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_SUB_GHZ);
}

/**
 * @brief Check if the device supports the IEEE802.15.4 2.4 GHz band
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_24_GHZ.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_24_ghz(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_24_GHZ);
}

/**
 * @brief Check if the device supports TX done interrupt
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_IRQ_TX_DONE.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_tx_done(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_IRQ_TX_DONE);
}

/**
 * @brief Check if the device supports RX start interrupt
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_IRQ_RX_START.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_rx_start(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_IRQ_RX_START);
}

/**
 * @brief Check if the device supports CCA done interrupt
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_IRQ_CCA_DONE.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_cca_done(ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_IRQ_CCA_DONE);
}

/**
 * @brief Check if the device reports the number of retransmissions of the last
 * TX procedure.
 *
 * Internally this function calls ieee802154_radio_ops::get_cap with @ref
 * IEEE802154_CAP_FRAME_RETRANS_INFO.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_frame_retrans_info(
    ieee802154_dev_t *dev)
{
    return dev->driver->get_cap(dev, IEEE802154_CAP_FRAME_RETRANS_INFO);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::set_rx_mode
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] mode RX mode
 *
 * @return result of @ref ieee802154_radio_ops::set_rx_mode
 */
static inline int ieee802154_radio_set_rx_mode(ieee802154_dev_t *dev,
                                               ieee802154_rx_mode_t mode)
{
    return dev->driver->set_rx_mode(dev, mode);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_IEEE802154_RADIO_H */
/** @} */
