/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_mbedtls_entropy_sources Entropy source API to Mbed TLS
 * @ingroup     pkg_mbedtls
 *
 * @{
 * @file
 * @brief       Functions to register and poll with the Mbed TLS entropy module.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Structure containing entropy function and its strength
 */
typedef struct {
    mbedtls_entropy_f_source_ptr func; /**< Pointer to entropy callback func. */
    int strong;                        /**< Strength of the entropy function
                                         *  (strong=1 for high entropy sources,
                                         *   strong=0 for weak entropy sources)*/
} entropy_source_mbedtls_riot_t;

/**
 * @brief   Add all available entropy sources to poll.
 *
 * This function would typically not be called by a user. It is called during
 * initialization by @ref entropy_mbedtls_riot_init.
 *
 * @param[out] ctx    Pointer to mbedtls context.
 *
 * @return             0 if successful
 * @return             MBEDTLS_ERR_ENTROPY_MAX_SOURCES on failure
 */
int riot_add_entropy_src_avail(mbedtls_entropy_context *ctx);

/**
 * @brief   Wrapper around RIOTs HWRNG API.
 *
 * Required to comply with mbed TLS entropy callback function signature.
 *
 * @param[in]  data    Not used.
 * @param[in]  output  Pointer to the output buffer to fill.
 * @param[in]  len     Length of requested entropy in bytes.
 * @param[out] olen    Equals len.
 *
 * @return             0 always
 */
int riot_hwrng_poll(void *data, unsigned char *output, size_t len,
                     size_t *olen);

/**
 * @brief   Wrapper around RIOTs ADC entropy API.
 *
 * Required to comply with mbed TLS entropy callback function signature.
 *
 * @param[in]  data    Not used.
 * @param[in]  output  Pointer to the output buffer to fill.
 * @param[in]  len     Length of requested entropy in bytes.
 * @param[out] olen    Equals len.
 *
 * @return             0 if no critical failures occurred,
 *                     MBEDTLS_ERR_ENTROPY_SOURCE_FAILED otherwise.
 */
int riot_adc_poll(void *data, unsigned char *output, size_t len,
                  size_t *olen);

#if !defined(MODULE_MBEDTLS_ENTROPY_SOURCE_HWRNG) && \
    !defined(MODULE_ENTROPY_SOURCE_ADC_NOISE)
#error "You must enable at least one entropy source. Currently supported are \
        HWRNG and ADC_NOISE"
#endif

#ifdef __cplusplus
}
#endif
/** @} */
