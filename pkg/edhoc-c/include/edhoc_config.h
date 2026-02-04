/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_edhoc_c
 * @{
 *
 * @file
 * @brief       EDHOC-C configuration file
 *
 * @author      Timothy Claeys <timothy.claeys@inria.fr>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief EDHOC_CIPHER_SUITE_X_ENABLED
 *
 * Enables support for a specific EDHOC cipher suite
 *
 */
#define EDHOC_CIPHER_SUITE_0_ENABLED

/**
 * @brief EDHOC_AUTH_METHOD_X_ENABLED
 * @{
 *
 * Enables support for a specific EDHOC authentication method
 *
 */
#define EDHOC_AUTH_METHOD_0_ENABLED
#define EDHOC_AUTH_METHOD_1_ENABLED
#define EDHOC_AUTH_METHOD_2_ENABLED
#define EDHOC_AUTH_METHOD_3_ENABLED
/** @} */

/**
 * @brief EDHOC_ASYNC_API_ENABLED
 *
 * Exposes the individual EDHOC message functions for asynchronous usage.
 *
 */
#define EDHOC_ASYNC_API_ENABLED

/**
 * @brief EDHOC_DEBUG_ENABLE
 *
 * Enables some extra methods that allow for easier testing and debugging
 *
 */
#define EDHOC_DEBUG_ENABLED

/**
 * @brief EDHOC_AUTH_CERT_ENABLED
 * @{
 *
 * Enables CBOR certificates as the EDHOC local credential
 *
 */
#define EDHOC_AUTH_CERT_ENABLED
#if defined(EDHOC_AUTH_CERT_ENABLED)
#define EDHOC_AUTH_CBOR_CERT
#endif
/** @} */

/**
 * @brief EDHOC_AUTH_RPK_ENABLED
 *
 * Enables COSE raw public keys as the EDHOC local credential
 *
 */
#define EDHOC_AUTH_RPK_ENABLED

/**
 * @brief EDHOC_COSE_HEADER_SIZE
 *
 * Sets the maximum number of COSE header elements
 */
#define EDHOC_COSE_HEADER_SIZE              (5)

/**
 * @brief EDHOC_CREDENTIAL_MAX_SIZE
 *
 * Sets the maximum buffer size for credentials (raw keys or certificates)
 *
 */
#define EDHOC_CRED_SIZE                     (256)

/**
 * @brief EDHOC_CREDENTIAL_ID_MAX_SIZE
 *
 * Sets the maximum buffer size for credential identifiers
 *
 */
#define EDHOC_CRED_ID_SIZE                  (256)

/**
 * @brief EDHOC_ADD_DATA_MAX_SIZE
 *
 * Maximum number of additional data bytes to piggy-back on the EDHOC exchange
 *
 */
#define EDHOC_ADDITIONAL_DATA_SIZE          (64)

#ifdef __cplusplus
}
#endif
