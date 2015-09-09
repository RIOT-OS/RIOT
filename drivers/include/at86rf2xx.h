/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_at86rf2xx AT86RF2xx based drivers
 * @ingroup     drivers
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
 */

#ifndef AT86RF2XX_H_
#define AT86RF2XX_H_

#include <stdint.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/gnrc/netdev.h"
#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum possible packet size in byte
 */
#define AT86RF2XX_MAX_PKT_LENGTH        (127)

/**
 * @brief   Default addresses used if the CPUID module is not present
 * @{
 */
#define AT86RF2XX_DEFAULT_ADDR_SHORT    (0x0230)
#define AT86RF2XX_DEFAULT_ADDR_LONG     (0x1222334455667788)
/** @} */

/**
  * @brief   Channel configuration
  * @{
  */
#ifdef MODULE_AT86RF212B
/* the AT86RF212B has a sub-1GHz radio */
#define AT86RF2XX_MIN_CHANNEL           (0)
#define AT86RF2XX_MAX_CHANNEL           (10)
#define AT86RF2XX_DEFAULT_CHANNEL       (5)
#else
#define AT86RF2XX_MIN_CHANNEL           (11U)
#define AT86RF2XX_MAX_CHANNEL           (26U)
#define AT86RF2XX_DEFAULT_CHANNEL       (26U)
#endif
/** @} */

/**
 * @brief   Default PAN ID
 *
 * @todo    Read some global network stack specific configuration value
 */
#define AT86RF2XX_DEFAULT_PANID         (0x0023)

/**
 * @brief   Default TX power (0dBm)
 */
#define AT86RF2XX_DEFAULT_TXPOWER       (0U)

/**
 * @brief   Flags for device internal states (see datasheet)
 * @{
 */
#define AT86RF2XX_STATE_TRX_OFF      (0x08)     /**< idle */
#define AT86RF2XX_STATE_PLL_ON       (0x09)     /**< ready to transmit */
#define AT86RF2XX_STATE_SLEEP        (0x0f)     /**< sleep mode */
#define AT86RF2XX_STATE_BUSY_RX_AACK (0x11)     /**< busy receiving data */
#define AT86RF2XX_STATE_BUSY_TX_ARET (0x12)     /**< busy transmitting data */
#define AT86RF2XX_STATE_RX_AACK_ON   (0x16)     /**< wait for incoming data */
#define AT86RF2XX_STATE_TX_ARET_ON   (0x19)     /**< ready for sending data */
#define AT86RF2XX_STATE_IN_PROGRESS  (0x1f)     /**< ongoing state conversion */
/** @} */

/**
 * @brief   Internal device option flags
 * @{
 */
#define AT86RF2XX_OPT_AUTOACK        (0x0001)       /**< auto ACKs active */
#define AT86RF2XX_OPT_CSMA           (0x0002)       /**< CSMA active */
#define AT86RF2XX_OPT_PROMISCUOUS    (0x0004)       /**< promiscuous mode
                                                     *   active */
#define AT86RF2XX_OPT_PRELOADING     (0x0008)       /**< preloading enabled */
#define AT86RF2XX_OPT_TELL_TX_START  (0x0010)       /**< notify MAC layer on TX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_TX_END    (0x0020)       /**< notify MAC layer on TX
                                                     *   finished */
#define AT86RF2XX_OPT_TELL_RX_START  (0x0040)       /**< notify MAC layer on RX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_RX_END    (0x0080)       /**< notify MAC layer on RX
                                                     *   finished */
#define AT86RF2XX_OPT_RAWDUMP        (0x0100)       /**< pass RAW frame data to
                                                     *   upper layer */
#define AT86RF2XX_OPT_SRC_ADDR_LONG  (0x0200)       /**< send data using long
                                                     *   source address */
#define AT86RF2XX_OPT_USE_SRC_PAN    (0x0400)       /**< do not compress source
                                                     *   PAN ID */
/** @} */

/**
  * @brief   Frequency configuration for sub-GHz devices.
  * @{
  */
typedef enum {
    AT86RF2XX_FREQ_915MHZ,       /**< frequency 915MHz enabled */
    AT86RF2XX_FREQ_868MHZ,       /**< frequency 868MHz enabled */
} at86rf2xx_freq_t;
/** @} */

/**
 * @brief   Device descriptor for AT86RF2XX radio devices
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
#ifdef MODULE_AT86RF212B
    at86rf2xx_freq_t freq;              /**< currently used frequency */
#endif
    uint8_t addr_short[2];              /**< the radio's short address */
    uint8_t addr_long[8];               /**< the radio's long address */
    uint16_t options;                   /**< state of used options */
    uint8_t idle_state;                 /**< state to return to after sending */
} at86rf2xx_t;

/**
 * @brief struct holding all params needed for device initialization
 */
typedef struct at86rf2xx_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_speed_t spi_speed;  /**< SPI speed to use */
    gpio_t cs_pin;          /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t sleep_pin;       /**< GPIO pin connected to the sleep pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} at86rf2xx_params_t;

/**
 * @brief   Initialize a given AT86RF2xx device
 *
 * @param[out] dev          device descriptor
 * @param[in] spi           SPI bus the device is connected to
 * @param[in] spi_speed     SPI speed to use
 * @param[in] cs_pin        GPIO pin connected to chip select
 * @param[in] int_pin       GPIO pin connected to the interrupt pin
 * @param[in] sleep_pin     GPIO pin connected to the sleep pin
 * @param[in] reset_pin     GPIO pin connected to the reset pin
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int at86rf2xx_init(at86rf2xx_t *dev, spi_t spi, spi_speed_t spi_speed,
                   gpio_t cs_pin, gpio_t int_pin,
                   gpio_t sleep_pin, gpio_t reset_pin);

/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in] dev           device to reset
 */
void at86rf2xx_reset(at86rf2xx_t *dev);

/**
 * @brief   Trigger a clear channel assessment
 *
 * @param[in] dev           device to use
 *
 * @return                  true if channel is clear
 * @return                  false if channel is busy
 */
bool at86rf2xx_cca(at86rf2xx_t *dev);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (2-byte) short address
 */
uint16_t at86rf2xx_get_addr_short(at86rf2xx_t *dev);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in] dev           device to write to
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
uint64_t at86rf2xx_get_addr_long(at86rf2xx_t *dev);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void at86rf2xx_set_addr_long(at86rf2xx_t *dev, uint64_t addr);

/**
 * @brief   Get the configured channel of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel
 */
uint8_t at86rf2xx_get_chan(at86rf2xx_t *dev);

/**
 * @brief   Set the channel of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] chan          channel to set
 */
void at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t chan);

#ifdef MODULE_AT86RF212B
/**
 * @brief   Get the configured frequency of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set frequency
 */
at86rf2xx_freq_t at86rf2xx_get_freq(at86rf2xx_t *dev);

/**
 * @brief   Set the frequency of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] chan          frequency to set
 */
void at86rf2xx_set_freq(at86rf2xx_t *dev, at86rf2xx_freq_t freq);
#endif

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t at86rf2xx_get_pan(at86rf2xx_t *dev);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in] dev           device to write to
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
int16_t at86rf2xx_get_txpower(at86rf2xx_t *dev);

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
void at86rf2xx_set_txpower(at86rf2xx_t *dev, int16_t txpower);

/**
 * @brief   Get the maximum number of retransmissions
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retransmissions
 */
uint8_t at86rf2xx_get_max_retries(at86rf2xx_t *dev);

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
void at86rf2xx_set_max_retries(at86rf2xx_t *dev, uint8_t max);

/**
 * @brief   Get the maximum number of channel access attempts per frame (CSMA)
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retries
 */
uint8_t at86rf2xx_get_csma_max_retries(at86rf2xx_t *dev);

/**
 * @brief   Set the maximum number of channel access attempts per frame (CSMA)
 *
 * This setting specifies the number of attempts to access the channel to
 * transmit a frame. If the channel is busy @p retries times, then frame
 * transmission fails.
 * Valid values: 0 to 5, -1 means CSMA disabled
 *
 * @param[in] dev           device to write to
 * @param[in] max           the maximum number of retries
 */
void at86rf2xx_set_csma_max_retries(at86rf2xx_t *dev, int8_t retries);

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
void at86rf2xx_set_csma_backoff_exp(at86rf2xx_t *dev, uint8_t min, uint8_t max);

/**
 * @brief   Set seed for CSMA random backoff
 *
 * @param[in] dev           device to write to
 * @param[in] entropy       11 bit of entropy as seed for random backoff
 */
void at86rf2xx_set_csma_seed(at86rf2xx_t *dev, uint8_t entropy[2]);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in] dev           device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in] dev           device to change state of
 * @param[in] state         the targeted new state
 */
void at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state);

/**
 * @brief   Reset the internal state machine to TRX_OFF mode.
 *
 * This will force a transition to TRX_OFF regardless of whether the transceiver
 * is currently busy sending or receiving. This function is used to get back to
 * a known state during driver initialization.
 *
 * @param[in] dev           device to operate on
 */
void at86rf2xx_reset_state_machine(at86rf2xx_t *dev);

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
size_t at86rf2xx_send(at86rf2xx_t *dev, uint8_t *data, size_t len);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in] dev            device to prepare for sending
 */
void at86rf2xx_tx_prepare(at86rf2xx_t *dev);

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
size_t at86rf2xx_tx_load(at86rf2xx_t *dev, uint8_t *data, size_t len,
                         size_t offset);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
void at86rf2xx_tx_exec(at86rf2xx_t *dev);

/**
 * @brief   Read the length of a received packet
 *
 * @param dev               device to read from
 *
 * @return                  overall length of a received packet in byte
 */
size_t at86rf2xx_rx_len(at86rf2xx_t *dev);

/**
 * @brief   Read a chunk of data from the receive buffer of the given device
 *
 * @param[in]  dev          device to read from
 * @param[out] data         buffer to write data to
 * @param[in]  len          number of bytes to read from device
 * @param[in]  offset       offset in the receive buffer
 */
void at86rf2xx_rx_read(at86rf2xx_t *dev, uint8_t *data, size_t len,
                       size_t offset);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_H_ */
/** @} */
