/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lora LoRa modulation
 * @ingroup     net
 * @brief       LoRa modulation header definitions
 * @{
 *
 * @file
 * @brief       LoRa modulation header definitions
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef NET_LORA_H
#define NET_LORA_H

#include <stdint.h>
#include <stdlib.h>

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_lora_conf  LoRa modulation compile configurations
 * @ingroup  config
 * @{
 */
/** @brief Frequency resolution in Hz */
#ifndef LORA_FREQUENCY_RESOLUTION_DEFAULT
#define LORA_FREQUENCY_RESOLUTION_DEFAULT      (61.03515625)
#endif

/** @brief Preamble length, same for Tx and Rx */
#ifndef CONFIG_LORA_PREAMBLE_LENGTH_DEFAULT
#define CONFIG_LORA_PREAMBLE_LENGTH_DEFAULT         (8U)
#endif

/** @brief Symbols timeout (s) */
#ifndef CONFIG_LORA_SYMBOL_TIMEOUT_DEFAULT
#define CONFIG_LORA_SYMBOL_TIMEOUT_DEFAULT          (10U)
#endif

#if IS_ACTIVE(CONFIG_LORA_BW_DEFAULT_125)
#define CONFIG_LORA_BW_DEFAULT                      (LORA_BW_125_KHZ)
#elif IS_ACTIVE(CONFIG_LORA_BW_DEFAULT_250)
#define CONFIG_LORA_BW_DEFAULT                      (LORA_BW_250_KHZ)
#elif IS_ACTIVE(CONFIG_LORA_BW_DEFAULT_500)
#define CONFIG_LORA_BW_DEFAULT                      (LORA_BW_500_KHZ)
#endif

/** @brief Set default bandwidth to 125kHz */
#ifndef CONFIG_LORA_BW_DEFAULT
#define CONFIG_LORA_BW_DEFAULT                      (LORA_BW_125_KHZ)
#endif

#if IS_ACTIVE(CONFIG_LORA_SF_DEFAULT_SF6)
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF6)
#elif IS_ACTIVE(CONFIG_LORA_SF_DEFAULT_SF7)
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF7)
#elif IS_ACTIVE(CONFIG_LORA_SF_DEFAULT_SF8)
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF8)
#elif IS_ACTIVE(CONFIG_LORA_SF_DEFAULT_SF9)
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF9)
#elif IS_ACTIVE(CONFIG_LORA_SF_DEFAULT_SF10)
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF10)
#elif IS_ACTIVE(CONFIG_LORA_SF_DEFAULT_SF11)
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF11)
#elif IS_ACTIVE(CONFIG_LORA_SF_DEFAULT_SF12)
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF12)
#endif

/** @brief Set default spreading factor to 12 */
#ifndef CONFIG_LORA_SF_DEFAULT
#define CONFIG_LORA_SF_DEFAULT                      (LORA_SF12)
#endif

#if IS_ACTIVE(CONFIG_LORA_CR_DEFAULT_CR_4_5)
#define CONFIG_LORA_CR_DEFAULT                      (LORA_CR_4_5)
#elif IS_ACTIVE(CONFIG_LORA_CR_DEFAULT_CR_4_6)
#define CONFIG_LORA_CR_DEFAULT                      (LORA_CR_4_6)
#elif IS_ACTIVE(CONFIG_LORA_CR_DEFAULT_CR_4_7)
#define CONFIG_LORA_CR_DEFAULT                      (LORA_CR_4_7)
#elif IS_ACTIVE(CONFIG_LORA_CR_DEFAULT_CR_4_8)
#define CONFIG_LORA_CR_DEFAULT                      (LORA_CR_4_8)
#endif

/** @brief Set default coding rate to 8 */
#ifndef CONFIG_LORA_CR_DEFAULT
#define CONFIG_LORA_CR_DEFAULT                      (LORA_CR_4_8)
#endif

/** @brief Set fixed payload length on */
#ifndef LORA_FIX_LENGTH_PAYLOAD_ON_DEFAULT
#define LORA_FIX_LENGTH_PAYLOAD_ON_DEFAULT     (false)
#endif

/** @brief Set this to true to enable inverted IQ mode */
#ifdef DOXYGEN
#define CONFIG_LORA_IQ_INVERTED_DEFAULT
#endif

/** @brief Set this to true to enable frequency hopping */
#ifdef DOXYGEN
#define CONFIG_LORA_FREQUENCY_HOPPING_DEFAULT
#endif

/** @brief Frequency hopping period */
#ifndef CONFIG_LORA_FREQUENCY_HOPPING_PERIOD_DEFAULT
#define CONFIG_LORA_FREQUENCY_HOPPING_PERIOD_DEFAULT (0U)
#endif

/** @brief Set this to true to enable fixed header length mode (implicit header) */
#ifdef DOXYGEN
#define CONFIG_LORA_FIXED_HEADER_LEN_MODE_DEFAULT
#endif

/** @brief Enable/disable payload CRC, optional
 *
 * @deprecated Use inverse @ref CONFIG_LORA_PAYLOAD_CRC_OFF_DEFAULT instead.
 * Will be removed after 2021.04 release
*/
#ifndef LORA_PAYLOAD_CRC_ON_DEFAULT
#if IS_ACTIVE(CONFIG_LORA_PAYLOAD_CRC_OFF_DEFAULT)
#define LORA_PAYLOAD_CRC_ON_DEFAULT                 (false)
#else
#define LORA_PAYLOAD_CRC_ON_DEFAULT                 (true)
#endif
#endif

/** @brief Set payload length, unused with implicit header */
#ifndef LORA_PAYLOAD_LENGTH_DEFAULT
#define LORA_PAYLOAD_LENGTH_DEFAULT            (0U)
#endif

/** @} */

/**
 * @name    LoRa syncword values for network types
 * @{
 */
#define LORA_SYNCWORD_PUBLIC           (0x34)  /**< Syncword used for public networks */
#define LORA_SYNCWORD_PRIVATE          (0x12)  /**< Syncword used for private networks */
/** @} */

/**
 * @name    LoRa modulation available values
 *
 */
/**
 * @brief   LoRa modulation bandwidth.
 */
enum {
    LORA_BW_125_KHZ = 0,               /**< 125 kHz bandwidth */
    LORA_BW_250_KHZ,                   /**< 250 kHz bandwidth */
    LORA_BW_500_KHZ                    /**< 500 kHz bandwidth */
};

/**
 * @brief   LoRa modulation spreading factor rate
 */
enum {
    LORA_SF6 = 6,                      /**< spreading factor 6 */
    LORA_SF7,                          /**< spreading factor 7 */
    LORA_SF8,                          /**< spreading factor 8 */
    LORA_SF9,                          /**< spreading factor 9 */
    LORA_SF10,                         /**< spreading factor 10 */
    LORA_SF11,                         /**< spreading factor 11 */
    LORA_SF12                          /**< spreading factor 12 */
};

/**
 * @brief   LoRa modulation error coding rate.
 */
enum {
    LORA_CR_4_5 = 1,                   /**< coding rate 4/5 */
    LORA_CR_4_6,                       /**< coding rate 4/6 */
    LORA_CR_4_7,                       /**< coding rate 4/7 */
    LORA_CR_4_8                        /**< coding rate 4/8 */
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_LORA_H */
/** @} */
