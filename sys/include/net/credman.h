/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_credman (D)TLS Credential Manager
 * @ingroup     net net_dtls
 * @brief       Credentials management module for (D)TLS
 *
 * @{
 *
 * @file
 * @brief       (D)TLS credentials management module definitions
 *
 * @note        This module DOES NOT copy the credentials into the system. It
 *              just holds the pointers to the credentials given by the user.
 *              The user must make sure that these pointers are valid during the
 *              lifetime of the application.
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */


#include <unistd.h>
#include <stdint.h>
#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_credman_conf (D)TLS Credential Manager compile configurations
 * @ingroup config
 * @{
 */
/**
 * @brief Maximum number of credentials in credential pool
 */
#ifndef CONFIG_CREDMAN_MAX_CREDENTIALS
#define CONFIG_CREDMAN_MAX_CREDENTIALS  (2)
#endif

/**
 * @brief Maximum number of ASN.1 objects when decoding keys.
 */
#ifndef CONFIG_CREDMAN_MAX_ASN1_OBJ
#define CONFIG_CREDMAN_MAX_ASN1_OBJ  (8)
#endif
/** @} */

/**
 * @brief Buffer of the credential
 */
typedef struct {
    const void *s;          /**< Pointer to the buffer */
    size_t len;             /**< Length of credman_buffer_t::s */
} credman_buffer_t;

/**
 * @brief PSK parameters
 */
typedef struct {
    credman_buffer_t key;   /**< Key buffer */
    credman_buffer_t id;    /**< ID buffer */
    credman_buffer_t hint;  /**< Hint buffer */
} psk_params_t;

/**
 * @brief ECDSA public keys
 */
typedef struct {
    const void *x;          /**< X part of the public key */
    const void *y;          /**< Y part of the public key */
} ecdsa_public_key_t;

/**
 * @brief ECDSA parameters
 */
typedef struct {
    const void *private_key;            /**< Pointer to the private key */
    ecdsa_public_key_t public_key;      /**< Public key */
    ecdsa_public_key_t *client_keys;    /**< Array of clients public keys */
    size_t client_keys_size;            /**< Size of ecdsa_params_t::clients_keys */
} ecdsa_params_t;

/**
 * @brief Tag of the credential.
 */
typedef uint16_t credman_tag_t;

/**
 * @brief Used to signal empty/no tag
 */
#define CREDMAN_TAG_EMPTY   (0)

/**
 * @brief Credential types
 */
typedef enum {
    /**
     * @brief Empty type
     *
     * Used to detect uninitialized @ref credman_credential_t internally.
     */
    CREDMAN_TYPE_EMPTY  = 0,
    CREDMAN_TYPE_PSK    = 1,    /**< PSK credential type */
    CREDMAN_TYPE_ECDSA  = 2,    /**< ECDSA credential type */
} credman_type_t;

/**
 * @brief Credential information
 */
typedef struct {
    credman_type_t type;        /**< Type of the credential */
    credman_tag_t tag;          /**< Tag of the credential */
    union {
        psk_params_t psk;       /**< PSK credential parameters */
        ecdsa_params_t ecdsa;   /**< ECDSA credential parameters */
    } params;                   /**< Credential parameters */
} credman_credential_t;

/**
 * @brief Return values
 */
enum {
    CREDMAN_OK              = 0,    /**< No error */
    CREDMAN_EXIST           = -1,   /**< Credential already exist in system pool */
    CREDMAN_NO_SPACE        = -2,   /**< No space in system pool for new credential */
    CREDMAN_NOT_FOUND       = -3,   /**< Credential not found in the system pool */
    CREDMAN_INVALID         = -4,   /**< Invalid input parameter(s) */
    CREDMAN_TYPE_UNKNOWN    = -5,   /**< Unknown credential type */
    CREDMAN_ERROR           = -6,   /**< Other errors */
};

/**
 * @brief Adds a credential to the credential pool
 *
 * @param[in] credential    Credential to add.
 *
 * @return CREDMAN_OK on success
 * @return CREDMAN_EXIST if credential of @p tag and @p type already exist
 * @return CREDMAN_NO_SPACE if credential pool is full
 * @return CREDMAN_TYPE_UNKNOWN if @p credential has unknown
 *         credman_credential_t::type
 * @return CREDMAN_INVALID if @p credential has
 * @return CREDMAN_INVALID credman_credential_t::tag with the value of
 *         CREDMAN_TAG_EMPTY
 * @return CREDMAN_INVALID credman_credential_t::type with the value of
 *         CREDMAN_TYPE_EMPTY
 * @return CREDMAN_INVALID credman_credential_t::params with invalid credential
 *         parameters i.e. the key points to NULL or has a length of 0
 * @return CREDMAN_ERROR on other errors
 */
int credman_add(const credman_credential_t *credential);

/**
 * @brief Gets a credential from credential pool
 *
 * @param[out] credential   Found credential
 * @param[in] tag           Tag of credential to get
 * @param[in] type          Type of credential to get
 *
 * @return CREDMAN_OK on success
 * @return CREDMAN_NOT_FOUND if no credential with @p tag and @p type found
 * @return CREDMAN_ERROR on other errors
 */
int credman_get(credman_credential_t *credential, credman_tag_t tag,
                credman_type_t type);

/**
 * @brief Delete a credential from the credential pool. Does nothing if
 *        credential with credman_credential_t::tag @p tag and
 *        credman_credential_t::type @p type is not found.
 *
 * @param[in] tag           Tag of the credential
 * @param[in] type          Type of the credential
 */
void credman_delete(credman_tag_t tag, credman_type_t type);

/**
 * @brief Gets the number of credentials currently in the credential pool
 *
 * Maximum number of allowed credentials is defined by CONFIG_CREDMAN_MAX_CREDENTIALS
 *
 * @return number of credentials currently in the credential pool
 */
int credman_get_used_count(void);

#if IS_USED(MODULE_CREDMAN_LOAD) || DOXYGEN
/**
 * @brief Load a public key from a buffer, as a `SubjectPublicKeyInfo` sequence, according to
 *        RFC5280. The key should be encoded in DER format.
 *
 * @pre `buf != NULL && out != NULL`.
 *
 * @note To use this functionality include the module `credman_load`. Credman only supports ECDSA
 *       for now, so [RFC5480](https://tools.ietf.org/html/rfc5480) applies.
 *
 * @experimental This API is considered experimental and will probably change without notice!
 *
 * @see https://tools.ietf.org/html/rfc5280#section-4.1
 *
 * @param[in]  buf          Buffer holding the encoded public key
 * @param[in]  buf_len      Length of @p buf
 * @param[out] out          ECDSA public key to populate
 *
 * @retval CREDMAN_OK on success
 * @retval CREDMAN_INVALID if the key is not valid
 */
int credman_load_public_key(const void *buf, size_t buf_len, ecdsa_public_key_t *out);

/**
 * @brief Load a private key from a buffer, as a `OneAsymmetricKey` sequence, according to RFC5958.
 *        This is compatible with the previous version PKCS#8 (defined in RFC5208). If the optional
 *        respective public key is present, it will be loaded as well. The key should be encoded in
 *        DER format.
 *
 * @pre `buf != NULL && cred != NULL`
 *
 * @note To use this functionality include the module `credman_load`. Credman only supports ECDSA
 *       for now.
 *
 * @experimental This API is considered experimental and will probably change without notice!
 *
 * @see https://tools.ietf.org/html/rfc5958#section-2
 *
 * @param[in]  buf          Buffer holding the encoded private key
 * @param[in]  buf_len      Length of @p buf
 * @param[out] cred         Credential to populate
 *
 * @retval CREDMAN_OK on success
 * @retval CREDMAN_INVALID if the key is not valid
 */
int credman_load_private_key(const void *buf, size_t buf_len, credman_credential_t *cred);

/**
 * @brief Load an ECC private key from a buffer, as an `ECPrivateKey` sequence, according to RFC5915.
 *        If the optional respective public key is present, it will be loaded as well. The key
 *        should be encoded in DER format.
 *
 * @pre `buf != NULL && cred != NULL`
 *
 * @note To use this functionality include the module `credman_load`.
 *
 * @experimental This API is considered experimental and will probably change without notice!
 *
 * @see https://tools.ietf.org/html/rfc5915#section-3
 *
 * @param[in]  buf          Buffer holding the encoded private key
 * @param[in]  buf_len      Length of @p buf
 * @param[out] cred         Credential to populate
 *
 * @retval CREDMAN_OK on success
 * @retval CREDMAN_INVALID if the key is not valid
 */
int credman_load_private_ecc_key(const void *buf, size_t buf_len, credman_credential_t *cred);
#endif /* MODULE_CREDMAN_LOAD || DOXYGEN */

#ifdef TEST_SUITES
/**
 * @brief Empties the credential pool
 */
void credman_reset(void);
#endif /*TEST_SUITES */

#ifdef __cplusplus
}
#endif

/** @} */
