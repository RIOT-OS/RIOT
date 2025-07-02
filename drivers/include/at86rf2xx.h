/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

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

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "kernel_defines.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/nettype.h"
#include "net/ieee802154/radio.h"
#include "event.h"

/* we need no peripherals for memory mapped radios */
#if !defined(MODULE_AT86RFA1) && !defined(MODULE_AT86RFR2)
#include "periph/spi.h"
#include "periph/gpio.h"
#endif

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
#define AT86RF2XX_DEFAULT_CHANNEL       (CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL)
/* Page 2 is O-QPSK 100 kbit/s (channel 0), or 250 kbit/s (channels 1-10) */
#define AT86RF2XX_DEFAULT_PAGE          (CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE)
#else
#define AT86RF2XX_MIN_CHANNEL           (IEEE802154_CHANNEL_MIN)
#define AT86RF2XX_MAX_CHANNEL           (IEEE802154_CHANNEL_MAX)
#define AT86RF2XX_DEFAULT_CHANNEL       (CONFIG_IEEE802154_DEFAULT_CHANNEL)
#define AT86RF2XX_DEFAULT_PAGE          (0)
/* Only page 0 is supported in the 2.4 GHz band */
#endif
/** @} */

/**
 * @brief   Default TX power (0dBm)
 */
#define AT86RF2XX_DEFAULT_TXPOWER       (CONFIG_IEEE802154_DEFAULT_TXPOWER)

/**
 * @brief   Base (minimal) RSSI value in dBm
 */
#if MODULE_AT86RF233
#   define RSSI_BASE_VAL                   (-94)
#elif MODULE_AT86RF212B
/**
 * @note for the default settings in RIOT for the at86rf212b,
 *       for other settings this value may change.
 */
#   define RSSI_BASE_VAL                   (-98)
#elif MODULE_AT86RFA1 || MODULE_AT86RFR2
#   define RSSI_BASE_VAL                   (-90)
#else
#   define RSSI_BASE_VAL                   (-91)
#endif

/**
 * @brief   Min Receiver sensitivity value in dBm
 */
#if MODULE_AT86RF233
#   define MIN_RX_SENSITIVITY              (-101)
#elif MODULE_AT86RF212B
#   define MIN_RX_SENSITIVITY              (-110)
#elif MODULE_AT86RFA1 || MODULE_AT86RFR2
#   define MIN_RX_SENSITIVITY              (-100)
#else
#   define MIN_RX_SENSITIVITY              (-101)
#endif

/**
 * @brief   Whether there is a periph version of the radio
 */
#if IS_USED(MODULE_AT86RFA1) || IS_USED(MODULE_AT86RFR2)
#define AT86RF2XX_IS_PERIPH (1)
#else
#define AT86RF2XX_IS_PERIPH (0)
#endif

/**
 * @brief   ED Register
 */
#if defined(MODULE_AT86RF231) || IS_ACTIVE(AT86RF2XX_PERIPH)
#define AT86RF2XX_HAVE_ED_REGISTER (1)
#else
#define AT86RF2XX_HAVE_ED_REGISTER (0)
#endif

/**
 * @brief   Support for SubGHz bands
 */
#ifdef MODULE_AT86RF212B
#define AT86RF2XX_HAVE_SUBGHZ (1)
#else
#define AT86RF2XX_HAVE_SUBGHZ (0)
#endif

#if defined(DOXYGEN) || defined(MODULE_AT86RF232) || defined(MODULE_AT86RF233) || defined(MODULE_AT86RFR2)
/**
 * @brief   Frame retry counter reporting
 *
 * The AT86RF2XX_HAVE_RETRIES flag enables support for NETOPT_TX_RETRIES NEEDED
 * operation.
 */
#define AT86RF2XX_HAVE_RETRIES             (1)
#else
#define AT86RF2XX_HAVE_RETRIES             (0)
#endif

/**
 * @brief   Frame retry counter register
 *
 * Some radios include the XAH_CTRL_2 register which contains the frame retry
 * counter. Only the at86rf232 and the at86rf233 support this register.
 */
#if AT86RF2XX_HAVE_RETRIES && defined(AT86RF2XX_REG__XAH_CTRL_2)
#define AT86RF2XX_HAVE_RETRIES_REG         (1)
#else
#define AT86RF2XX_HAVE_RETRIES_REG         (0)
#endif

/**
 * @brief   TX Start IRQ
 */
#ifdef AT86RF2XX_REG__IRQ_MASK1
#define AT86RF2XX_HAVE_TX_START_IRQ        (1)
#else
#define AT86RF2XX_HAVE_TX_START_IRQ        (0)
#endif

/**
 * @brief   Random Number Generator
 *
 * Most AT86RF radios have the option to use the highest bits of the RSSI
 * register as a source of randomness.
 * See Section 11.2 of the at86rf233 reference manual. (RND_VALUE)
 */
#if defined(MODULE_AT86RF233) || defined(MODULE_AT86RF231) || defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
#ifndef AT86RF2XX_RANDOM_NUMBER_GENERATOR
#define AT86RF2XX_RANDOM_NUMBER_GENERATOR  (1)
#endif
#else
#ifndef AT86RF2XX_RANDOM_NUMBER_GENERATOR
#define AT86RF2XX_RANDOM_NUMBER_GENERATOR  (0)
#endif
#endif

/**
 * @brief   Smart idle listening feature
 *
 * This feature optimizes radio operation in the listening mode, reducing
 * current consumption by ~50%. It is supported by only at86rf233. The reference
 * manual recommends to disable this feature for RSSI measurements or random number
 * generation (Section 8.4 and Section 11.2).
 */
#if defined(MODULE_AT86RF233) || defined(MODULE_AT86RFR2)
#ifndef AT86RF2XX_SMART_IDLE_LISTENING
#define AT86RF2XX_SMART_IDLE_LISTENING     (1)
#endif
#else
#define AT86RF2XX_SMART_IDLE_LISTENING     (0)
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
#define AT86RF2XX_OPT_CSMA           (0x0010)       /**< CSMA active */
#define AT86RF2XX_OPT_PROMISCUOUS    (0x0020)       /**< promiscuous mode active */
#define AT86RF2XX_OPT_PRELOADING     (0x0040)       /**< preloading enabled */
#define AT86RF2XX_OPT_AUTOACK        (0x0080)       /**< Auto ACK active */
#define AT86RF2XX_OPT_ACK_PENDING    (0x0100)       /**< ACK frames with data pending */

/** @} */

#if IS_ACTIVE(AT86RF2XX_BASIC_MODE) || defined(DOXYGEN)
/**
 * @brief Internal radio state equivalent to RX_ON
 */
#define AT86RF2XX_PHY_STATE_RX       AT86RF2XX_STATE_RX_ON
/**
 * @brief Internal radio state equivalent to RX_BUSY
 */
#define AT86RF2XX_PHY_STATE_RX_BUSY  AT86RF2XX_STATE_BUSY_RX
/**
 * @brief Internal radio state equivalent to TX_ON
 */
#define AT86RF2XX_PHY_STATE_TX       AT86RF2XX_STATE_PLL_ON
/**
 * @brief Internal radio state equivalent to TX_BUSY
 */
#define AT86RF2XX_PHY_STATE_TX_BUSY  AT86RF2XX_STATE_BUSY_TX
#else
#define AT86RF2XX_PHY_STATE_RX       AT86RF2XX_STATE_RX_AACK_ON
#define AT86RF2XX_PHY_STATE_RX_BUSY  AT86RF2XX_STATE_BUSY_RX_AACK
#define AT86RF2XX_PHY_STATE_TX       AT86RF2XX_STATE_TX_ARET_ON
#define AT86RF2XX_PHY_STATE_TX_BUSY  AT86RF2XX_STATE_BUSY_TX_ARET
#endif /* IS_ACTIVE(AT86RF2XX_BASIC_MODE) */

#if AT86RF2XX_IS_PERIPH
/**
 * @brief   memory mapped radio needs no parameters
 */
typedef void at86rf2xx_params_t;
#else
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
#endif

/**
 * @brief   Device descriptor for AT86RF2XX radio devices
 *
 * @extends netdev_ieee802154_t
 */
typedef struct {
#if !AT86RF2XX_IS_PERIPH
    /* device specific fields */
    at86rf2xx_params_t params;              /**< parameters for initialization */
#endif
#if AT86RF2XX_HAVE_RETRIES && AT86RF2XX_IS_PERIPH
    /* Only radios with the XAH_CTRL_2 register support frame retry reporting */
    int8_t tx_retries;                  /**< Number of NOACK retransmissions */
#endif
    bool sleep;                         /**< whether the device is sleeping or not */
    mutex_t lock;                       /**< device lock */
} at86rf2xx_t;

/**
 * @brief Event Bottom Half Processor descriptor for AT86RF2XX transceivers.
 */
typedef struct {
    at86rf2xx_t dev;        /**< Device descriptor */
    ieee802154_dev_t *hal;  /**< Pointer to the Radio HAL descriptor */
    event_queue_t *evq;     /**< Pointer to the event queue */
    event_t ev;             /**< ISR offload event */
} at86rf2xx_bhp_ev_t;

/**
 * @brief   Set the short address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void at86rf2xx_set_addr_short(at86rf2xx_t *dev, const network_uint16_t *addr);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void at86rf2xx_set_addr_long(at86rf2xx_t *dev, const eui64_t *addr);

/**
 * @brief   Get the PHY mode of the given device
 *
 * @param[in,out] dev       device to read from
 * @return                  the currently set phy mode
 */
uint8_t at86rf2xx_get_phy_mode(at86rf2xx_t *dev);

/**
 * @brief   Get the current O-QPSK rate mode of the PHY
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set rate mode
 */
uint8_t at86rf2xx_get_rate(at86rf2xx_t *dev);

/**
 * @brief   Set the current O-QPSK rate mode of the PHY
 *          rate modes > 0 are proprietary.
 *
 *          rate 0:   250 kbit/s (IEEE mode)
 *          rate 1:   500 kbit/s
 *          rate 2:  1000 kbit/s (compatible with AT86RF215)
 *          rate 3:  2000 kbit/s
 *
 * @param[in] dev           device to write to
 * @param[in] rate          the selected data rate mode (0-3)
 *
 * @return                  0 on success, otherwise error value
 */
int at86rf2xx_set_rate(at86rf2xx_t *dev, uint8_t rate);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] pan           PAN ID to set
 */
void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan);

/**
 * @brief   Get the configured receiver sensitivity of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured receiver sensitivity in dBm
 */
int8_t at86rf2xx_get_rxsensitivity(const at86rf2xx_t *dev);

/**
 * @brief   Set the receiver sensitivity of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] rxsens        rx sensitivity in dBm
 */
void at86rf2xx_set_rxsensitivity(const at86rf2xx_t *dev, int8_t rxsens);

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

/**
 * @brief   Enable the smart receive technology (SRT)
 *
 * The SRT reduces the power consumption during RX listening periods.
 *
 * @param[in]  dev          device to use
 *
 */
void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev);

/**
 * @brief   Disable the smart receive technology (SRT)
 *
 * @param[in]  dev          device to use
 *
 */
void at86rf2xx_disable_smart_idle(at86rf2xx_t *dev);

/**
 * @brief Disable AT86RF2XX clock output.
 *
 * @note This function only works for periph versions of AT86RF2XX.
 *
 * @param[in,out]  dev      pointer to device descriptor.
 */
void at86rf2xx_disable_clock_output(at86rf2xx_t *dev);

/**
 * @brief   Offload pending IRQ.
 *
 * Should be called in thread context from a thread with higher priority than
 * the thread that controls the device.
 *
 * @param[in]  hal          device to offload IRQs.
 *
 */
void at86rf2xx_irq_handler(ieee802154_dev_t *hal);

/**
 * @brief Init an AT86RF2XX device
 *
 * @param[in,out] dev   pointer to device descriptor
 * @param[in] params    parameters for device initialization. Can be NULL for periph variant.
 * @param[in,out] hal   pointer to the Radio HAL descriptor
 * @param[in] cb        ISR callback. Can be NULL for periph variant.
 * @param[in] ctx       Context of the ISR callback. Can be NULL for periph variant.
 *
 * @return error code
 * @retval 0 on success
 * @retval negative errno on failure
 */
int at86rf2xx_init(at86rf2xx_t *dev, const at86rf2xx_params_t *params, ieee802154_dev_t *hal, void (*cb)(void*), void *ctx);

/**
 * @brief Init an AT86RF2XX device with an event based Bottom Half Processor.
 *
 * @param[in,out] bhp   pointer to the event BHP descriptor
 * @param[in] params    parameters for device initialization
 * @param[in,out] hal   pointer to the Radio HAL descriptor
 * @param[in] evq       pointer to the event queue
 *
 * @return error code
 * @retval 0 on success
 * @retval negative errno on failure
 */
int at86rf2xx_init_event(at86rf2xx_bhp_ev_t *bhp, const at86rf2xx_params_t *params, ieee802154_dev_t *hal, event_queue_t *evq);

#ifdef __cplusplus
}
#endif

/** @} */
