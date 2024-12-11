/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "iolist.h"
#include "sys/uio.h"
#include "bitarithm.h"
#include "byteorder.h"
#include "net/eui64.h"
#include "net/ieee802154.h"
#include "errno.h"

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
     * calling @ref ieee802154_radio_confirm_transmit. In case CSMA-CA
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
    IEEE802154_CAP_FRAME_RETRANS        = BIT0,
    /**
     * @brief the device supports Auto CSMA-CA
     *
     * The device supports performing CSMA-CA before transmitting a frame.  If
     * CSMA-CA procedure succeeds, the device sends the frame and reports a
     * @ref TX_STATUS_SUCCESS when calling @ref
     * ieee802154_radio_confirm_transmit. If it fails, the device reports
     * @ref TX_STATUS_MEDIUM_BUSY.
     */
    IEEE802154_CAP_AUTO_CSMA            = BIT1,
    /**
     * @brief the device supports automatic ACK frame transmission
     *
     * The device automatically sends an ACK frame when it receives a frame
     * with the ACK Req bit set.
     */
    IEEE802154_CAP_AUTO_ACK             = BIT2,
    /**
     * @brief the device support ACK timeout interrupt
     *
     * The device will automatically attempt to receive and handle the ACK
     * frame if expected.
     * If the ACK frame is not received, the device reports @ref
     * TX_STATUS_NO_ACK when calling @ref ieee802154_radio_confirm_transmit.
     * Otherwise, it reports @ref TX_STATUS_SUCCESS.
     *
     * The ACK frame is not indicated to the upper layer.
     */
    IEEE802154_CAP_IRQ_ACK_TIMEOUT      = BIT3,
    /**
     * @brief the device supports the IEEE802.15.4 2.4 GHz band
     *
     * It's assumed that @ref IEEE802154_CAP_IRQ_TX_DONE is present.
     */
    IEEE802154_CAP_24_GHZ               = BIT4,
    /**
     * @brief the device support the IEEE802.15.4 Sub GHz band
     */
    IEEE802154_CAP_SUB_GHZ              = BIT5,
    /**
     * @brief the device reports reception off frames with invalid CRC.
     */
    IEEE802154_CAP_IRQ_CRC_ERROR        = BIT6,
    /**
     * @brief the device reports when the transmission is done
     */
    IEEE802154_CAP_IRQ_TX_DONE          = BIT7,
    /**
     * @brief the device reports the start of a frame (SFD) when received.
     */
    IEEE802154_CAP_IRQ_RX_START         = BIT8,
    /**
     * @brief the device reports the start of a frame (SFD) was sent.
     */
    IEEE802154_CAP_IRQ_TX_START         = BIT9,
    /**
     * @brief the device reports the end of the CCA procedure
     */
    IEEE802154_CAP_IRQ_CCA_DONE         = BIT10,
    /**
     * @brief the device provides the number of retransmissions
     *
     * It's assumed that @ref IEEE802154_CAP_FRAME_RETRANS is present.
     */
    IEEE802154_CAP_FRAME_RETRANS_INFO   = BIT11,
    /**
     * @brief the device retains all register values when off.
     */
    IEEE802154_CAP_REG_RETENTION        = BIT12,
    /**
     * @brief Binary Phase Shift Keying PHY mode
     */
    IEEE802154_CAP_PHY_BPSK             = BIT13,
    /**
     * @brief Amplitude-Shift Keying PHY mode
     */
    IEEE802154_CAP_PHY_ASK              = BIT14,
    /**
     * @brief Offset Quadrature Phase-Shift Keying
     */
    IEEE802154_CAP_PHY_OQPSK            = BIT15,
    /**
     * @brief Multi-Rate Offset Quadrature Phase-Shift Keying PHY mode
     */
    IEEE802154_CAP_PHY_MR_OQPSK         = BIT16,
    /**
     * @brief Multi-Rate Orthogonal Frequency-Division Multiplexing PHY mode
     */
    IEEE802154_CAP_PHY_MR_OFDM          = BIT17,
    /**
     * @brief Multi-Rate Frequency Shift Keying PHY mode
     */
    IEEE802154_CAP_PHY_MR_FSK           = BIT18,
    /**
     * @brief the device supports source address match table.
     *
     * A Source Address Match table contains source addresses with pending
     * data. When a coordinator device receives an IEEE 802.15.4 Data
     * Request command from a child node, the Frame Pending bit of the ACK is
     * set if the source address matches one from the table.
     */
    IEEE802154_CAP_SRC_ADDR_MATCH       = BIT19,
} ieee802154_rf_caps_t;

/**
 * @brief Bit-mask for PHY modes capabilities.
 */
#define IEEE802154_RF_CAPS_PHY_MASK \
    (IEEE802154_CAP_PHY_BPSK        \
    | IEEE802154_CAP_PHY_ASK        \
    | IEEE802154_CAP_PHY_OQPSK      \
    | IEEE802154_CAP_PHY_MR_OQPSK   \
    | IEEE802154_CAP_PHY_MR_OFDM    \
    | IEEE802154_CAP_PHY_MR_FSK)    \

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
     * @brief the transceiver received a frame with an invalid crc.
     *
     * @note some radios won't flush the framebuffer on reception of a frame
     * with invalid CRC. Therefore it's required to call @ref
     * ieee802154_radio_read.
     *
     * @note since the behavior of radios after frame reception is undefined,
     * the upper layer should set the transceiver state to IDLE as soon as
     * possible before calling @ref ieee802154_radio_read
     */
    IEEE802154_RADIO_INDICATION_CRC_ERROR,

    /**
     * @brief the transceiver sent out a valid SFD
     *
     * This event is present if radio has @ref IEEE802154_CAP_IRQ_TX_START cap.
     *
     * @note The SFD of an outgoing ACK (AUTOACK) should not be indicated
     */
    IEEE802154_RADIO_INDICATION_TX_START,

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
     * The transceiver might be in a "FB Lock" state where no more frames are
     * received. This is done in order to avoid overwriting the Frame Buffer
     * with new frame arrivals.  In order to leave this state, the upper layer
     * must call @ref ieee802154_radio_read
     *
     * @note since the behavior of radios after frame reception is undefined,
     * the upper layer should set the transceiver state to IDLE as soon as
     * possible before calling @ref ieee802154_radio_read
     */
    IEEE802154_RADIO_INDICATION_RX_DONE,

    /**
     * @brief the transceiver either finished sending a frame, the retransmission
     *        procedure or the channel activity detection prior transmission.
     *
     * This event is present if radio has @ref IEEE802154_CAP_IRQ_TX_DONE cap.
     * The upper layer should immediately call @ref
     * ieee802154_radio_confirm_transmit when on this event.
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
 * @brief Source Address Match commands.
 */
typedef enum {
    /**
     * @brief Enable or disable source address match.
     *
     * Enabling it sets the frame pending to all ACK frames in
     * response to a Data Request command (if the radio doesn't
     * support Source Address Matching) or to a specific address
     * in the Source Address Matching table
     */
    IEEE802154_SRC_MATCH_EN,
    /**
     * @brief Add a short address to entry.
     *
     * This command should only be implemented if @ref IEEE802154_CAP_SRC_ADDR_MATCH
     * is available.
     */
    IEEE802154_SRC_MATCH_SHORT_ADD,
    /**
     * @brief Clear short address from entry.
     * This command should only be implemented if @ref IEEE802154_CAP_SRC_ADDR_MATCH
     * is available.
     */
    IEEE802154_SRC_MATCH_SHORT_CLEAR,
    /**
     * @brief Add a extended address to entry.
     * This command should only be implemented if @ref IEEE802154_CAP_SRC_ADDR_MATCH
     * is available.
     */
    IEEE802154_SRC_MATCH_EXT_ADD,
    /**
     * @brief Clear extended address from entry.
     *
     * This command should only be implemented if @ref IEEE802154_CAP_SRC_ADDR_MATCH
     * is available.
     */
    IEEE802154_SRC_MATCH_EXT_CLEAR,
} ieee802154_src_match_t;

/**
 * @brief Address filter command
 */
typedef enum {
    IEEE802154_AF_SHORT_ADDR, /**< Set short IEEE 802.15.4 address (network_uint16_t) */
    IEEE802154_AF_EXT_ADDR,   /**< Set extended IEEE 802.15.4 address (eui64_t) */
    IEEE802154_AF_PANID,      /**< Set PAN ID (uint16_t) */
    IEEE802154_AF_PAN_COORD,  /**< Set device as PAN coordinator (bool) */
} ieee802154_af_cmd_t;

/**
 * @brief Frame Filter mode
 */
typedef enum {
    /**
     * @brief accept all valid frames that match address filter configuration
     */
    IEEE802154_FILTER_ACCEPT,
    /**
     * @brief accept only ACK frames
     *
     * @note This mode should only be implemented if the transceiver doesn't
     * handle ACK frame reception (when @ref IEEE802154_CAP_FRAME_RETRANS and
     * @ref IEEE802154_CAP_IRQ_ACK_TIMEOUT are not present).
     */
    IEEE802154_FILTER_ACK_ONLY,
    /**
     * @brief accept all valid frames
     *
     * @note This mode is optional
     */
    IEEE802154_FILTER_PROMISC,
    /**
     * @brief accept all frames, regardless of FCS
     *
     * @note This mode is optional
     */
    IEEE802154_FILTER_SNIFFER,
} ieee802154_filter_mode_t;

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
    int8_t retrans;                     /**< number of frame retransmissions of the last TX */
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
     * @brief pointer to the private descriptor of the device
     */
    void *priv;
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
 * @brief   Derived submac parameters from PHY configuration
 */
typedef struct {
    uint32_t ack_timeout_us;            /**< Ack timeout in µs */
    uint16_t csma_backoff_us;           /**< CSMA sender backoff period in µs */
    uint16_t sifs_period_us;            /**< SIFS period in µs */
} ieee802154_phy_conf_result_t;

/**
 * @brief Holder of the PHY configuration
 */
typedef struct {
    ieee802154_phy_mode_t phy_mode;     /**< IEEE802.15.4 PHY mode */
    uint16_t channel;                   /**< IEEE802.15.4 channel number */
    uint8_t page;                       /**< IEEE802.15.4 channel page */
    int8_t pow;                         /**< TX power in dBm */
    ieee802154_phy_conf_result_t res;   /**< PHY configuration deduced parameters */
} ieee802154_phy_conf_t;

/**
 * @brief extension for IEEE 802.15.4 OQPSK PHY
 */
typedef struct {
    ieee802154_phy_conf_t super;        /**< common settings */
} ieee802154_oqpsk_conf_t;

/**
 * @brief extension for IEEE 802.15.4 BPSK PHY
 */
typedef struct {
    ieee802154_phy_conf_t super;        /**< common settings */
} ieee802154_bpsk_conf_t;

/**
 * @brief extension for IEEE 802.15.4 ASK PHY
 */
typedef struct {
    ieee802154_phy_conf_t super;        /**< common settings */
} ieee802154_ask_conf_t;

/**
 * @brief extension for IEEE 802.15.4g MR-OQPSK PHY
 */
typedef struct {
    ieee802154_phy_conf_t super;        /**< common settings */
    ieee802154_mr_oqpsk_chips_t chips;  /**< chip rate       */
    uint8_t rate_mode;                  /**< rate mode       */
} ieee802154_mr_oqpsk_conf_t;

/**
 * @brief extension for IEEE 802.15.4g MR-ODFM PHY
 */
typedef struct {
    ieee802154_phy_conf_t super;        /**< common settings */
    uint8_t option;                     /**< OFDM Option */
    uint8_t scheme;                     /**< Modulation & Coding Scheme */
} ieee802154_mr_ofdm_conf_t;

/**
 * @brief extension for IEEE 802.15.4g MR-FSK PHY
 */
typedef struct {
    ieee802154_phy_conf_t super;        /**< common settings */
    ieee802154_mr_fsk_srate_t srate;    /**< symbol rate */
    uint8_t mod_ord;                    /**< modulation order, 2 or 4 */
    uint8_t mod_idx;                    /**< modulation index */
    ieee802154_mr_fsk_fec_t fec;        /**< forward error correction */
} ieee802154_mr_fsk_conf_t;

/**
 * @brief IEEE 802.15.4 radio operations
 */
typedef enum {
    /**
     * @brief Transmission of a preloaded frame.
     */
    IEEE802154_HAL_OP_TRANSMIT,
    /**
     * @brief Set the transceiver state to RX.
     */
    IEEE802154_HAL_OP_SET_RX,
    /**
     * @brief Set the transceiver state to IDLE (RX off).
     */
    IEEE802154_HAL_OP_SET_IDLE,
    /**
     * @brief Request Clear Channel Assessment
     */
    IEEE802154_HAL_OP_CCA,

    /* add more as needed (e.g Energy Scanning, transmit slotted ACK) */
} ieee802154_hal_op_t;

/**
 * @brief Radio ops struct declaration
 */
struct ieee802154_radio_ops {
    /**
     * @brief Radio device capabilities
     *
     * This field contains bitflags of supported capabilities
     * (@ref ieee802154_rf_caps_t) by the device.
     */
    const uint32_t caps;

    /**
     * @brief Write a frame into the framebuffer.
     *
     * This function shouldn't do any checks, so the frame MUST be valid. The
     * previous content of the framebuffer is replaced by @p psdu.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] psdu PSDU frame to be sent
     *
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*write)(ieee802154_dev_t *dev, const iolist_t *psdu);

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
     * @brief Read a frame from the internal framebuffer
     *
     * This function reads the received frame from the internal framebuffer.
     * It should try to copy the received PSDU frame into @p buf. The FCS
     * field will **not** be copied and its size **not** be taken into account
     * for the return value. If the radio provides any kind of framebuffer
     * protection, this function should release it.
     *
     * @post Don't call this function if there was no reception event
     * (either @ref IEEE802154_RADIO_INDICATION_RX_DONE or @ref
     * IEEE802154_RADIO_INDICATION_CRC_ERROR). Otherwise there's risk of RX
     * underflow.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[out] buf buffer to write the received PSDU frame into.
     * @param[in] size size of @p buf
     * @param[in] info information of the received frame (LQI, RSSI). Can be
     *            NULL if this information is not needed.
     *
     * @retval number of bytes written in @p buffer (0 if @p buf == NULL)
     * @retval -ENOBUFS if the frame doesn't fit in @p buf
     */
    int (*read)(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info);
    /**
     * @brief Turn off the device
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * When this function returns, the radio shall be off.
     *
     * @post the device is off
     *
     * @retval 0 on success
     * @retval negative errno on error
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
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*request_on)(ieee802154_dev_t *dev);

    /**
     * @brief Confirmation function for @ref ieee802154_radio_ops::request_on.
     *
     * @pre call to @ref ieee802154_radio_ops::request_on was successful.
     *
     * @post the transceiver state is IDLE
     * During boot or in case the radio doesn't support @ref
     * IEEE802154_CAP_REG_RETENTION when @ref off was called, the
     * Physical Information Base will be undefined. Thus, take into
     * consideration that the following functions should be called right after
     * the radio is turned on again:
     * - @ref set_cca_threshold
     * - @ref set_cca_mode
     * - @ref config_phy
     * - @ref config_addr_filter
     * - @ref set_csma_params
     * - @ref set_frame_filter_mode
     * - @ref config_src_addr_match
     * - @ref set_frame_retrans (if available)
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @retval 0 if the device is on
     * @retval -EAGAIN if the device is still busy turning on
     * @retval negative errno on error
     */
    int (*confirm_on)(ieee802154_dev_t *dev);

    /**
     * @brief   Request a radio operation.
     *
     * This functions is used to request a radio operation. See @ref
     * ieee802154_hal_op_t for a list of available operations.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] op operation to be executed
     * @param[in] ctx operation specific context
     *
     * @return status of the request
     *
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*request_op)(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx);

    /**
     * @brief   Confirmation function for @ref ieee802154_radio_ops::request_op
     *
     * This function must be called to finish a given @ref ieee802154_hal_op_t.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] op operation to be confirmed
     * @param[in] ctx operation specific context
     *
     * @return status of the request
     *
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*confirm_op)(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx);

    /**
     * @brief Set the threshold for the Energy Detection (first mode of CCA)
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] threshold the threshold in dBm.
     *
     * @retval 0 on success
     * @retval negative errno on error
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
     * @retval 0 on success
     * @retval -ENOTSUP if the mode is not supported
     * @retval negative errno on error
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
     * @pre the transceiver state is IDLE.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in,out] conf the PHY configuration
     *
     * @retval 0        on success
     * @retval -EINVAL  if the configuration is not valid for the device.
     * @retval <0       error, return value is negative errno indicating the cause.
     */
    int (*config_phy)(ieee802154_dev_t *dev, ieee802154_phy_conf_t *conf);

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
     * @retval 0 on success
     * @retval negative errno on error
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
     *                    ieee802154_radio_request_transmit function is
     *                    equivalent to CCA send.
     *
     * @retval 0 on success
     * @retval -EINVAL if the settings are not supported.
     * @retval negative errno on error
     */
    int (*set_csma_params)(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd,
                           int8_t retries);

    /**
     * @brief Set the frame filter mode.
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] mode address filter mode
     *
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*set_frame_filter_mode)(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode);

    /**
     * @brief Get the frame filter mode.
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[out] mode address filter mode
     *
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*get_frame_filter_mode)(ieee802154_dev_t *dev, ieee802154_filter_mode_t *mode);

    /**
     * @brief Configure the address filter.
     *
     * This functions is used for configuring the address filter parameters
     * required by the IEEE 802.15.4 standard.
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] cmd command for the address filter
     * @param[in] value value for @p cmd.
     *
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*config_addr_filter)(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value);

    /**
     * @brief Set the source address match configuration.
     *
     * This function configures the source address match filter in order to set
     * the Frame Pending bit in ACK frames accordingly.
     * In case the radio doesn't support @ref IEEE802154_CAP_SRC_ADDR_MATCH,
     * this functions is used to activate the Frame Pending bit for all ACK
     * frames (in order to be compliant with the IEEE 802.15.4 standard).
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] cmd command for the source address match configuration
     * @param[in] value value associated to @p cmd.
     *
     * @retval 0 on success
     * @retval negative errno on error
     */
    int (*config_src_addr_match)(ieee802154_dev_t *dev, ieee802154_src_match_t cmd,
                                 const void *value);
};

/**
 * @brief Check if the device has a specific capability
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] cap capabilities to check for
 *
 * @retval true if the device has the capability
 * @retval false if the device doesn't have the capability
 */
static inline bool ieee802154_radio_has_capability(ieee802154_dev_t *dev, uint32_t cap)
{
    return (dev->driver->caps & cap) == cap;
}

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
 * @brief Transmit a preloaded frame
 *
 * This functions calls ieee802154_radio_ops::request_op with @ref
 * IEEE802154_HAL_OP_TRANSMIT and NULL context.
 *
 * @pre The upper layer should have called set the transceiver to IDLE (see
 * @ref ieee802154_radio_set_idle) and the frame is already in the framebuffer
 * (@ref ieee802154_radio_ops_t::write).
 * @pre the device is on
 *
 * @note @ref ieee802154_radio_confirm_transmit MUST be used to
 * finish the transmission.
 *
 * @return result of @ref ieee802154_radio_request_transmit
 *
 * @retval 0 on success
 * @retval negative errno on error
 */
static inline int ieee802154_radio_request_transmit(ieee802154_dev_t *dev)
{
    return dev->driver->request_op(dev, IEEE802154_HAL_OP_TRANSMIT, NULL);
}

/**
 * @brief Confirmation function for @ref ieee802154_radio_request_transmit
 * This function must be called to finish the transmission procedure and
 * get the transmission status. This function should be called on @ref
 * IEEE802154_RADIO_CONFIRM_TX_DONE. If no interrupt is available, this
 * function can be polled.
 *
 * This functions calls ieee802154_radio_ops::confirm_op with @ref
 * IEEE802154_HAL_OP_TRANSMIT and sets the context to @p info.
 *
 * @pre the device is on
 * @pre call to @ref ieee802154_radio_request_transmit was successful.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[out] info the TX information. Pass NULL
 * if the information is not needed. If the radio supports AutoCCA, the
 * status should indicate transmission done or channel busy. If the radio
 * supports frame retransmissions, the status should indicate if medium
 * was busy, no ACK was received or transmission succeeded.
 *
 * @retval whether the transmission finished or not
 *
 * @return 0 if the transmission finished
 * @return -EAGAIN otherwise
 */
static inline int ieee802154_radio_confirm_transmit(ieee802154_dev_t *dev,
                                                    ieee802154_tx_info_t *info)
{
    return dev->driver->confirm_op(dev, IEEE802154_HAL_OP_TRANSMIT, info);
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
 * @brief Shortcut to @ref ieee802154_radio_ops::read
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[out] buf buffer to write the received frame into.
 * @param[in] size size of @p buf
 * @param[in] info information of the received frame (LQI, RSSI). Can be
 *            NULL if this information is not needed.
 *
 * @return result of @ref ieee802154_radio_ops::read
 */
static inline int ieee802154_radio_read(ieee802154_dev_t *dev,
                                                 void *buf,
                                                 size_t size,
                                                 ieee802154_rx_info_t *info)
{
    return dev->driver->read(dev, buf, size, info);
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
 * @pre the transceiver state is IDLE.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] conf the PHY configuration
 *
 * @return result of @ref ieee802154_radio_ops::config_phy
 */
static inline int ieee802154_radio_config_phy(ieee802154_dev_t *dev,
                                              ieee802154_phy_conf_t *conf)
{
    return dev->driver->config_phy(dev, conf);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::config_src_addr_match
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] cmd command for the source address match configuration
 * @param[in] value value associated to @p cmd.
 *
 * @return  result of @ref ieee802154_radio_ops::config_src_addr_match
 */
static inline int ieee802154_radio_config_src_address_match(ieee802154_dev_t *dev,
                                                            ieee802154_src_match_t cmd,
                                                            const void *value)
{
    return dev->driver->config_src_addr_match(dev, cmd, value);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::off
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @post the transceiver state is IDLE.
 *
 * @return result of @ref ieee802154_radio_ops::off
 */
static inline int ieee802154_radio_off(ieee802154_dev_t *dev)
{
    return dev->driver->off(dev);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::config_addr_filter
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] cmd command for the address filter
 * @param[in] value value for @p cmd.
 *
 * @return result of @ref ieee802154_radio_ops::config_addr_filter
 */
static inline int ieee802154_radio_config_addr_filter(ieee802154_dev_t *dev,
                                                  ieee802154_af_cmd_t cmd,
                                                  const void* value)
{
    return dev->driver->config_addr_filter(dev, cmd, value);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::set_frame_filter_mode
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] mode frame filter mode
 *
 * @return result of @ref ieee802154_radio_ops::set_frame_filter_mode
 */
static inline int ieee802154_radio_set_frame_filter_mode(ieee802154_dev_t *dev,
                                                    ieee802154_filter_mode_t mode)
{
    return dev->driver->set_frame_filter_mode(dev, mode);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_ops::get_frame_filter_mode
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[out] mode frame filter mode
 *
 * @return result of @ref ieee802154_radio_ops::get_frame_filter_mode
 */
static inline int ieee802154_radio_get_frame_filter_mode(ieee802154_dev_t *dev,
                                                         ieee802154_filter_mode_t *mode)
{
    if (dev->driver->get_frame_filter_mode) {
        return dev->driver->get_frame_filter_mode(dev, mode);
    }
    return -ENOTSUP;
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
 * @brief Request the transceiver state to IDLE.
 *
 * During IDLE, the radio won't be able to receive frames but it's still
 * responsive to other HAL functions.
 *
 * This functions calls ieee802154_radio_ops::request_op with @ref
 * IEEE802154_HAL_OP_SET_IDLE and sets the context to @p force
 *
 * @pre the device is on
 *
 * @note @ref ieee802154_radio_confirm_set_idle MUST be used to
 * finish the state transition.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] force whether the state transition should be forced or not. If
 *                  forced, the transceiver aborts any ongoing operation.
 *
 * @return status of the request
 *
 * @retval 0 on success
 * @retval negative errno on error
 */
static inline int ieee802154_radio_request_set_idle(ieee802154_dev_t *dev, bool force)
{
    return dev->driver->request_op(dev, IEEE802154_HAL_OP_SET_IDLE, &force);
}

/**
 * @brief Confirmation function for @ref ieee802154_radio_request_set_idle
 *
 * @pre call to @ref ieee802154_radio_request_set_idle was successful.
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return whether the state transition finished or not
 *
 * @return 0 if the transition finished
 * @return -EAGAIN otherwise.
 */
static inline int ieee802154_radio_confirm_set_idle(ieee802154_dev_t *dev)
{
    return dev->driver->confirm_op(dev, IEEE802154_HAL_OP_SET_IDLE, NULL);
}

/**
 * @brief Request the transceiver state to RX.
 *
 * During RX, the radio will listen to incoming frames
 *
 * This functions calls ieee802154_radio_ops::request_op with @ref
 * IEEE802154_HAL_OP_SET_RX and NULL context.
 *
 * @pre the device is on
 *
 * @note @ref ieee802154_radio_confirm_set_rx MUST be used to
 * finish the state transition.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return status of the request
 *
 * @retval 0 on success
 * @retval negative errno on error
 */
static inline int ieee802154_radio_request_set_rx(ieee802154_dev_t *dev)
{
    return dev->driver->request_op(dev, IEEE802154_HAL_OP_SET_RX, NULL);
}

/**
 * @brief Confirmation function for @ref ieee802154_radio_request_set_rx
 *
 * @pre call to @ref ieee802154_radio_request_set_rx was successful.
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return whether the state transition finished or not
 *
 * @return 0 if the transition finished
 * @return -EAGAIN otherwise.
 */
static inline int ieee802154_radio_confirm_set_rx(ieee802154_dev_t *dev)
{
    return dev->driver->confirm_op(dev, IEEE802154_HAL_OP_SET_RX, NULL);
}

/**
 * @brief Set transceiver state to IDLE (blocking)
 *
 * This function will internally call @ref ieee802154_radio_request_set_idle
 * and poll @ref ieee802154_radio_confirm_set_idle.
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] force whether the state transition should be forced or not. If
 *                  forced, the transceiver aborts any ongoing operation.
 *
 * @return result of the state transition
 *
 * @retval 0 on success
 * @retval negative errno on error
 */
static inline int ieee802154_radio_set_idle(ieee802154_dev_t *dev, bool force)
{
    int res = ieee802154_radio_request_set_idle(dev, force);
    if (res < 0) {
        return res;
    }
    while (ieee802154_radio_confirm_set_idle(dev) == -EAGAIN) {}

    return 0;
}

/**
 * @brief Set transceiver state to RX (blocking)
 *
 * This function will internally call @ref ieee802154_radio_request_set_rx
 * and poll @ref ieee802154_radio_confirm_set_rx.
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return result of the state transition
 *
 * @retval 0 on success
 * @retval negative errno on error
 */
static inline int ieee802154_radio_set_rx(ieee802154_dev_t *dev)
{
    int res = ieee802154_radio_request_set_rx(dev);
    if (res < 0) {
        return res;
    }
    while (ieee802154_radio_confirm_set_rx(dev) == -EAGAIN) {}

    return 0;
}

/**
 * @brief Request Stand-Alone Clear Channel Assessment
 *
 * This functions calls ieee802154_radio_ops::request_op with @ref
 * IEEE802154_HAL_OP_CCA and NULL context.
 *
 * @pre the device is on
 *
 * @note @ref ieee802154_radio_confirm_cca MUST be used to
 * finish the CCA procedure and get the channel status.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return 0 on success
 * @return negative errno on error
 */
static inline int ieee802154_radio_request_cca(ieee802154_dev_t *dev)
{
    return dev->driver->request_op(dev, IEEE802154_HAL_OP_CCA, NULL);
}

/**
 * @brief Shortcut to @ref ieee802154_radio_confirm_cca
 *
 * This function must be called to finish the CCA procedure.  This
 * function should be called on @ref IEEE802154_RADIO_CONFIRM_CCA,
 * If no interrupt is available, this function can be polled.
 *
 * This functions calls ieee802154_radio_ops::request_op with @ref
 * IEEE802154_HAL_OP_CCA and sets the context to a boolean where the result
 * of the CCA should be store. Setting it to true means the channel is clear.
 *
 * @pre call to @ref ieee802154_radio_request_cca was successful.
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return status of the CCA procedure
 *
 * @retval positive number if the channel is clear
 * @retval 0 if the channel is busy
 * @retval -EAGAIN if the CCA procedure hasn't finished.
 */
static inline int ieee802154_radio_confirm_cca(ieee802154_dev_t *dev)
{
    bool clear;
    int res = dev->driver->confirm_op(dev, IEEE802154_HAL_OP_CCA, &clear);
    if (res < 0) {
        return res;
    }
    return clear;
}

/**
 * @brief Perform a Clear Channel Assessment (blocking)
 *
 * This function will internally call @ref ieee802154_radio_request_cca
 * and poll @ref ieee802154_radio_confirm_cca.
 *
 * @pre the device is on
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return status of the CCA
 *
 * @retval positive number if the channel is clear
 * @retval 0 if the channel is busy
 * @retval negative errno on error
 */
static inline int ieee802154_radio_cca(ieee802154_dev_t *dev)
{
    int res = ieee802154_radio_request_cca(dev);
    if (res < 0) {
        return res;
    }
    while ((res = ieee802154_radio_confirm_cca(dev)) == -EAGAIN) {}

    return res;
}

/**
 * @brief Check if the device supports ACK timeout
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_IRQ_ACK_TIMEOUT.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_ack_timeout(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_IRQ_ACK_TIMEOUT);
}

/**
 * @brief Check if the device supports frame retransmissions (with CSMA-CA).
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_FRAME_RETRANS.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_frame_retrans(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_FRAME_RETRANS);
}

/**
 * @brief Check if the device supports Auto CSMA-CA for transmissions.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_AUTO_CSMA.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_auto_csma(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_AUTO_CSMA);
}

/**
 * @brief Check if the device supports the IEEE802.15.4 Sub-GHz band
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_SUB_GHZ.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_sub_ghz(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_SUB_GHZ);
}

/**
 * @brief Check if the device supports the IEEE802.15.4 2.4 GHz band
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_24_GHZ.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_24_ghz(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_24_GHZ);
}

/**
 * @brief Check if the device supports TX done interrupt
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_IRQ_TX_DONE.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_tx_done(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_IRQ_TX_DONE);
}

/**
 * @brief Check if the device supports RX start interrupt
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_IRQ_RX_START.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_rx_start(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_IRQ_RX_START);
}

/**
 * @brief Check if the device supports TX start interrupt
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_IRQ_TX_START.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_tx_start(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_IRQ_TX_START);
}

/**
 * @brief Check if the device supports CCA done interrupt
 *
 * Internally this function reads ieee802154_radio_ops::caps with @ref
 * IEEE802154_CAP_IRQ_CCA_DONE.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_irq_cca_done(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_IRQ_CCA_DONE);
}

/**
 * @brief Check if the device reports the number of retransmissions of the last
 * TX procedure.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_FRAME_RETRANS_INFO.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_frame_retrans_info(
    ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_FRAME_RETRANS_INFO);
}

/**
 * @brief Check if the device supports the BPSK PHY mode.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_PHY_BPSK.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_phy_bpsk(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_PHY_BPSK);
}

/**
 * @brief Check if the device supports the ASK PHY mode.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_PHY_ASK.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_phy_ask(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_PHY_ASK);
}

/**
 * @brief Check if the device supports the O-QPSK PHY mode.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_PHY_OQPSK.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_phy_oqpsk(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_PHY_OQPSK);
}

/**
 * @brief Check if the device supports the MR-O-QPSK PHY mode.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_PHY_MR_OQPSK.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_phy_mr_oqpsk(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_PHY_MR_OQPSK);
}

/**
 * @brief Check if the device supports the MR-OFDM PHY mode.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_PHY_MR_OFDM.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_phy_mr_ofdm(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_PHY_MR_OFDM);
}

/**
 * @brief Check if the device supports the MR-FSK PHY mode.
 *
 * Internally this function reads ieee802154_radio_ops::caps and checks for
 * @ref IEEE802154_CAP_PHY_MR_FSK.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return true if the device has support
 * @return false otherwise
 */
static inline bool ieee802154_radio_has_phy_mr_fsk(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_CAP_PHY_MR_FSK);
}

/**
 * @brief Get supported PHY modes of the device.
 *
 * Internally this function reads ieee802154_radio_ops::caps and returns only
 * the bits from  @ref IEEE802154_RF_CAPS_PHY_MASK.
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 *
 * @return PHY modes bit mask.
 */
static inline uint32_t ieee802154_radio_get_phy_modes(ieee802154_dev_t *dev)
{
    return (dev->driver->caps & IEEE802154_RF_CAPS_PHY_MASK);
}

/**
 * @brief Convert a @ref ieee802154_phy_mode_t to a @ref ieee802154_rf_caps_t
 * value.
 *
 * @param[in] phy_mode PHY mode
 *
 * @return Equivalent capability given the PHY mode.
 * @return 0 on invalid values
 * @return 0 when @ref IEEE802154_PHY_DISABLED is given as the parameter.
 */
static inline uint32_t ieee802154_phy_mode_to_cap(
    ieee802154_phy_mode_t phy_mode)
{
    switch (phy_mode) {
        case IEEE802154_PHY_BPSK:
            return IEEE802154_CAP_PHY_BPSK;
        case IEEE802154_PHY_ASK:
            return IEEE802154_CAP_PHY_ASK;
        case IEEE802154_PHY_OQPSK:
            return IEEE802154_CAP_PHY_OQPSK;
        case IEEE802154_PHY_MR_OQPSK:
            return IEEE802154_CAP_PHY_MR_OQPSK;
        case IEEE802154_PHY_MR_OFDM:
            return IEEE802154_CAP_PHY_MR_OFDM;
        case IEEE802154_PHY_MR_FSK:
            return IEEE802154_CAP_PHY_MR_FSK;

        case IEEE802154_PHY_DISABLED:
        default:
            break;
    }

    return 0;
}

/**
 * @brief Convert a @ref ieee802154_rf_caps_t to a @ref ieee802154_phy_mode_t
 * value.
 *
 * @note The @p parameter must be one of the PHY capabilities.
 *
 * @param[in] cap The IEEE 802.15.4 capability.
 *
 * @return Equivalent phy mode given the capability.
 * @return 0 on invalid values
 */
static inline ieee802154_phy_mode_t ieee802154_cap_to_phy_mode(uint32_t cap)
{
    switch (cap) {
        case IEEE802154_CAP_PHY_BPSK:
            return IEEE802154_PHY_BPSK;
        case IEEE802154_CAP_PHY_ASK:
            return IEEE802154_PHY_ASK;
        case IEEE802154_CAP_PHY_OQPSK:
            return IEEE802154_PHY_OQPSK;
        case IEEE802154_CAP_PHY_MR_OQPSK:
            return IEEE802154_PHY_MR_OQPSK;
        case IEEE802154_PHY_MR_OFDM:
            return IEEE802154_PHY_MR_OFDM;
        case IEEE802154_CAP_PHY_MR_FSK:
            return IEEE802154_PHY_MR_FSK;

        default:
            break;
    }

    return IEEE802154_PHY_DISABLED;
}

#ifdef __cplusplus
}
#endif

/** @} */
