/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_at86rf2xx AT86RF2xx based drivers
 * @ingroup     drivers_netdev
 *
 * This module contains drivers for radio devices in Atmel's AT86RF2xx series.
 * The driver is aimed to work with all devices of this series.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for AT86RF2xx based drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef AT86RF2XX_H
#define AT86RF2XX_H

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/nettype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum possible packet size in byte
 */
#define AT86RF2XX_MAX_PKT_LENGTH        (IEEE802154_FRAME_LEN_MAX)

/**
 * @name    Channel configuration
 * @{
 */
#ifdef MODULE_AT86RF212B
/* the AT86RF212B has a sub-1GHz radio */
#define AT86RF2XX_MIN_CHANNEL           (IEEE802154_CHANNEL_MIN_SUBGHZ)
#define AT86RF2XX_MAX_CHANNEL           (IEEE802154_CHANNEL_MAX_SUBGHZ)
#define AT86RF2XX_DEFAULT_CHANNEL       (IEEE802154_DEFAULT_SUBGHZ_CHANNEL)
/* Page 2 is O-QPSK 100 kbit/s (channel 0), or 250 kbit/s (channels 1-10) */
#define AT86RF2XX_DEFAULT_PAGE          (IEEE802154_DEFAULT_SUBGHZ_PAGE)
#else
#define AT86RF2XX_MIN_CHANNEL           (IEEE802154_CHANNEL_MIN)
#define AT86RF2XX_MAX_CHANNEL           (IEEE802154_CHANNEL_MAX)
#define AT86RF2XX_DEFAULT_CHANNEL       (IEEE802154_DEFAULT_CHANNEL)
/* Only page 0 is supported in the 2.4 GHz band */
#endif
/** @} */

/**
 * @brief   Default PAN ID
 *
 * @todo    Read some global network stack specific configuration value
 */
#define AT86RF2XX_DEFAULT_PANID         (IEEE802154_DEFAULT_PANID)

/**
 * @brief   Default TX power (0dBm)
 */
#define AT86RF2XX_DEFAULT_TXPOWER       (IEEE802154_DEFAULT_TXPOWER)

/**
 * @brief   Base (minimal) RSSI value in dBm
 */
#if MODULE_AT86RF233
#   define RSSI_BASE_VAL                   (-94)
#elif MODULE_AT86RF212B
/**
 * @note for the default settings in RIOT for the at86rf212b,
 *       for other seetings this value may change.
 */
#   define RSSI_BASE_VAL                   (-98)
#else
#   define RSSI_BASE_VAL                   (-91)
#endif

#if defined(DOXYGEN) || defined(MODULE_AT86RF232) || defined(MODULE_AT86RF233)
/**
 * @brief   Frame retry counter reporting
 *
 * The AT86RF2XX_HAVE_RETRIES flag enables support for NETOPT_TX_RETRIES NEEDED
 * operation. Required for this functionality is the XAH_CTRL_2 register which
 * contains the frame retry counter. Only the at86rf232 and the at86rf233
 * support this register.
 */
#define AT86RF2XX_HAVE_RETRIES             (1)
#else
#define AT86RF2XX_HAVE_RETRIES             (0)
#endif

/**
 * @name    Flags for device internal states (see datasheet)
 * @{
 */
#define AT86RF2XX_STATE_P_ON           (0x00)     /**< initial power on */
#define AT86RF2XX_STATE_BUSY_RX        (0x01)     /**< busy receiving data (basic mode) */
#define AT86RF2XX_STATE_BUSY_TX        (0x02)     /**< busy transmitting data (basic mode) */
#define AT86RF2XX_STATE_FORCE_TRX_OFF  (0x03)     /**< force transition to idle */
#define AT86RF2XX_STATE_RX_ON          (0x06)     /**< listen mode (basic mode) */
#define AT86RF2XX_STATE_TRX_OFF        (0x08)     /**< idle */
#define AT86RF2XX_STATE_PLL_ON         (0x09)     /**< ready to transmit */
#define AT86RF2XX_STATE_SLEEP          (0x0f)     /**< sleep mode */
#define AT86RF2XX_STATE_BUSY_RX_AACK   (0x11)     /**< busy receiving data (extended mode) */
#define AT86RF2XX_STATE_BUSY_TX_ARET   (0x12)     /**< busy transmitting data (extended mode) */
#define AT86RF2XX_STATE_RX_AACK_ON     (0x16)     /**< wait for incoming data */
#define AT86RF2XX_STATE_TX_ARET_ON     (0x19)     /**< ready for sending data */
#define AT86RF2XX_STATE_IN_PROGRESS    (0x1f)     /**< ongoing state conversion */
/** @} */

/**
 * @name    Internal device option flags
 * @{
 */
#define AT86RF2XX_OPT_TELL_TX_START  (0x0001)       /**< notify MAC layer on TX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_TX_END    (0x0002)       /**< notify MAC layer on TX
                                                     *   finished */
#define AT86RF2XX_OPT_TELL_RX_START  (0x0004)       /**< notify MAC layer on RX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_RX_END    (0x0008)       /**< notify MAC layer on RX
                                                     *   finished */
#define AT86RF2XX_OPT_CSMA           (0x0010)       /**< CSMA active */
#define AT86RF2XX_OPT_PROMISCUOUS    (0x0020)       /**< promiscuous mode
                                                     *   active */
#define AT86RF2XX_OPT_PRELOADING     (0x0040)       /**< preloading enabled */
#define AT86RF2XX_OPT_AUTOACK        (0x0080)       /**< Auto ACK active */
#define AT86RF2XX_OPT_ACK_PENDING    (0x0100)       /**< ACK frames with data
                                                     *   pending */

/** @} */

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct at86rf2xx_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t sleep_pin;       /**< GPIO pin connected to the sleep pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} at86rf2xx_params_t;

/**
 * @brief   Device descriptor for AT86RF2XX radio devices
 *
 * @extends netdev_ieee802154_t
 */
typedef struct {
    netdev_ieee802154_t netdev;             /**< netdev parent struct */
    /* device specific fields */
    at86rf2xx_params_t params;              /**< parameters for initialization */
    uint16_t flags;                         /**< Device specific flags */
    uint8_t state;                          /**< current state of the radio */
    uint8_t tx_frame_len;                   /**< length of the current TX frame */
#ifdef MODULE_AT86RF212B
    /* Only AT86RF212B supports multiple pages (PHY modes) */
    uint8_t page;                       /**< currently used channel page */
#endif
    uint8_t idle_state;                 /**< state to return to after sending */
    uint8_t pending_tx;                 /**< keep track of pending TX calls
                                             this is required to know when to
                                             return to @ref at86rf2xx_t::idle_state */
#if AT86RF2XX_HAVE_RETRIES
    /* Only radios with the XAH_CTRL_2 register support frame retry reporting */
    uint8_t tx_retries;                 /**< Number of NOACK retransmissions */
#endif
    /** @} */
} at86rf2xx_t;

/**
 * @brief   Setup an AT86RF2xx based device state
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       parameters for device initialization
 */
void at86rf2xx_setup(at86rf2xx_t *dev, const at86rf2xx_params_t *params);

/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in,out] dev       device to reset
 */
void at86rf2xx_reset(at86rf2xx_t *dev);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (2-byte) short address
 */
uint16_t at86rf2xx_get_addr_short(const at86rf2xx_t *dev);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void at86rf2xx_set_addr_short(at86rf2xx_t *dev, uint16_t addr);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (8-byte) long address
 */
uint64_t at86rf2xx_get_addr_long(const at86rf2xx_t *dev);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void at86rf2xx_set_addr_long(at86rf2xx_t *dev, uint64_t addr);

/**
 * @brief   Get the configured channel number of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel number
 */
uint8_t at86rf2xx_get_chan(const at86rf2xx_t *dev);

/**
 * @brief   Set the channel number of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] chan          channel number to set
 */
void at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t chan);

/**
 * @brief   Get the configured channel page of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel page
 */
uint8_t at86rf2xx_get_page(const at86rf2xx_t *dev);

/**
 * @brief   Set the channel page of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] page          channel page to set
 */
void at86rf2xx_set_page(at86rf2xx_t *dev, uint8_t page);

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t at86rf2xx_get_pan(const at86rf2xx_t *dev);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] pan           PAN ID to set
 */
void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t at86rf2xx_get_txpower(const at86rf2xx_t *dev);

/**
 * @brief   Set the transmission power of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] txpower       transmission power in dBm
 */
void at86rf2xx_set_txpower(const at86rf2xx_t *dev, int16_t txpower);

/**
 * @brief   Get the maximum number of retransmissions
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retransmissions
 */
uint8_t at86rf2xx_get_max_retries(const at86rf2xx_t *dev);

/**
 * @brief   Set the maximum number of retransmissions
 *
 * This setting specifies the number of attempts to retransmit a frame, when it
 * was not acknowledged by the recipient, before the transaction gets cancelled.
 * The maximum value is 7.
 *
 * @param[in] dev           device to write to
 * @param[in] max           the maximum number of retransmissions
 */
void at86rf2xx_set_max_retries(const at86rf2xx_t *dev, uint8_t max);

/**
 * @brief   Get the maximum number of channel access attempts per frame (CSMA)
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retries
 */
uint8_t at86rf2xx_get_csma_max_retries(const at86rf2xx_t *dev);

/**
 * @brief   Set the maximum number of channel access attempts per frame (CSMA)
 *
 * This setting specifies the number of attempts to access the channel to
 * transmit a frame. If the channel is busy @p retries times, then frame
 * transmission fails.
 * Valid values: 0 to 5, -1 means CSMA disabled
 *
 * @param[in] dev           device to write to
 * @param[in] retries       the maximum number of retries
 */
void at86rf2xx_set_csma_max_retries(const at86rf2xx_t *dev, int8_t retries);

/**
 * @brief   Set the min and max backoff exponent for CSMA/CA
 *
 * - Maximum BE: 0 - 8
 * - Minimum BE: 0 - [max]
 *
 * @param[in] dev           device to write to
 * @param[in] min           the minimum BE
 * @param[in] max           the maximum BE
 */
void at86rf2xx_set_csma_backoff_exp(const at86rf2xx_t *dev,
                                    uint8_t min, uint8_t max);

/**
 * @brief   Set seed for CSMA random backoff
 *
 * @param[in] dev           device to write to
 * @param[in] entropy       11 bit of entropy as seed for random backoff
 */
void at86rf2xx_set_csma_seed(const at86rf2xx_t *dev, const uint8_t entropy[2]);

/**
 * @brief   Get the CCA threshold value
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the current CCA threshold value
 */
int8_t at86rf2xx_get_cca_threshold(const at86rf2xx_t *dev);

/**
 * @brief   Set the CCA threshold value
 *
 * @param[in] dev           device to write to
 * @param[in] value         the new CCA threshold value
 */
void at86rf2xx_set_cca_threshold(const at86rf2xx_t *dev, int8_t value);

/**
 * @brief   Get the latest ED level measurement
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the last ED level
 */
int8_t at86rf2xx_get_ed_level(at86rf2xx_t *dev);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in,out] dev       device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in,out] dev       device to change state of
 * @param[in] state         the targeted new state
 *
 * @return                  the previous state before the new state was set
 */
uint8_t at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state);

/**
 * @brief   Convenience function for simply sending data
 *
 * @note This function ignores the PRELOADING option
 *
 * @param[in,out] dev       device to use for sending
 * @param[in] data          data to send (must include IEEE802.15.4 header)
 * @param[in] len           length of @p data
 *
 * @return                  number of bytes that were actually send
 * @return                  0 on error
 */
size_t at86rf2xx_send(at86rf2xx_t *dev, const uint8_t *data, size_t len);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in,out] dev        device to prepare for sending
 */
void at86rf2xx_tx_prepare(at86rf2xx_t *dev);

/**
 * @brief   Load chunks of data into the transmit buffer of the given device
 *
 * @param[in,out] dev       device to write data to
 * @param[in] data          buffer containing the data to load
 * @param[in] len           number of bytes in @p buffer
 * @param[in] offset        offset used when writing data to internal buffer
 *
 * @return                  offset + number of bytes written
 */
size_t at86rf2xx_tx_load(at86rf2xx_t *dev, const uint8_t *data,
                         size_t len, size_t offset);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
void at86rf2xx_tx_exec(const at86rf2xx_t *dev);

/**
 * @brief   Perform one manual channel clear assessment (CCA)
 *
 * The CCA mode and threshold level depends on the current transceiver settings.
 *
 * @param[in]  dev          device to use
 *
 * @return                  true if channel is determined clear
 * @return                  false if channel is determined busy
 */
bool at86rf2xx_cca(at86rf2xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_H */
/** @} */
