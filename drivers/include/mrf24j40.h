/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @{
 *
 * @file
 * @brief       Interface definition for MRF24J40 based drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef MRF24J40_H_
#define MRF24J40_H_

#include <stdint.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/gnrc/netdev.h"
#include "xtimer.h"


#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief   Maximum possible packet size in byte
 */
//#define MRF24J40_MAX_PKT_LENGTH


/**
 * @brief   Default addresses used if the CPUID module is not present
 * @{
 */
#define MRF24J40_DEFAULT_ADDR_SHORT    (0x0230)
#define MRF24J40_DEFAULT_ADDR_LONG     (0x1222334455667788)
/** @} */

/**
  * @brief   Channel configuration
  * @{
  */
#define MRF24J40_MIN_CHANNEL           (11U)
#define MRF24J40_MAX_CHANNEL           (26U)
#define MRF24J40_DEFAULT_CHANNEL       (26U)
/** @} */

/**
 * @brief   Device descriptor for MRF24J40 radio devices
 */
typedef struct {
    /* netdev fields */
    const gnrc_netdev_driver_t *driver; /**< pointer to the devices interface */
    gnrc_netdev_event_cb_t event_cb;    /**< netdev event callback */
    kernel_pid_t mac_pid;               /**< the driver's thread's PID */
    /* device specific fields */
    spi_t spi;                          /**< used SPI device */
    gpio_t cs_pin;                      /**< chip select pin */
    gpio_t sleep_pin;                   /**< sleep pin */
    gpio_t reset_pin;                   /**< reset pin */
    gpio_t int_pin;                     /**< external interrupt pin */
    gnrc_nettype_t proto;               /**< protocol the radio expects */
    uint8_t state;                      /**< current state of the radio */
    uint8_t seq_nr;                     /**< sequence number to use next */
    uint8_t frame_len;                  /**< length of the current TX frame */
    uint16_t pan;                       /**< currently used PAN ID */
    uint8_t chan;                       /**< currently used channel */
    uint8_t addr_short[2];              /**< the radio's short address */
    uint8_t addr_long[8];               /**< the radio's long address */
    uint16_t options;                   /**< state of used options */
    uint8_t idle_state;                 /**< state to return to after sending */
} mrf24j40_t;


/**
 * @brief struct holding all params needed for device initialization
 */
typedef struct mrf24j40_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_speed_t spi_speed;  /**< SPI speed to use */
    gpio_t cs_pin;          /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t sleep_pin;        /**< GPIO pin connected to the sleep pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} mrf24j40_params_t;

/**
 * @brief enables interrupts
 */
void mrf24j40_set_interrupts(mrf24j40_t *dev);

/**
 * @brief clears interrupt flags
 */
void mrf24j40_clear_irq_flags(mrf24j40_t *dev);

/**
 * @brief   Default PAN ID
 *
 * @todo    Read some global network stack specific configuration value
 */
#define MRF24J40_DEFAULT_PANID         (0x0023)

/**
 * @brief   Default TX power (0dBm)
 */
#define MRF24J40_DEFAULT_TXPOWER       (0U)

/**
 * @brief   Base (minimal) RSSI value in dBm
 */
#define RSSI_BASE_VAL                   (-91)



/**
 * @brief   Initialize a given MRF24J40 device
 *
 * @param[out] dev          device descriptor
 * @param[in] spi           SPI bus the device is connected to
 * @param[in] spi_speed     SPI speed to use
 * @param[in] cs_pin        GPIO pin connected to chip select
 * @param[in] int_pin       GPIO pin connected to the interrupt pin
 * @param[in] sleep_pin      GPIO pin connected to the sleep pin
 * @param[in] reset_pin     GPIO pin connected to the reset pin
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mrf24j40_init(mrf24j40_t *dev, spi_t spi, spi_speed_t spi_speed,
                   gpio_t cs_pin, gpio_t int_pin,
                   gpio_t sleep_pin, gpio_t reset_pin);

/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in] dev           device to reset
 */
void mrf24j40_reset(mrf24j40_t *dev);

/**
 * @brief   Trigger a clear channel assessment
 *
 * @param[in] dev           device to use
 *
 * @return                  true if channel is clear
 * @return                  false if channel is busy
 */
bool mrf24j40_cca(mrf24j40_t *dev);

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
 * @brief   Get the configured channel page of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel page
 */
uint8_t mrf24j40_get_page(mrf24j40_t *dev);

/**
 * @brief   Set the channel page of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] page          channel page to set
 */
void mrf24j40_set_page(mrf24j40_t *dev, uint8_t page);

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
 * @brief   Get the maximum number of retransmissions
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retransmissions
 */
uint8_t mrf24j40_get_max_retries(mrf24j40_t *dev);

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
void mrf24j40_set_max_retries(mrf24j40_t *dev, uint8_t max);

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
 * @brief   Set seed for CSMA random backoff
 *
 * @param[in] dev           device to write to
 * @param[in] entropy       11 bit of entropy as seed for random backoff
 */
void mrf24j40_set_csma_seed(mrf24j40_t *dev, uint8_t entropy[2]);

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
//void mrf24j40_set_option(mrf24j40_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in] dev           device to change state of
 * @param[in] state         the targeted new state
 */
//void mrf24j40_set_state(mrf24j40_t *dev, uint8_t state);

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
 * @brief   Convenience function for simply sending data
 *
 * @note This function ignores the PRELOADING option
 *
 * @param[in] dev           device to use for sending
 * @param[in] data          data to send (must include IEEE802.15.4 header)
 * @param[in] len           length of @p data
 *
 * @return                  number of bytes that were actually send
 * @return                  0 on error
 */
size_t mrf24j40_send(mrf24j40_t *dev, uint8_t *data, size_t len);

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

/**
 * @brief   Read the length of a received packet
 *
 * @param dev               device to read from
 *
 * @return                  overall length of a received packet in byte
 */
size_t mrf24j40_rx_len(mrf24j40_t *dev);

/**
 * @brief   Read a chunk of data from the receive buffer of the given device
 *
 * @param[in]  dev          device to read from
 * @param[out] data         buffer to write data to
 * @param[in]  len          number of bytes to read from device
 * @param[in]  offset       offset in the receive buffer
 */
void mrf24j40_rx_read(mrf24j40_t *dev, uint8_t *data, size_t len,
                       size_t offset);

#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_H_ */
/** @} */
