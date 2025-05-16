/*
 * Copyright (C) 2024 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        error.h
 * @brief       Error definitions for the PSA Crypto API
 *
 * @details     This header file is also compatible with the PSA Certified Status code API.
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Status code type used for all PSA Certified APIs.
 *
 * @details This is either @ref PSA_SUCCESS, which is zero, indicating success; or a small
 *          negative value indicating that an error occurred. Errors are encoded as one of
 *          the @c PSA_ERROR_xxx values defined here.
 */
#ifndef PSA_SUCCESS
typedef int32_t psa_status_t;
#endif

/**
 * @brief Status code to indicate general success.
 */
#define PSA_SUCCESS ((psa_status_t)0)

/**
 * @brief Status code that indicates a programmer error in the client.
 */
#define PSA_ERROR_PROGRAMMER_ERROR ((psa_status_t)-129)

/**
 * @brief Status code that indicates that the caller is not permitted to connect to a Service.
 */
#define PSA_ERROR_CONNECTION_REFUSED ((psa_status_t)-130)

/**
 * @brief Status code that indicates that the caller cannot connect to a service.
 */
#define PSA_ERROR_CONNECTION_BUSY ((psa_status_t)-131)

/**
 * @brief Status code that indicates an error that does not correspond to any defined
 *        failure cause.
 */
#define PSA_ERROR_GENERIC_ERROR ((psa_status_t)-132)

/**
 * @brief Status code that indicates that the requested action is denied by a policy.
 */
#define PSA_ERROR_NOT_PERMITTED ((psa_status_t)-133)

/**
 * @brief Status code that indicates that the requested operation or a parameter is not supported.
 */
#define PSA_ERROR_NOT_SUPPORTED ((psa_status_t)-134)

/**
 * @brief Status code that indicates that the parameters passed to the function are invalid.
 */
#define PSA_ERROR_INVALID_ARGUMENT ((psa_status_t)-135)

/**
 * @brief Status code that indicates that a handle parameter is not valid.
 *
 * @details Usually means that a key identifier does not refer to an existing key.
 */
#define PSA_ERROR_INVALID_HANDLE ((psa_status_t)-136)

/**
 * @brief Status code that indicates that the requested action cannot be performed in the
 *        current state.
 *
 * @details Multi-part operations return this error when one of the functions is called out
 *          of sequence. We also return this error if the caller has not initialized the library
 *          by a call to @ref psa_crypto_init().
 */
#define PSA_ERROR_BAD_STATE ((psa_status_t)-137)

/**
 * @brief Status code that indicates that an output buffer parameter is too small.
 *
 * @details Applications can call the @c PSA_xxx_SIZE macros listed in the function description to
 *          determine a sufficient buffer size.
 */
#define PSA_ERROR_BUFFER_TOO_SMALL ((psa_status_t)-138)

/**
 * @brief Status code that indicates that an identifier or index is already in use.
 */
#define PSA_ERROR_ALREADY_EXISTS ((psa_status_t)-139)

/**
 * @brief Status code that indicates that an identified resource does not exist.
 */
#define PSA_ERROR_DOES_NOT_EXIST ((psa_status_t)-140)

/**
 * @brief Status code that indicates that there is not enough runtime memory.
 */
#define PSA_ERROR_INSUFFICIENT_MEMORY ((psa_status_t)-141)

/**
 * @brief Status code that indicates that there is not enough persistent storage.
 */
#define PSA_ERROR_INSUFFICIENT_STORAGE ((psa_status_t)-142)

/**
 * @brief Status code that indicates that a data source has insufficient capacity left.
 */
#define PSA_ERROR_INSUFFICIENT_DATA ((psa_status_t)-143)

/**
 * @brief Status code that indicates an error within the service.
 */
#define PSA_ERROR_SERVICE_FAILURE ((psa_status_t)-144)

/**
 * @brief Status code that indicates a communication failure between the function and another
 *        service or component.
 */
#define PSA_ERROR_COMMUNICATION_FAILURE ((psa_status_t)-145)

/**
 * @brief Status code that indicates a storage failure that may have led to data loss.
 *
 * @details When a storage failure occurs, it is no longer possible to ensure the global
 *          integrity of the keystore. Access to other data might fail even if the data
 *          is still readable but its integrity cannot be guaranteed.
 */
#define PSA_ERROR_STORAGE_FAILURE ((psa_status_t)-146)

/**
 * @brief Status code that indicates that a hardware failure was detected.
 */
#define PSA_ERROR_HARDWARE_FAILURE ((psa_status_t)-147)

/**
 * @brief Status code that indicates that there is not enough entropy to generate random data
 *        needed for the requested action.
 */
#define PSA_ERROR_INSUFFICIENT_ENTROPY ((psa_status_t)-148)

/**
 * @brief Status code that indicates that a signature, MAC or hash is incorrect.
 */
#define PSA_ERROR_INVALID_SIGNATURE ((psa_status_t)-149)

/**
 * @brief Status code that indicates that the decrypted padding is incorrect.
 */
#define PSA_ERROR_INVALID_PADDING ((psa_status_t)-150)

/**
 * @brief Status code that indicates that internal data has been tampered with.
 *
 * @details This error code is intended as a last resort when a security breach is detected
 *          and it is unsure whether the keystore data is still protected. Only return this
 *          error code to report an alarm from a tampering detector, to indicate that the
 *          confidentiality of stored data can no longer be guaranteed, or to indicate that
 *          the integrity of previously returned data is now considered compromised.
 */
#define PSA_ERROR_CORRUPTION_DETECTED ((psa_status_t)-151)

/**
 * @brief Status code that indicates that stored data has been corrupted.
 *
 * @details When a storage failure occurs, it is no longer possible to ensure the global integrity
 *          of the keystore. Depending on the global integrity guarantees, access to other data
 *          might fail even if the data is still readable but its integrity cannot be guaranteed.
 */
#define PSA_ERROR_DATA_CORRUPT ((psa_status_t)-152)

/**
 * @brief Status code that indicates that data read from storage is not valid for the
 *        implementation.
 */
#define PSA_ERROR_DATA_INVALID ((psa_status_t)-153)

/**
 * @brief Status code that indicates that the requested operation is interruptible, and still
 *        has work to do.
 *
 * @details This status code does not mean that the operation has failed or that it has succeeded.
 *          The operation must be repeated until it completes with either success or failure.
 */
#define PSA_OPERATION_INCOMPLETE ((psa_status_t)-248)

#ifdef __cplusplus
}
#endif

/** @} */
