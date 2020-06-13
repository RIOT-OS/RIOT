/*
 * Copyright (C) 2019 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_at86rf215 AT86RF215 based drivers
 * @ingroup     drivers_netdev
 *
 * This module contains a driver for the Atmel AT86RF215 radio.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for AT86RF215 based drivers
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef AT86RF215_H
#define AT86RF215_H

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/nettype.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Registers for the Radio Frontend
 */
typedef struct at86rf215_RF_regs at86rf215_RF_regs_t;

/**
 * @brief   Registers for the BaseBand Controller
 */
typedef struct at86rf215_BBC_regs at86rf215_BBC_regs_t;

/**
 * @brief Signature for the Battery monitor callback.
 *
 * @param[in] arg           optional argument which is passed to the
 *                          callback
 */
typedef void (*at86rf215_batmon_cb_t)(void *arg);

/**
 * @brief MR-O-QPSK chip rates (kChip/s)
 */
enum {
    AT86RF215_FCHIP_100,
    AT86RF215_FCHIP_200,
    AT86RF215_FCHIP_1000,
    AT86RF215_FCHIP_2000,
};

/**
 * @brief   Maximum possible packet size in byte
 */
#define AT86RF215_MAX_PKT_LENGTH        (2047)

/**
 * @defgroup drivers_at86rf215_config     AT86RF215 driver compile configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief   Set to 1 if the clock output of the AT86RF215 is used
 *          as a clock source on the board.
 *          Otherwise it is turned off to save energy.
 */
#ifdef DOXYGEN
#define CONFIG_AT86RF215_USE_CLOCK_OUTPUT
#endif

#if defined(DOXYGEN) && !defined(CONFIG_AT86RF215_TRIM_VAL)
/**
 * @brief   Trim value for the external crystal oscillator.
 *
 *          Each increment adds 300nF capacitance between the crystal oscillator pins
 *          TCXO and XTAL2.
 *
 *          Range: 0..15
 *
 *          Use in conjunction with @see CONFIG_AT86RF215_USE_CLOCK_OUTPUT and a frequency
 *          meter connected to the clock output pin of the AT86RF215.
 *          Tweak the value until the measured clock output matches 26 MHz the best.
 */
#define CONFIG_AT86RF215_TRIM_VAL              (0)
#endif

/**
 * @name    Channel configuration
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_CHANNEL
#define CONFIG_AT86RF215_DEFAULT_CHANNEL        (CONFIG_IEEE802154_DEFAULT_CHANNEL)
#endif

#ifndef CONFIG_AT86RF215_DEFAULT_SUBGHZ_CHANNEL
#define CONFIG_AT86RF215_DEFAULT_SUBGHZ_CHANNEL (CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL)
#endif
/** @} */

/**
 * @name    Enable Reduced Power Consumption
 * @{
 */
#ifndef CONFIG_AT86RF215_RPC_EN
#define CONFIG_AT86RF215_RPC_EN                 (0)
#endif
/** @} */

/**
 * @name    Default PHY Mode
 * @{
 */
#if IS_ACTIVE(CONFIG_AT86RF215_DEFAULT_LEGACY_OQPSK)
#define CONFIG_AT86RF215_DEFAULT_PHY_MODE   (IEEE802154_PHY_OQPSK)
#elif IS_ACTIVE(CONFIG_AT86RF215_DEFAULT_MR_OQPSK)
#define CONFIG_AT86RF215_DEFAULT_PHY_MODE   (IEEE802154_PHY_MR_OQPSK)
#elif IS_ACTIVE(CONFIG_AT86RF215_DEFAULT_MR_OFDM)
#define CONFIG_AT86RF215_DEFAULT_PHY_MODE   (IEEE802154_PHY_MR_OFDM)
#endif

#ifndef CONFIG_AT86RF215_DEFAULT_PHY_MODE
#define CONFIG_AT86RF215_DEFAULT_PHY_MODE   (IEEE802154_PHY_OQPSK)
#endif
/** @} */

/**
 * @name    Default O-QPSK Rate Mode
 *          Non-zero value enables proprietary high data rate by default
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_OQPSK_RATE
#define CONFIG_AT86RF215_DEFAULT_OQPSK_RATE     (0)
#endif
/** @} */

/**
 * @name    Default MR-O-QPSK Chip Rate
 * @{
 */
#if IS_ACTIVE(CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS_100)
#define CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS (AT86RF215_FCHIP_100)
#elif IS_ACTIVE(CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS_200)
#define CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS (AT86RF215_FCHIP_200)
#elif IS_ACTIVE(CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS_1000)
#define CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS (AT86RF215_FCHIP_1000)
#elif IS_ACTIVE(CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS_2000)
#define CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS (AT86RF215_FCHIP_2000)
#endif

#ifndef CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS
#define CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS (AT86RF215_FCHIP_1000)
#endif
/** @} */

/**
 * @name    Default MR-O-QPSK Rate Mode
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_MR_OQPSK_RATE
#define CONFIG_AT86RF215_DEFAULT_MR_OQPSK_RATE  (2)
#endif
/** @} */

/**
 * @name    Default MR-OFDM Option
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_MR_OFDM_OPT
#define CONFIG_AT86RF215_DEFAULT_MR_OFDM_OPT    (2)
#endif
/** @} */

/**
 * @name    Default MR-OFDM Modulation & Coding Scheme
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_MR_OFDM_MCS
#define CONFIG_AT86RF215_DEFAULT_MR_OFDM_MCS    (2)
#endif
/** @} */

/**
 * @name    Default MR-FSK Symbol Rate
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_MR_FSK_SRATE
#define CONFIG_AT86RF215_DEFAULT_MR_FSK_SRATE   FSK_SRATE_200K
#endif
/** @} */

/**
 * @name    Default MR-FSK Modulation Index, fraction of 64
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_MR_FSK_MOD_IDX
#define CONFIG_AT86RF215_DEFAULT_MR_FSK_MOD_IDX (64)
#endif
/** @} */

/**
 * @name    Default MR-FSK Modulation Order
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_MR_FSK_MORD
#define CONFIG_AT86RF215_DEFAULT_MR_FSK_MORD    FSK_MORD_4SFK
#endif
/** @} */

/**
 * @name    Default MR-FSK Forward Error Correction Scheme
 * @{
 */
#ifndef CONFIG_AT86RF215_DEFAULT_MR_FSK_FEC
#define CONFIG_AT86RF215_DEFAULT_MR_FSK_FEC     IEEE802154_FEC_NONE
#endif
/** @} */

/**
 * @brief   Default TX power (0dBm)
 */
#ifndef CONFIG_AT86RF215_DEFAULT_TXPOWER
#define CONFIG_AT86RF215_DEFAULT_TXPOWER       (CONFIG_IEEE802154_DEFAULT_TXPOWER)
#endif

/** @} */

/**
 * @name    Flags for device internal states (see datasheet)
 * @{
 */
typedef enum {
    AT86RF215_STATE_OFF,        /**< radio not configured */
    AT86RF215_STATE_IDLE,       /**< idle state, listening */
    AT86RF215_STATE_RX_SEND_ACK,/**< receiving frame, sending ACK */
    AT86RF215_STATE_TX,         /**< sending frame */
    AT86RF215_STATE_TX_WAIT_ACK,/**< sending frame, wait for ACK */
    AT86RF215_STATE_SLEEP       /**< sleep mode, not listening */
} at86rf215_state_t;
/** @} */

enum {
    AT86RF215_MODE_LEGACY_OQPSK,
    AT86RF215_MODE_MR_OQPSK,
    AT86RF215_MODE_MR_OFDM,
    AT86RF215_MODE_MR_FSK
};

/**
 * @name    Internal device option flags
 * @{
 */
#define AT86RF215_OPT_TELL_TX_START  (0x0001)       /**< notify MAC layer on TX start */
#define AT86RF215_OPT_TELL_TX_END    (0x0002)       /**< notify MAC layer on TX finished */
#define AT86RF215_OPT_TELL_RX_START  (0x0004)       /**< notify MAC layer on RX start */
#define AT86RF215_OPT_TELL_RX_END    (0x0008)       /**< notify MAC layer on RX finished */
#define AT86RF215_OPT_CSMA           (0x0010)       /**< CSMA active */
#define AT86RF215_OPT_PROMISCUOUS    (0x0020)       /**< promiscuous mode active */
#define AT86RF215_OPT_PRELOADING     (0x0040)       /**< preloading enabled */
#define AT86RF215_OPT_AUTOACK        (0x0080)       /**< Auto ACK active */
#define AT86RF215_OPT_ACK_REQUESTED  (0x0100)       /**< ACK requested for current frame */
#define AT86RF215_OPT_AGCH           (0x0200)       /**< AGC Hold active */
#define AT86RF215_OPT_TX_PENDING     (0x0400)       /**< Frame is loaded into TX buffer */
#define AT86RF215_OPT_CCA_PENDING    (0x0800)       /**< CCA needs to be done for the current frame */
#define AT86RF215_OPT_RPC            (0x1000)       /**< Enable Reduced Power Consumption */
#define AT86RF215_OPT_CCATX          (0x2000)       /**< TX after CCA performd automatically */
/** @} */

/**
 * @name    Internal timeout flags
 * @{
 */
#define AT86RF215_TIMEOUT_ACK        (0x0001)       /**< ACK timeout */
#define AT86RF215_TIMEOUT_CSMA       (0x0002)       /**< CMSA timeout */
/** @} */

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct at86rf215_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} at86rf215_params_t;

/**
 * @brief   Device descriptor for AT86RF215 radio devices
 *
 * @extends netdev_ieee802154_t
 */
typedef struct at86rf215 {
    netdev_ieee802154_t netdev;             /**< netdev parent struct */
    /* device specific fields */
    at86rf215_params_t params;              /**< parameters for initialization */
    struct at86rf215 *sibling;              /**< The other radio */
    const at86rf215_RF_regs_t  *RF;         /**< Radio Frontend Registers */
    const at86rf215_BBC_regs_t *BBC;        /**< Baseband Registers */
    xtimer_t timer;                         /**< timer for ACK & CSMA timeout */
    msg_t timer_msg;                        /**< message for timeout timer */
    uint32_t ack_timeout_usec;              /**< time to wait before retransmission in µs */
    uint32_t csma_backoff_period;           /**< CSMA Backoff period */
    uint16_t flags;                         /**< Device specific flags */
    uint16_t num_chans;                     /**< Number of legal channel at current modulation */
    uint16_t tx_frame_len;                  /**< length of the current TX frame */
    uint8_t timeout;                        /**< indicates which timeout was reached */
    uint8_t state;                          /**< current state of the radio */
    uint8_t retries_max;                    /**< number of retries until ACK is received */
    uint8_t retries;                        /**< retries left */
    uint8_t csma_retries_max;               /**< number of retries until channel is clear */
    uint8_t csma_retries;                   /**< CSMA retries left */
#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
    uint8_t fsk_pl;                         /**< FSK Preamble Length */
#endif
    uint8_t csma_minbe;                     /**< CSMA minimum backoff exponent */
    uint8_t csma_maxbe;                     /**< CSMA maximum backoff exponent */
    int8_t  csma_ed;                        /**< CSMA energy detect threshold */
} at86rf215_t;

/**
 * @brief   Setup an AT86RF215 based device state
 *
 * @param[out] dev_09       sub-GHz device descriptor
 * @param[out] dev_24       2.4 GHz device descriptor
 * @param[in]  params       parameters for device initialization
 * @param[in]  index        index of @p params in a global parameter struct array.
 *                          If initialized manually, pass a unique identifier instead.
 */
void at86rf215_setup(at86rf215_t *dev_09, at86rf215_t *dev_24, const at86rf215_params_t *params, uint8_t index);

/**
 * @brief   Trigger a hardware reset and configure radio with default values.
 *
 * @param[in,out] dev       device to configure
 */
void at86rf215_reset_and_cfg(at86rf215_t *dev);

/**
 * @brief   Trigger a hardware reset, configuration is retained.
 *
 * @param[in,out] dev       device to reset
 */
void at86rf215_reset(at86rf215_t *dev);

/**
 * @brief   Get the short address of the given device form multi address filter
 *
 * @param[in] dev           device to read from
 * @param[in] filter        address filter to read
 *
 * @return                  the currently set (2-byte) short address
 */
uint16_t at86rf215_get_addr_short(const at86rf215_t *dev, uint8_t filter);

/**
 * @brief   Set the short address of the given device to multi address filter
 *
 * @param[in,out] dev       device to write to
 * @param[in] filter          (1-byte) address filter to set
 * @param[in] addr          (2-byte) short address to set
 */
void at86rf215_set_addr_short(at86rf215_t *dev, uint8_t filter, uint16_t addr);

/**
 * @brief   Get whether a frame filter is enabled or not
 *
 * @param[in] dev           device to read from
 * @param[in] filter        (1-byte) filter to get
 *
 * @return                  (bool) the current state of the filter
 */
bool at86rf215_get_framefilter_enabled(at86rf215_t *dev, uint8_t filter);

/**
 * @brief   Enables a frame filter
 *
 * @param[in] dev           device to read from
 * @param[in] filter        (1-byte) filter to get
 *
 */
 void at86rf215_disable_framefilter(at86rf215_t *dev, uint8_t filter);

/**
 * @brief   Disables a frame filter
 *
 * @param[in] dev           device to read from
 * @param[in] filter        (1-byte) filter to get
 *
 */
void at86rf215_enable_framefilter(at86rf215_t *dev, uint8_t filter);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (8-byte) long address
 */
uint64_t at86rf215_get_addr_long(const at86rf215_t *dev);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void at86rf215_set_addr_long(at86rf215_t *dev, uint64_t addr);

/**
 * @brief   Get the configured channel number of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel number
 */
uint8_t at86rf215_get_chan(const at86rf215_t *dev);

/**
 * @brief   Set the channel number of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] chan          channel number to set
 */
void at86rf215_set_chan(at86rf215_t *dev, uint16_t chan);

/**
 * @brief   Get the configured PAN ID of the given device from multi address filter
 *
 * @param[in] dev           device to read from
 * @param[in] filter        address filter to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t at86rf215_get_pan(const at86rf215_t *dev, uint8_t filter);

/**
 * @brief   Set the PAN ID of the given address filter
 *
 * @param[in,out] dev       device to write to
 * @param[in] filter        address filter to set
 * @param[in] pan           PAN ID to set
 */
void at86rf215_set_pan(at86rf215_t *dev, uint8_t filter, uint16_t pan);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t at86rf215_get_txpower(const at86rf215_t *dev);

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
void at86rf215_set_txpower(const at86rf215_t *dev, int16_t txpower);

/**
 * @brief   Get the CCA threshold value
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the current CCA threshold value
 */
int8_t at86rf215_get_cca_threshold(const at86rf215_t *dev);

/**
 * @brief   Set the CCA threshold value
 *
 * @param[in] dev           device to write to
 * @param[in] value         the new CCA threshold value
 */
void at86rf215_set_cca_threshold(at86rf215_t *dev, int8_t value);

/**
 * @brief   Get the latest ED level measurement
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the last ED level
 */
int8_t at86rf215_get_ed_level(at86rf215_t *dev);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in,out] dev       device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void at86rf215_set_option(at86rf215_t *dev, uint16_t option, bool state);

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
 * @return                  or negative error code
 */
ssize_t at86rf215_send(at86rf215_t *dev, const void *data, size_t len);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in,out] dev       device to prepare for sending
 *
 * @return                  0 on success, error otherwise
 */
int at86rf215_tx_prepare(at86rf215_t *dev);

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
size_t at86rf215_tx_load(at86rf215_t *dev, const uint8_t *data,
                         size_t len, size_t offset);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 *
 * @return                  0 on success, error otherwise
 */
int at86rf215_tx_exec(at86rf215_t *dev);

/**
 * @brief   Abort sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to abort TX on
 */
void at86rf215_tx_abort(at86rf215_t *dev);

/**
 * @brief   Signal that the transfer of the frame (and optional ACK reception)
 *          has finished. Sets the radio in RX mode.
 *
 * @param[in] dev           device to use
 */
void at86rf215_tx_done(at86rf215_t *dev);

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
bool at86rf215_cca(at86rf215_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF215_H */
/** @} */
