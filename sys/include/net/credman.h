/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef NET_CREDMAN_H
#define NET_CREDMAN_H

#include <unistd.h>
#include <stdint.h>

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

#ifdef TEST_SUITES
/**
 * @brief Empties the credential pool
 */
void credman_reset(void);
#endif /*TEST_SUITES */

#ifdef __cplusplus
}
#endif

#endif /* NET_CREDMAN_H */
/** @} */
