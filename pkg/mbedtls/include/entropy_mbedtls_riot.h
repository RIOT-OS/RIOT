/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_mbedtls_entropy Access API to Mbed TLS entropy module
 * @ingroup     pkg_mbedtls
 *
 * @{
 * @file
 * @brief       Convenience functions to retrieve entropy from Mbed TLS. Direct
 *              use of the Mbed TLS API is also possible without this API.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#ifndef ENTROPY_MBEDTLS_RIOT_H
#define ENTROPY_MBEDTLS_RIOT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Initialize the Mbed TLS entropy module.
 *
 * A context structure is allocated and managed internally.
 * Connect all available entropy sources to the poll.
 *
 * @retval    0 if successful
 * @retval    MBEDTLS_ERR_ENTROPY_MAX_SOURCES on failure
 */
int entropy_mbedtls_riot_init(void);

/**
 * @brief   Retrieve entropy values after initialization.
 *
 * @pre Module is initialized already (i.e. @ref entropy_mbedtls_riot_init has been called).
 *
 * @param[out] output   Pointer to the output buffer to fill with entropy values.
 * @param[in] len      Length of requested entropy in bytes.
 *
 * @retval             0 if successful
 * @retval             MBEDTLS_ERR_ENTROPY_SOURCE_FAILED if source failed
 */
int entropy_mbedtls_riot_retrieve(unsigned char *output, size_t len);

/**
 * @brief   Uninitialize the Mbed TLS entropy module.
 *
 * Frees the internally allocated context. Mbed TLS zeroizes the memory.
 */
void entropy_mbedtls_riot_uninit(void);

/**
 * @brief   Get entropy values.
 *
 * Convenience function. A context structure is allocated and managed
 * internally. Requested entropy values are gathered and the context is
 * uninitialized afterwards.
 *
 * @param[out] output   Pointer to the output buffer to fill with entropy values.
 * @param[in] len      Length of requested entropy in bytes.
 *
 * @retval             0 if successful
 * @retval             MBEDTLS_ERR_ENTROPY_MAX_SOURCES on failure
 * @retval             MBEDTLS_ERR_ENTROPY_SOURCE_FAILED if source failed
 */
int entropy_mbedtls_riot_get(unsigned char *output, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY_MBEDTLS_RIOT_H */
/** @} */
