/*
 * Copyright (C) 2025 TU Dresden
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
 * @file
 * @brief       AEAD type definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#include "psa_crypto_operation_encoder.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "crypto/psa/riot_aeads.h"
#include "psa/algorithm.h"
#include "psa/cipher/types.h"

/* 
 * These states along with some information about the direction
 * (encryption/decryption) and whether the lengths have been set
 * get stored in the AEAD operation object in a single uint8.
 * The information is encoded as follows (counting from LSB):
 * - bit 1:     direction (dec=0, enc=1 - in accordance to
 *                         psa_encrypt_or_decrypt_t)
 * - bit 2:     were lengths set?
 * - bits 3-8:  state of the operation
 */
#define PSA_AEAD_OP_STATE_INACTIVE    (0 << 2)
#define PSA_AEAD_OP_STATE_LENGTHS_REQ (1 << 2)
#define PSA_AEAD_OP_STATE_NONCE_REQ   (2 << 2)
#define PSA_AEAD_OP_STATE_AAD_IN      (3 << 2)
#define PSA_AEAD_OP_STATE_MSG_IN      (4 << 2)
#define PSA_AEAD_OP_STATE_ERROR       (5 << 2)
/* Use these masks with bitwise and (&) on the operation state to
 * extract information from it, without having to use magic numbers. */
#define PSA_AEAD_OP_STATE_MASK        (0xFC)
#define PSA_AEAD_OP_DIRECTION_MASK    (1)
#define PSA_AEAD_OP_LENGHTS_MASK      (2)
#define PSA_AEAD_OP_CONFIG_MASK       (3)

/**
 * @brief   Structure storing an AEAD operation context
 */
struct psa_aead_operation_s {
    uint8_t state;                   /**< Encoded state of the operation */
    size_t ad_length;                /**< Length of additional data */
    size_t processed_ad_length;      /**< Length of already processed additional data */
    size_t message_length;           /**< Length of message data */
    size_t processed_message_length; /**< Length of already processed message data */
    psa_aead_op_t op;                /**< Encoded operation */
    /** Union containing AEAD cipher contexts for the executing backend */
    union aead_context {
#if IS_USED(MODULE_PSA_AEAD_CHACHA20_POLY1305) || defined(DOXYGEN)
        psa_aead_chacha20_poly1305_ctx_t chacha20poly1305; /**< ChaCha20 context*/
#endif
    } backend_ctx;
};

/* These are all temporarily defined as some numeric type to prevent errors at compile time.*/
/**
 * @brief   The type of the state object for multi-part AEAD operations.
 *
 * @details Before calling any function on an AEAD operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_aead_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static
 *            or global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_aead_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_AEAD_OPERATION_INIT, for example:
 *            @code
 *            @ref psa_aead_operation_t operation = @ref PSA_AEAD_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_aead_operation_init() to the object,
 *            for example:
 *            @code
 *            @ref psa_aead_operation_t operation;
 *            operation = @ref psa_aead_operation_init();
 *            @endcode
 *
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_aead_operation_s psa_aead_operation_t;

/**
 * @brief   This macro returns a suitable initializer for an AEAD operation object of type
 *          @ref psa_aead_operation_t.
 */
#define PSA_AEAD_OPERATION_INIT \
    {                           \
        0                       \
    }

/**
 * @brief   Return an initial value for an AEAD operation object.
 *
 * @return  psa_aead_operation_t
 */
static inline psa_aead_operation_t psa_aead_operation_init(void)
{
    const psa_aead_operation_t v = PSA_AEAD_OPERATION_INIT;

    return v;
}

#ifdef __cplusplus
}
#endif

/** @} */
