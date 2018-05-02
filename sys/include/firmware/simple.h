/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef FIRMWARE_SIMPLE_H
#define FIRMWARE_SIMPLE_H

#include "firmware.h"
#include "hashes/sha256.h"
#ifdef RIOT_VERSION
#include "firmware/flashwrite.h"
#endif

#include "tweetnacl.h"

/**
 *  @brief Provisional length for signed hash
 */
#define FIRMWARE_SIG_LEN            (crypto_sign_BYTES)

/**
 *  @brief Length of secret Ed25519 key
 */
#define FIRMWARE_SECKEY_LEN         (crypto_sign_SECRETKEYBYTES)

/**
 *  @brief Length of public Ed25519 key
 */
#define FIRMWARE_PUBKEY_LEN         (crypto_sign_PUBLICKEYBYTES)

#define FIRMWARE_METADATA_RIOTBOOT   (0x0100)

/**
 *  @brief Length of bytes to be signed
 */
#define FIRMWARE_SIGN_BYTES         (sizeof(firmware_simple_t) - FIRMWARE_SIG_LEN)

typedef struct {
    firmware_metadata_t metadata;       /**< generic bootloader specific firmware info */
    uint32_t metadata_type;             /**< Type of metadata, 16 bits type,
                                          16 bit version */
    uint32_t appid;                     /**< Application type ID */
    uint32_t size;                      /**< Size of firmware image */
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< SHA256 Hash of firmware image */
    uint8_t sig[FIRMWARE_SIG_LEN];      /**< Firmware Signature */
} firmware_simple_t;

/**
 * @brief   Possible firmware update states
 */
enum {
    FIRMWARE_UPDATE_IDLE,           /**< no firmware update in progress         */
    FIRMWARE_UPDATE_INITIALIZED,    /**< firmware update in progress, awaiting
                                         verification                           */
    FIRMWARE_UPDATE_VERIFIED,       /**< firmware update in progress & verified */
};

#ifdef RIOT_VERSION

/**
 * @brief Firmware update state struct
 */
typedef struct {
    firmware_flashwrite_t writer;       /**< flashwrite state struct */
    unsigned state;                         /**< state (see above enum)  */
    union {
        uint8_t metadata_buf[FIRMWARE_METADATA_SIZE];
        firmware_simple_t metadata;
    } m;
    sha256_context_t sha;                   /**< SHA256 digest state struct */
} firmware_simple_update_t;

int firmware_simple_init(firmware_simple_update_t *state);

int firmware_simple_putbytes(firmware_simple_update_t *state, const uint8_t *bytes, size_t len);

int firmware_simple_finish(firmware_simple_update_t *state);

#endif /* RIOT_VERSION */

/**
 * @brief  Print formatted FW image metadata to STDIO
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_simple_print(firmware_simple_t *riotboot);

/**
 * @brief  Sign metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] sk        NaCL secret signing key to use
 *
 */
int firmware_simple_sign(firmware_simple_t *riotboot, unsigned char *sk);

/**
 * @brief  Validate FW metadata signature
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] pk        NaCL public signing key to use
 *
 */
int firmware_simple_validate_signature(firmware_simple_t *riotboot, const unsigned char *pk);
#endif /* FIRMWARE_SIMPLE_H */
