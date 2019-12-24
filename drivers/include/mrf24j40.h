/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_mrf24j40 MRF24J40 based drivers
 * @ingroup     drivers_netdev
 *
 * This module contains drivers for radio devices in Microchip MRF24J40 series.
 * The driver is aimed to work with all devices of this series.
 *
 * Default TX power is 0dBm.
 *
 * TX power mapping:
 * * 0 -> -36dB
 * * 1 -> -35dB
 * * 2 -> -34dB
 * * 3 -> -33dB
 * * 4 -> -32dB
 * * 5 -> -31dB
 * * 6 -> -30dB
 * * 7 -> -30dB
 * * 8 -> -26dB
 * * 9 -> -25dB
 * * 10 -> -24dB
 * * 11 -> -23dB
 * * 12 -> -22dB
 * * 13 -> -21dB
 * * 14 -> -20dB
 * * 15 -> -20dB
 * * 16 -> -16dB
 * * 17 -> -15dB
 * * 18 -> -14dB
 * * 19 -> -13dB
 * * 20 -> -12dB
 * * 21 -> -11dB
 * * 22 -> -10dB
 * * 23 -> -10dB
 * * 24 -> -6dB
 * * 25 -> -5dB
 * * 26 -> -4dB
 * * 27 -> -3dB
 * * 28 -> -2dB
 * * 29 -> -1dB
 * * 30 -> -0dB
 * * 31 -> -0dB
 *
 * @{
 *
 * @file
 * @brief       Interface definition for MRF24J40 based drivers
 *
 * @author      Neo Nenaco <neo@nenaco.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MRF24J40_H
#define MRF24J40_H

#include <stdint.h>

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
 * @name    Flags for pseudo device internal states
 * @{
 */
#define MRF24J40_PSEUDO_STATE_IDLE      (0x01)      /**< Idle, ready to transmit or receive */
#define MRF24J40_PSEUDO_STATE_SLEEP     (0x02)      /**< sleep mode, registers functional, but no RF */
#define MRF24J40_PSEUDO_STATE_RESET     (0x04)      /**< Reset device, next state is idle */
/** @} */

/**
 * @name    Internal device option flags
 *
 * `0x00ff` is reserved for general IEEE 802.15.4 flags
 * (see @ref netdev_ieee802154_t)
 *
 * @{
 */
#define MRF24J40_OPT_CSMA               (0x0100)    /**< CSMA active */
#define MRF24J40_OPT_PROMISCUOUS        (0x0200)    /**< promiscuous mode
                                                     *   active */
#define MRF24J40_OPT_PRELOADING         (0x0400)    /**< preloading enabled */
#define MRF24J40_OPT_TELL_TX_START      (0x0800)    /**< notify MAC layer on TX
                                                     *   start */
#define MRF24J40_OPT_TELL_TX_END        (0x1000)    /**< notify MAC layer on TX
                                                     *   finished */
#define MRF24J40_OPT_TELL_RX_START      (0x2000)    /**< notify MAC layer on RX
                                                     *   start */
#define MRF24J40_OPT_TELL_RX_END        (0x4000)    /**< notify MAC layer on RX
                                                     *   finished */
#define MRF24J40_OPT_REQ_AUTO_ACK       (0x8000)    /**< notify MAC layer on RX
                                                     *   finished */
/** @} */


#define MRF24J40_TASK_TX_DONE           (0x01)      /**< TX operation is done */
#define MRF24J40_TASK_TX_READY          (0x02)      /**< TX operation results ready for processing */
#define MRF24J40_TASK_RX_READY          (0x04)      /**< RX processing needed */

#define MRF24J40_MAX_FRAME_RETRIES      (3U)        /**< Number of frame retries (fixed) */

/**
 * @defgroup drivers_mrf24j40_config     mrf24j40 driver compile configuration
 * @ingroup drivers_mrf24j40
 * @ingroup config
 * @{
 */

/**
 * @brief Enable external PA/LNA control
 *
 * Increase RSSI for MRF24J40MC/MD/ME devices. No effect on MRF24J40MA.
 * For more information, please refer to section 4.2 of MRF24J40 datasheet.
 */
#ifndef MRF24J40_USE_EXT_PA_LNA
#define MRF24J40_USE_EXT_PA_LNA         (0U)
#endif

/**
 * @brief Enable basic self-test on init
 *
 * Perform a write / read to a known register on startup to detect
 * if the device is connected.
 * Enable this if you want the boot not to hang if the device is
 * not connected / there are SPI errors.
 */
#ifndef MRF24J40_TEST_SPI_CONNECTION
#define MRF24J40_TEST_SPI_CONNECTION    (0U)
#endif
/** @} */

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct mrf24j40_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} mrf24j40_params_t;

/**
 * @brief   Device descriptor for MRF24J40 radio devices
 */
typedef struct {
    netdev_ieee802154_t netdev;             /**< netdev parent struct */
    /*  device specific fields  */
    mrf24j40_params_t params;               /**< parameters for initialization */
    uint8_t state;                          /**< current state of the radio */
    uint8_t idle_state;                     /**< state to return to after sending */
    uint8_t tx_frame_len;                   /**< length of the current TX frame */
    uint8_t header_len;                     /**< length of the header */
    uint8_t fcf_low;                        /**< Low 8 FCF bits of the current TX frame. */
    uint8_t pending;                        /**< Flags for pending tasks */
    uint8_t irq_flag;                       /**< Flags for IRQs */
    uint8_t tx_retries;                     /**< Number of retries needed for last transmission */
} mrf24j40_t;

/**
 * @brief   Setup an MRF24J40 based device state
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       parameters for device initialization
 */
void mrf24j40_setup(mrf24j40_t *dev, const mrf24j40_params_t *params);

/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in] dev           device to reset
 *
 * @return                  0 on success, error otherwise
 */
int mrf24j40_reset(mrf24j40_t *dev);

/**
 * @brief   Trigger a clear channel assessment & retrieve RSSI
 *
 * @param[in] dev           device to use
 * @param[in] rssi          RSSI value from register in dBm
 *
 * @return                  true if channel is clear
 * @return                  false if channel is busy
 */
bool mrf24j40_cca(mrf24j40_t *dev, int8_t *rssi);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (2-byte) short address
 */
uint16_t mrf24j40_get_addr_short(mrf24j40_t *dev);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void mrf24j40_set_addr_short(mrf24j40_t *dev, uint16_t addr);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (8-byte) long address
 */
uint64_t mrf24j40_get_addr_long(mrf24j40_t *dev);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void mrf24j40_set_addr_long(mrf24j40_t *dev, uint64_t addr);

/**
 * @brief   Get the configured channel number of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel number
 */
uint8_t mrf24j40_get_chan(mrf24j40_t *dev);

/**
 * @brief   Set the channel number of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] chan          channel number to set
 */
void mrf24j40_set_chan(mrf24j40_t *dev, uint8_t chan);

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t mrf24j40_get_pan(mrf24j40_t *dev);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] pan           PAN ID to set
 */
void mrf24j40_set_pan(mrf24j40_t *dev, uint16_t pan);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t mrf24j40_get_txpower(mrf24j40_t *dev);

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
void mrf24j40_set_txpower(mrf24j40_t *dev, int16_t txpower);

/**
 * @brief   Get the maximum number of channel access attempts per frame (CSMA)
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retries
 */
uint8_t mrf24j40_get_csma_max_retries(mrf24j40_t *dev);

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
void mrf24j40_set_csma_max_retries(mrf24j40_t *dev, int8_t retries);

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
void mrf24j40_set_csma_backoff_exp(mrf24j40_t *dev, uint8_t min, uint8_t max);

/**
 * @brief   Get the CCA threshold value
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the current CCA threshold value
 */
int8_t mrf24j40_get_cca_threshold(mrf24j40_t *dev);

/**
 * @brief   Set the CCA threshold value
 *
 * @param[in] dev           device to write to
 * @param[in] value         the new CCA threshold value
 */
void mrf24j40_set_cca_threshold(mrf24j40_t *dev, int8_t value);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in] dev           device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void mrf24j40_set_option(mrf24j40_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in] dev           device to change state of
 * @param[in] state         the targeted new state
 */
void mrf24j40_set_state(mrf24j40_t *dev, uint8_t state);

/**
 * @brief   Put in sleep mode
 *
 * @param[in] dev       device to put to sleep
 */
void mrf24j40_sleep(mrf24j40_t *dev);

/**
 * @brief   Put in sleep mode if idle_state is sleep
 *
 * @param[in] dev       device to put to sleep
 */
void mrf24j40_assert_sleep(mrf24j40_t *dev);

/**
 * @brief   Wake up from sleep mode
 *
 * @param[in] dev       device to eventually wake up
 */
void mrf24j40_assert_awake(mrf24j40_t *dev);

/**
 * @brief   Reset the internal state machine to TRX_OFF mode.
 *
 * This will force a transition to TRX_OFF regardless of whether the transceiver
 * is currently busy sending or receiving. This function is used to get back to
 * a known state during driver initialization.
 *
 * @param[in] dev           device to operate on
 */
void mrf24j40_reset_state_machine(mrf24j40_t *dev);

/**
 * @brief   Software Reset.
 *
 * This will force the power management circuitry, the baseband circuitry and the MAC circuitry
 * to be reset
 *
 * @param[in] dev           device to operate on
 */
void mrf24j40_software_reset(mrf24j40_t *dev);

/**
 * @brief   Convert scalar from mrf24j40 RSSI to dBm
 *
 * @param[in] value         value to convert to dBm
 * @return                  converted value in dBm
 */
int8_t mrf24j40_dbm_from_reg(uint8_t value);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in] dev            device to prepare for sending
 */
void mrf24j40_tx_prepare(mrf24j40_t *dev);

/**
 * @brief   Load chunks of data into the transmit buffer of the given device
 *
 * @param[in] dev           device to write data to
 * @param[in] data          buffer containing the data to load
 * @param[in] len           number of bytes in @p buffer
 * @param[in] offset        offset used when writing data to internal buffer
 *
 * @return                  offset + number of bytes written
 */
size_t mrf24j40_tx_load(mrf24j40_t *dev, uint8_t *data, size_t len,
                        size_t offset);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
void mrf24j40_tx_exec(mrf24j40_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_H */
/** @} */
