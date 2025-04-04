/*
 * Copyright (C) 2022 Inria
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SX1280_H
#define SX1280_H

/**
 * @defgroup    drivers_sx1280 LoRa radio driver
 * @ingroup     drivers_netdev
 * @brief       Driver for the sx1280 LoRa radio device
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 */

#include <assert.h>

#include "sx1280_driver/src/sx1280_hal.h"
#include "sx1280_driver/src/sx1280.h"
#include "smtc_ral/src/ral.h"
#include "smtc_ral/src/ral_defs.h"

#include "net/netdev.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SX1280 device default configuration
 * @{
 */
#define SX1280_CHANNEL_DEFAULT           (2403000000UL)             /**< Default channel frequency, 2403MHz */
#define SX1280_RADIO_TX_POWER            (SX1280_PWR_MAX)           /**< Radio power in dBm */
/** @} */

/**
 * * @note Forward declaration of the sx1280 device descriptor
 */
typedef struct sx1280 sx1280_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;                          /**< SPI device */
    spi_mode_t spi_mode;                /**< SPI mode */
    spi_clk_t spi_clk;                  /**< SPI clk */
    gpio_t nss_pin;                     /**< SPI NSS pin */
    gpio_t reset_pin;                   /**< Reset pin */
    gpio_t busy_pin;                    /**< Busy pin */
    gpio_t dio0_pin;                    /**< Dio0 pin */
    gpio_t dio1_pin;                    /**< Dio1 pin */
    sx1280_reg_mod_t regulator;         /**< Power regulator mode */
} sx1280_params_t;

/**
 * @brief   Device descriptor for the driver
 */
struct sx1280 {
    netdev_t netdev;                        /**< Netdev parent struct */
    ral_t ral;                              /**< Radio abstraction */
    ral_params_lora_t ral_params_lora;      /**< LoRa modulation parameters */
    sx1280_params_t *params;                /**< Initialization parameters */
    sx1280_hal_operating_mode_t mode;       /**< Operating Mode */
};

/**
 * @brief   Setup the radio device
 *
 * @param[in] dev                       Device descriptor
 * @param[in] params                    Parameters for device initialization
 * @param[in] index                     Index of @p params in a global parameter struct array.
 *                                      If initialized manually, pass a unique identifier instead.
 */
void sx1280_setup(sx1280_t *dev, const sx1280_params_t *params, uint8_t index);

/**
 * @brief   Initialize the given device
 *
 * @param[in,out] dev                   Device descriptor of the driver
 *
 * @return 0 on success
 */
int sx1280_init(sx1280_t *dev);

/**
 * @brief   Gets the channel RF frequency.
 *
 * @param[in]  dev                     Device descriptor of the driver
 *
 * @return The channel frequency
 */
uint32_t sx1280_get_channel(const sx1280_t *dev);

/**
 * @brief   Gets a random number
 *
 * @param[in]  dev                     Device descriptor of the driver
 *
 * @return a random number
 */
uint32_t sx1280_random(const sx1280_t *dev);
/**
 * @brief   Sets the channel RF frequency.
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] freq                     Channel RF frequency
 */
void sx1280_set_channel(sx1280_t *dev, uint32_t freq);

/**
 * @brief   Gets the LoRa bandwidth
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the bandwidth
 */
uint32_t sx1280_get_bandwidth(const sx1280_t *dev);

/**
 * @brief   Sets the LoRa bandwidth
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] bandwidth                The new bandwidth
 */
void sx1280_set_bandwidth(sx1280_t *dev, uint16_t bandwidth);

/**
 * @brief   Gets the LoRa spreading factor
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the spreading factor
 */
uint8_t sx1280_get_spreading_factor(const sx1280_t *dev);

/**
 * @brief   Sets the LoRa spreading factor
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] sf                       The spreading factor
 */
void sx1280_set_spreading_factor(sx1280_t *dev, uint8_t sf);

/**
 * @brief   Gets the LoRa coding rate
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the current LoRa coding rate
 */
uint8_t sx1280_get_coding_rate(const sx1280_t *dev);

/**
 * @brief   Sets the LoRa coding rate
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] cr                       The LoRa coding rate
 */
void sx1280_set_coding_rate(sx1280_t *dev, uint8_t cr);

/**
 * @brief   Gets the payload length
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the payload length
 */
uint8_t sx1280_get_lora_payload_length(const sx1280_t *dev);

/**
 * @brief   Sets the payload length
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] len                      The payload len
 */
void sx1280_set_lora_payload_length(sx1280_t *dev, uint8_t len);

/**
 * @brief   Checks if CRC verification mode is enabled
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa single mode
 */
bool sx1280_get_lora_crc(const sx1280_t *dev);

/**
 * @brief   Enable/Disable CRC verification mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] crc                      The CRC check mode
 */
void sx1280_set_lora_crc(sx1280_t *dev, bool crc);

/**
 * @brief   Gets the LoRa implicit header mode
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa implicit mode
 */
bool sx1280_get_lora_implicit_header(const sx1280_t *dev);

/**
 * @brief   Sets LoRa implicit header mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] mode                     The header mode
 */
void sx1280_set_lora_implicit_header(sx1280_t *dev, bool mode);

/**
 * @brief   Gets the LoRa preamble length
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the preamble length
 */
uint16_t sx1280_get_lora_preamble_length(const sx1280_t *dev);

/**
 * @brief   Sets the LoRa preamble length
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] preamble                 The LoRa preamble length
 */
void sx1280_set_lora_preamble_length(sx1280_t *dev, uint16_t preamble);

/**
 * @brief   Checks if the LoRa inverted IQ mode is enabled/disabled
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa IQ inverted mode
 */
bool sx1280_get_lora_iq_invert(const sx1280_t *dev);

/**
 * @brief   Enable/disable the LoRa IQ inverted mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] iq_invert                The LoRa IQ inverted mode
 */
void sx1280_set_lora_iq_invert(sx1280_t *dev, bool iq_invert);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SX1280_H */
