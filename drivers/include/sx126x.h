/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sx126x SX1261/2/8 and LLCC68 LoRa radio driver
 * @ingroup     drivers_netdev
 * @brief       Driver for the SX1261/2/8 and LLCC68 LoRa radio device
 *
 * @{
 *
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SX126X_H
#define SX126X_H

#include <assert.h>

#include "sx126x_driver.h"

#include "net/netdev.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * * @note Forward declaration of the SX126x device descriptor
 */
typedef struct sx126x sx126x_t;

/**
 * @brief RF switch states
 */
typedef enum {
    SX126X_RF_MODE_RX,
    SX126X_RF_MODE_TX_LPA,
    SX126X_RF_MODE_TX_HPA,
} sx126x_rf_mode_t;

/**
 * @brief   Whether there's only one variant of this driver at compile time or
 * not.
 */
#define SX126X_SINGLE ((            \
                          IS_USED(MODULE_SX1261) \
                        + IS_USED(MODULE_SX1262) \
                        + IS_USED(MODULE_SX1268) \
                        + IS_USED(MODULE_LLCC68) \
                        + IS_USED(MODULE_SX126X_STM32WL) \
                        ) == 1)

/**
 * @brief   Used to identify if its a generic SPI module
 */
#if (IS_USED(MODULE_SX1261) || IS_USED(MODULE_SX1262) || \
     IS_USED(MODULE_SX1268) || IS_USED(MODULE_LLCC68))
#define SX126X_SPI    1
#endif

/**
 * @brief   Variant of the SX126x driver.
 */
typedef enum {
    SX126X_TYPE_SX1261,
    SX126X_TYPE_SX1262,
    SX126X_TYPE_SX1268,
    SX126X_TYPE_LLCC68,
    SX126X_TYPE_STM32WL,
} sx126x_type_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;                          /**< SPI device */
    gpio_t nss_pin;                     /**< SPI NSS pin */
    gpio_t reset_pin;                   /**< Reset pin */
    gpio_t busy_pin;                    /**< Busy pin */
    gpio_t dio1_pin;                    /**< Dio1 pin */
    sx126x_reg_mod_t regulator;         /**< Power regulator mode */
    sx126x_type_t type;                 /**< Variant of sx126x */
#if IS_USED(MODULE_SX126X_RF_SWITCH)
    /**
     * @ brief  Interface to set RF switch parameters
     */
    void(*set_rf_mode)(sx126x_t *dev, sx126x_rf_mode_t rf_mode);
#endif
} sx126x_params_t;

/**
 * @brief   Device descriptor for the driver
 */
struct sx126x {
    netdev_t netdev;                        /**< Netdev parent struct */
    sx126x_params_t *params;                /**< Initialization parameters */
    sx126x_pkt_params_lora_t pkt_params;    /**< Lora packet parameters */
    sx126x_mod_params_lora_t mod_params;    /**< Lora modulation parameters */
    uint32_t channel;                       /**< Current channel frequency (in Hz) */
    uint8_t rx_timeout;                     /**< Rx Timeout in terms of symbols */
    bool radio_sleep;                       /**< Radio sleep status */
};

/**
 * @brief   Setup the radio device
 *
 * @param[in] dev                       Device descriptor
 * @param[in] params                    Parameters for device initialization
 * @param[in] index                     Index of @p params in a global parameter struct array.
 *                                      If initialized manually, pass a unique identifier instead.
 */
void sx126x_setup(sx126x_t *dev, const sx126x_params_t *params, uint8_t index);

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev                    Device descriptor of the driver
 *
 * @return                  0 on success
 */
int sx126x_init(sx126x_t *dev);

/**
 * @brief   Converts symbol value to time in milliseconds.
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] symbols                  Symbols
 *
 * @return Time for symbol(s) in milliseconds
 */
static inline int sx126x_symbol_to_msec(sx126x_t *dev, uint16_t symbols)
{
    assert(dev && (dev->mod_params.bw <= SX126X_LORA_BW_500) && \
           (dev->mod_params.bw >= SX126X_LORA_BW_125));

    /* Refer section 6.1.4 LoRa Time-on-Air in SX1268 datasheet */
    return (symbols * (1 << (dev->mod_params.sf + 7 - dev->mod_params.bw)) / 1000);
}

/**
 * @brief   Gets the channel RF frequency.
 *
 * @param[in]  dev                     Device descriptor of the driver
 *
 * @return The channel frequency
 */
uint32_t sx126x_get_channel(const sx126x_t *dev);

/**
 * @brief   Sets the channel RF frequency.
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] freq                     Channel RF frequency
 */
void sx126x_set_channel(sx126x_t *dev, uint32_t freq);

/**
 * @brief   Gets the LoRa bandwidth
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the bandwidth
 */
uint8_t sx126x_get_bandwidth(const sx126x_t *dev);

/**
 * @brief   Sets the LoRa bandwidth
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] bandwidth                The new bandwidth
 */
void sx126x_set_bandwidth(sx126x_t *dev, uint8_t bandwidth);

/**
 * @brief   Gets the LoRa spreading factor
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the spreading factor
 */
uint8_t sx126x_get_spreading_factor(const sx126x_t *dev);

/**
 * @brief   Sets the LoRa spreading factor
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] sf                       The spreading factor
 */
void sx126x_set_spreading_factor(sx126x_t *dev, uint8_t sf);

/**
 * @brief   Gets the LoRa coding rate
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the current LoRa coding rate
 */
uint8_t sx126x_get_coding_rate(const sx126x_t *dev);

/**
 * @brief   Sets the LoRa coding rate
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] cr                       The LoRa coding rate
 */
void sx126x_set_coding_rate(sx126x_t *dev, uint8_t cr);

/**
 * @brief   Gets the payload length
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the payload length
 */
uint8_t sx126x_get_lora_payload_length(const sx126x_t *dev);

/**
 * @brief   Sets the payload length
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] len                      The payload len
 */
void sx126x_set_lora_payload_length(sx126x_t *dev, uint8_t len);

/**
 * @brief   Checks if CRC verification mode is enabled
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa single mode
 */
bool sx126x_get_lora_crc(const sx126x_t *dev);

/**
 * @brief   Enable/Disable CRC verification mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] crc                      The CRC check mode
 */
void sx126x_set_lora_crc(sx126x_t *dev, bool crc);

/**
 * @brief   Gets the LoRa implicit header mode
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa implicit mode
 */
bool sx126x_get_lora_implicit_header(const sx126x_t *dev);

/**
 * @brief   Sets LoRa implicit header mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] mode                     The header mode
 */
void sx126x_set_lora_implicit_header(sx126x_t *dev, bool mode);

/**
 * @brief   Gets the LoRa preamble length
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the preamble length
 */
uint16_t sx126x_get_lora_preamble_length(const sx126x_t *dev);

/**
 * @brief   Sets the LoRa preamble length
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] preamble                 The LoRa preamble length
 */
void sx126x_set_lora_preamble_length(sx126x_t *dev, uint16_t preamble);

/**
 * @brief   Checks if the LoRa inverted IQ mode is enabled/disabled
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa IQ inverted mode
 */
bool sx126x_get_lora_iq_invert(const sx126x_t *dev);

/**
 * @brief   Enable/disable the LoRa IQ inverted mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] iq_invert                The LoRa IQ inverted mode
 */
void sx126x_set_lora_iq_invert(sx126x_t *dev, bool iq_invert);

#ifdef __cplusplus
}
#endif

#endif /* SX126X_H */
/** @} */
