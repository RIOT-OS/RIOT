/*
 * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften Hamburg
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
 * @author      neo@nenaco.de
 */

#ifndef MRF24J40_H_
#define MRF24J40_H_

#include <stdint.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev2.h"
#include "net/netdev2/ieee802154.h"
#include "net/gnrc/nettype.h"

#ifdef __cplusplus
extern "C" {
#endif




/**
 * @brief   Maximum possible packet size in byte
 */
#define MRF24J40_MAX_PKT_LENGTH        (127)


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
 * @brief   Default PAN ID
 *
 * @todo    Read some global network stack specific configuration value
 */
#define MRF24J40_DEFAULT_PANID         (0x0023)


/**
 * @brief   Default TX power (0dBm)
 *  0 -> -36dB
 *  1 -> -35dB
 *  2 -> -34dB
 *  3 -> -33dB
 *  4 -> -32dB
 *  5 -> -31dB
 *  6 -> -30dB
 *  7 -> -30dB
 *  8 -> -26dB
 *  9 -> -25dB
 *  10 -> -24dB
 *  11 -> -23dB
 *  12 -> -22dB
 *  13 -> -21dB
 *  14 -> -20dB
 *  15 -> -20dB
 *  16 -> -16dB
 *  17 -> -15dB
 *  18 -> -14dB
 *  19 -> -13dB
 *  20 -> -12dB
 *  21 -> -11dB
 *  22 -> -10dB
 *  23 -> -10dB
 *  24 -> -6dB
 *  25 -> -5dB
 *  26 -> -4dB
 *  27 -> -3dB
 *  28 -> -2dB
 *  29 -> -1dB
 *  30 -> -0dB
 *  31 -> -0dB
 */

#define MRF24J40_DEFAULT_TXPOWER       (31U)

/**
 * @brief   Base (minimal) RSSI value in dBm
 */
#define RSSI_BASE_VAL                   (-91)

/**
 * @brief   Flags for PSEUDO DEVICE INTERNAL STATES
 * @{
 */
#define MRF24J40_PSEUDO_STATE_TRX_OFF      (0x0)        /**< idle */
#define MRF24J40_PSEUDO_STATE_PLL_ON       (0x1)        /**< ready to transmit */
#define MRF24J40_PSEUDO_STATE_SLEEP        (0x2)        /**< sleep mode */
#define MRF24J40_PSEUDO_STATE_BUSY_RX_AACK (0x3)        /**< busy receiving data */
#define MRF24J40_PSEUDO_STATE_BUSY_TX_ARET (0x4)        /**< busy transmitting data */
#define MRF24J40_PSEUDO_STATE_RX_AACK_ON   (0x5)        /**< wait for incoming data */
#define MRF24J40_PSEUDO_STATE_RX_AACK_OFF  (0x6)        /**< wait for incoming data, no automatic Acknowledge */
#define MRF24J40_PSEUDO_STATE_TX_ARET_ON   (0x7)        /**< ready for sending data */
#define MRF24J40_PSEUDO_STATE_TX_ARET_OFF  (0x8)        /**< ready for sending data */
#define MRF24J40_PSEUDO_STATE_IN_PROGRESS  (0x9)        /**< ongoing state conversion */
#define MRF24J40_PSEUDO_STATE_BUSY_TX_ARET_OFF (0xa)    /**< busy transmitting data */
/** @} */

/**
 * @brief   Internal device option flags
 *
 * `0x00ff` is reserved for general IEEE 802.15.4 flags
 * (see @ref netdev2_ieee802154_t)
 *
 * @{
 */
//#define AT86RF2XX_OPT_SRC_ADDR_LONG  (NETDEV2_IEEE802154_SRC_MODE_LONG) /**< legacy define */
//#define AT86RF2XX_OPT_RAWDUMP        (NETDEV2_IEEE802154_RAW)           /**< legacy define */
#define MRF24J40_OPT_AUTOACK         (NETDEV2_IEEE802154_ACK_REQ)   /**< legacy define */

#define MRF24J40_OPT_CSMA           (0x0100)                        /**< CSMA active */
#define MRF24J40_OPT_PROMISCUOUS    (0x0200)                        /**< promiscuous mode active */
#define MRF24J40_OPT_PRELOADING     (0x0400)                        /**< preloading enabled */
#define MRF24J40_OPT_TELL_TX_START  (0x0800)                        /**< notify MAC layer on TX start */
#define MRF24J40_OPT_TELL_TX_END    (0x1000)                        /**< notify MAC layer on TX finished */
#define MRF24J40_OPT_TELL_RX_START  (0x2000)                        /**< notify MAC layer on RX start */
#define MRF24J40_OPT_TELL_RX_END    (0x4000)                        /**< notify MAC layer on RX */


/**
 * @brief struct holding all params needed for device initialization
 */
typedef struct mrf24j40_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_speed_t spi_speed;  /**< SPI speed to use */
    gpio_t cs_pin;          /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t sleep_pin;       /**< GPIO pin connected to the sleep pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} mrf24j40_params_t;

/**
 * @brief   Device descriptor for MRF24J40 radio devices
 */
typedef struct {
    netdev2_ieee802154_t netdev;            /**< netdev2 parent struct */
    /**
     * @brief   device specific fields
     * @{
     */
    mrf24j40_params_t params;               /**< parameters for initialization */
    uint8_t state;                          /**< current state of the radio */
    uint8_t tx_frame_len;                   /**< length of the current TX frame */
    uint8_t idle_state;                     /**< state to return to after sending */
    uint8_t pending_tx;                     /**< keep track of pending TX calls
                                               this is required to know when to
                                               return to @ref mrf24j40_t::idle_state */
} mrf24j40_t;



/**
 * @brief   Setup an MRF24J40 based device state
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       parameters for device initialization
 */
void mrf24j40_setup(mrf24j40_t *dev, const mrf24j40_params_t *params);

/**
 * @brief enables interrupts
 */
void mrf24j40_set_interrupts(mrf24j40_t *dev);

/**
 * @brief clears interrupt flags
 */
void mrf24j40_clear_irq_flags(mrf24j40_t *dev);


//int mrf24j40_init(mrf24j40_t *dev, spi_t spi, spi_speed_t spi_speed,
//                   gpio_t cs_pin, gpio_t int_pin,
//                   gpio_t sleep_pin, gpio_t reset_pin);

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
void mrf24j40_set_csma_seed(mrf24j40_t * dev, uint8_t entropy[2]);

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

/**
 * @brief   Configures the PHY of the radio device / sets channel / Transmit-Power / delay
 *
 * @param dev               device to read from
 *
 * @return                  overall length of a received packet in byte
 */
void mrf24j40_configure_phy(mrf24j40_t *dev);


/**
 * @brief   Checks the content of th transit normal fifo
 *
 * @param dev               device to read from
 */
void mrf24j40_testread_tx_norm_buf(void);

/**
 * @brief   Initialize the MRF24J40 Interrupt / Port and Pin of uC
 *
 * @param dev               device to read from
 */
void init_mrf24j40_int(void);


#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_H_ */
/** @} */
