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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LoRa modulation default values
 * @{
 */
#ifndef LORA_FREQUENCY_RESOLUTION_DEFAULT
/** @brief Frequency resolution in Hz */
#define LORA_FREQUENCY_RESOLUTION_DEFAULT      (61.03515625)
#endif
#ifndef LORA_PREAMBLE_LENGTH_DEFAULT
/**< @brief Preamble length, same for Tx and Rx */
#define LORA_PREAMBLE_LENGTH_DEFAULT           (8U)
#endif
#ifndef LORA_SYMBOL_TIMEOUT_DEFAULT
/** @brief Symbols timeout (s) */
#define LORA_SYMBOL_TIMEOUT_DEFAULT            (10U)
#endif
#ifndef LORA_BW_DEFAULT
/** @brief Set default bandwidth to 125kHz */
#define LORA_BW_DEFAULT                        (LORA_BW_125_KHZ)
#endif
#ifndef LORA_SF_DEFAULT
/** @brief Set default spreading factor to 12 */
#define LORA_SF_DEFAULT                        (LORA_SF12)
#endif
#ifndef LORA_CR_DEFAULT
/** @brief Set default coding rate to 8 */
#define LORA_CR_DEFAULT                        (LORA_CR_4_8)
#endif
#ifndef LORA_FIX_LENGTH_PAYLOAD_ON_DEFAULT
/** @brief Set fixed payload length on */
#define LORA_FIX_LENGTH_PAYLOAD_ON_DEFAULT     (false)
#endif
#ifndef LORA_IQ_INVERTED_DEFAULT
/** @brief Set inverted IQ on */
#define LORA_IQ_INVERTED_DEFAULT               (false)
#endif
#ifndef LORA_FREQUENCY_HOPPING_DEFAULT
/** @brief Frequency hopping on */
#define LORA_FREQUENCY_HOPPING_DEFAULT         (false)
#endif
#ifndef LORA_FREQUENCY_HOPPING_PERIOD_DEFAULT
/** @brief Frequency hopping period */
#define LORA_FREQUENCY_HOPPING_PERIOD_DEFAULT  (0U)
#endif
#ifndef LORA_FIXED_HEADER_LEN_MODE_DEFAULT
/** @brief Set fixed header length mode (implicit header) */
#define LORA_FIXED_HEADER_LEN_MODE_DEFAULT     (false)
#endif
#ifndef LORA_PAYLOAD_CRC_ON_DEFAULT
/** @brief Enable payload CRC, optional */
#define LORA_PAYLOAD_CRC_ON_DEFAULT            (true)
#endif
#ifndef LORA_PAYLOAD_LENGTH_DEFAULT
/** @brief Set payload length, unused with implicit header */
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
