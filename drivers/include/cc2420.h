/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_cc2420 CC2420 driver
 * @ingroup     drivers
 *
 * This module contains the driver for TI CC2420 radio devices.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for CC2420 based drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef CC2420_H_
#define CC2420_H_

#include <stdint.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum possible packet size in byte
 */
#define CC2420_MAX_PKT_LENGTH     (127)

/**
 * @brief   Default addresses used if the CPUID module is not present
 * @{
 */
#define CC2420_DEFAULT_ADDR_SHORT (0x0230)
#define CC2420_DEFAULT_ADDR_LONG  (0x1222334455667788)
/** @} */

/**
  * @brief   Channel configuration
  * @{
  */
#define CC2420_MIN_CHANNEL        (11U)
#define CC2420_MAX_CHANNEL        (26U)
#define CC2420_DEFAULT_CHANNEL    (26U)
/** @} */

/**
 * @brief   Default PAN ID
 *
 * TODO: Read some global network stack specific configuration value
 */
#define CC2420_DEFAULT_PANID      (0x0023)

/**
 * @brief   Default TX power (0dBm)
 */
#define CC2420_DEFAULT_TXPOWER    (0U)

/**
 * @brief   Flags for device internal states (see datasheet)
 * @{
 */
#define CC2420_STATE_IDLE         (0x01)     /**< idle state -> IDLE */
#define CC2420_STATE_P_DOWN       (0x02)     /**< power down state -> PD */
#define CC2420_STATE_RX_ON        (0x04)     /**< ready to receive -> RX_SFD_SEARCH*/
#define CC2420_STATE_RX_BUSY      (0x08)     /**< busy receiving data -> all other rx states */
#define CC2420_STATE_TX_PREP      (0x10)     /**< prepare for sending data
                                              * -> driver state for preloading */
#define CC2420_STATE_TX_BUSY      (0x20)     /**< busy transmitting data */
#define CC2420_STATE_IN_PROGRESS  (0x40)     /**< ongoing state change */
/** @} */

/**
 * @brief   Internal device option flags
 * @{
 */
#define CC2420_OPT_AUTOACK        (0x0001)    /**< auto ACKs active */
#define CC2420_OPT_CSMA           (0x0002)    /**< CSMA active */
#define CC2420_OPT_PROMISCUOUS    (0x0004)    /**< promiscuous mode
                                               *   active */
#define CC2420_OPT_PRELOADING     (0x0008)    /**< preloading enabled */
#define CC2420_OPT_TELL_TX_START  (0x0010)    /**< notify MAC layer on TX
                                               *   start */
#define CC2420_OPT_TELL_TX_END    (0x0020)    /**< notify MAC layer on TX
                                               *   finished */
#define CC2420_OPT_TELL_RX_START  (0x0040)    /**< notify MAC layer on RX
                                               *   start */
#define CC2420_OPT_TELL_RX_END    (0x0080)    /**< notify MAC layer on RX
                                               *   finished */
#define CC2420_OPT_RAWDUMP        (0x0100)    /**< pass RAW frame data to
                                               *   upper layer */
#define CC2420_OPT_SRC_ADDR_LONG  (0x0200)    /**< send data using long
                                               *   source address */
#define CC2420_OPT_USE_SRC_PAN    (0x0400)    /**< do not compress source
                                               *   PAN ID */
/** @} */

/**
 * @brief   Device descriptor for CC2420 radio devices
 */
typedef struct {
    /* netdev fields */
    const gnrc_netdev_driver_t *driver; /**< pointer to the devices interface */
    gnrc_netdev_event_cb_t event_cb;    /**< netdev event callback */
    kernel_pid_t mac_pid;               /**< the driver's thread's PID */
    /* device specific fields */
    spi_t spi;                          /**< used SPI device */
    gpio_t cs_pin;                      /**< chip select pin */
    gpio_t reset_pin;                   /**< reset pin */
    gpio_t power_pin;                   /**< reset pin */
    gpio_t fifo_int_pin;                /**< external interrupt pin */
    gpio_t fifop_int_pin;               /**< external interrupt pin */
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
} cc2420_t;

/**
 * @brief struct holding all params needed for device initialization
 */
typedef struct cc2420_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_speed_t spi_speed;  /**< SPI speed to use */
    gpio_t cs_pin;          /**< GPIO pin connected to chip select */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
    gpio_t power_pin;       /**< GPIO pin connected to the vregen pin */
    gpio_t fifo_int_pin;    /**< GPIO pin connected to the FIFO interrupt pin */
    gpio_t fifop_int_pin;   /**< GPIO pin connected to the FIFOP interrupt pin */
} cc2420_params_t;

/**
 * @brief   Initialize a given CC2420 device
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       parameters for initialization `cc2420_params_t`
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int cc2420_init(cc2420_t *dev, const cc2420_params_t *params);

/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in] dev           device to reset
 */
void cc2420_reset(cc2420_t *dev);

/**
 * @brief   Trigger a clear channel assessment
 *
 * @param[in] dev           device to use
 *
 * @return                  true if channel is clear
 * @return                  false if channel is busy
 */
bool cc2420_cca(cc2420_t *dev);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (2-byte) short address
 */
uint16_t cc2420_get_addr_short(cc2420_t *dev);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void cc2420_set_addr_short(cc2420_t *dev, uint16_t addr);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (8-byte) long address
 */
uint64_t cc2420_get_addr_long(cc2420_t *dev);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void cc2420_set_addr_long(cc2420_t *dev, uint64_t addr);

/**
 * @brief   Get the configured channel of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel
 */
uint8_t cc2420_get_chan(cc2420_t *dev);

/**
 * @brief   Set the channel of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] chan          channel to set
 */
void cc2420_set_chan(cc2420_t *dev, uint8_t chan);

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t cc2420_get_pan(cc2420_t *dev);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] pan           PAN ID to set
 */
void cc2420_set_pan(cc2420_t *dev, uint16_t pan);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t cc2420_get_txpower(cc2420_t *dev);

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
void cc2420_set_txpower(cc2420_t *dev, int16_t txpower);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in] dev           device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void cc2420_set_option(cc2420_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in] dev           device to change state of
 * @param[in] state         the targeted new state
 */
void cc2420_set_state(cc2420_t *dev, uint8_t state);

/**
 * @brief   Get the state of the given device
 *
 * @param[in] dev           device to change state of
 *
 * @return                  the device's current state
 */
uint8_t cc2420_get_state(cc2420_t *dev);

/**
 * @brief   Reset the internal state machine to TRX_OFF mode.
 *
 * This will force a transition to TRX_OFF regardless of whether the transceiver
 * is currently busy sending or receiving. This function is used to get back to
 * a known state during driver initialization.
 *
 * @param[in] dev           device to operate on
 */
void cc2420_reset_state_machine(cc2420_t *dev);

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
size_t cc2420_send(cc2420_t *dev, uint8_t *data, size_t len);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in] dev            device to prepare for sending
 */
void cc2420_tx_prepare(cc2420_t *dev);

/**
 * @brief   Load chunks of data into the transmit buffer of the given device
 *
 * @param[in] dev           device to write data to
 * @param[in] data          buffer containing the data to load
 * @param[in] len           number of bytes in @p buffer
 *
 * @return                  offset + number of bytes written
 */
size_t cc2420_tx_load(cc2420_t *dev, uint8_t *data, size_t len);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
void cc2420_tx_exec(cc2420_t *dev);

/**
 * @brief   Read the length of a received packet
 *
 * @param dev               device to read from
 *
 * @return                  overall length of a received packet in byte
 */
size_t cc2420_rx_len(cc2420_t *dev);

/**
 * @brief   Read a chunk of data from the receive buffer of the given device
 *
 * @param[in]  dev          device to read from
 * @param[out] data         buffer to write data to
 * @param[in]  len          number of bytes to read from device
 * @param[in]  offset       offset in the receive buffer
 */
void cc2420_rx_read(cc2420_t *dev, uint8_t *data, size_t len,
                       size_t offset);

#ifdef __cplusplus
}
#endif

#endif /* CC2420_H_ */
/** @} */
