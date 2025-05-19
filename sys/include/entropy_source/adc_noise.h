/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup   sys_entropy_source_adc ADC Noise Entropy Source
 * @ingroup    sys_entropy_source
 * @brief      Entropy Source based on LSB of ADC samples
 *
 *
 * This module provides entropy from ADC samples. Thereby, only the LSB is considered for
 * generation of output values. The available resolution as well as internal sampling rates,
 * specific driver settings and device inaccuracies may lead to different behavior between
 * different platforms. The configured ADC pin might be unconnected and floating,
 * it can be exposed as an I/O pin or internally connected to a thermal noise source, an
 * avalanche diode circuit or the receive path of an antenna. The possibilities are manifold,
 * thus, configuration and deployment properties of this module need to be thoroughly
 * validated.
 *
 * @note It is worth noting that ADC pins are typically vulnerable when physically
 *       accessible. Developers should consider additional tamper detection concepts
 *       and enclosures.
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stddef.h>

#include "periph/adc.h"

/**
 * @ingroup    sys_entropy_source_config
 * @{
 */
/**
 * @brief   ADC resolution default configuration.
 */
#if IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ADC_RES_6BIT)
#define CONFIG_ENTROPY_SOURCE_ADC_RES          ADC_RES_6BIT
#elif IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ADC_RES_8BIT)
#define CONFIG_ENTROPY_SOURCE_ADC_RES          ADC_RES_8BIT
#elif IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ADC_RES_10BIT)
#define CONFIG_ENTROPY_SOURCE_ADC_RES          ADC_RES_10BIT
#elif IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ADC_RES_12BIT)
#define CONFIG_ENTROPY_SOURCE_ADC_RES          ADC_RES_12BIT
#elif IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ADC_RES_14BIT)
#define CONFIG_ENTROPY_SOURCE_ADC_RES          ADC_RES_14BIT
#elif IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ADC_RES_16BIT)
#define CONFIG_ENTROPY_SOURCE_ADC_RES          ADC_RES_16BIT
#endif
#ifndef CONFIG_ENTROPY_SOURCE_ADC_RES
#define CONFIG_ENTROPY_SOURCE_ADC_RES   ADC_RES_10BIT
#endif

/**
 * @brief   ADC line default configuration.
 *
 * The ADC line maps to an I/O pin. This number acts as index to an array
 * of predefined ADC devices that contain the pin definition. Typically,
 * the array is defined by a board in a periph_conf.h file. Please note that
 * a board is not required to specify a minimum number of lines.
 */
#ifdef CONFIG_ENTROPY_SOURCE_ADC_LINE_NUM
#define CONFIG_ENTROPY_SOURCE_ADC_LINE         ADC_LINE(CONFIG_ENTROPY_SOURCE_ADC_LINE_NUM)
#else
#define CONFIG_ENTROPY_SOURCE_ADC_LINE         ADC_LINE(0)
#endif

/**
 * @brief   ADC estimated entropy per sample [2^16 * bit/sample].
 *
 * The entropy value needs to be estimated and evaluated thoroughly beforehand
 * deployment! To avoid float, the
 * actual entropy value per one byte sample needs to be manually multiplied by
 * 2^16 before before configuring it (e.g., to
 * an entropy value of 1 bit/sample, a value of 1 * 65536 needs to be set) .
 * We default to zero which is an invalid configuration to enforce a
 * thoughtful investigation on the actual entropy properties. See
 * @ref sys_entropy_source for further information about entropy source
 * validation.
 */
#if !defined(CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ADC_NOISE) || defined(DOXYGEN)
#ifndef CONFIG_ENTROPY_SOURCE_ADC_HMIN
#define CONFIG_ENTROPY_SOURCE_ADC_HMIN         (0)  /**< H_min=0 bit/sample * 2^16
                                                     *   is invalid and needs to
                                                     *   set manually!
                                                     */
#endif /* !CONFIG_ENTROPY_SOURCE_ADC_HMIN */

#ifndef CONFIG_ENTROPY_SOURCE_ADC_HEALTH_TEST
#define CONFIG_ENTROPY_SOURCE_ADC_HEALTH_TEST  0  /**< Disable ADC health test
                                                   *   by default.
                                                   */
#endif
#ifndef CONFIG_ENTROPY_SOURCE_ADC_COND
#define CONFIG_ENTROPY_SOURCE_ADC_COND         0   /**< Disable ADC conditioning
                                                    *   test by default.
                                                    */
#endif
#endif /* !CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ADC_NOISE || DOXYGEN */

/* Throw warning if H_min has not been re-configured */
#if !CONFIG_ENTROPY_SOURCE_ADC_HMIN
#warning The min. provided entropy must be set before using this module
#endif
/** @} */

/**
 * @brief   Initialize ADC and test structures, if tests are enabled.
 *
 * @return          ENTROPY_SOURCE_OK on success
 * @return          ENTROPY_SOURCE_ERR_INIT on ADC initialization failure
 */
int entropy_source_adc_init(void);

/**
 * @brief   Generates bytes from ADC noise
 *
 * @param[out] buf   pointer to write noisy bytes to
 * @param[in]  len   number of bytes to generate
 *
 * @return          ENTROPY_SOURCE_OK on success
 * @return          negative @ref entropy_source_error_t code on error
 */
int entropy_source_adc_get(uint8_t *buf, size_t len);

/**
 * @brief           Static entropy per sample value for this source
 *                  [bit/sample * 2^16]
 *
 * @return          entropy per sample
 */
static inline uint32_t entropy_source_adc_entropy_per_sample(void)
{
    return CONFIG_ENTROPY_SOURCE_ADC_HMIN;
}

#ifdef __cplusplus
}
#endif

/** @} */
