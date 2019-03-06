/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_suit SUIT manifest parsers
 * @ingroup     sys
 * @brief       SUIT manifest parser for CBOR based manifests
 *
 * This is a simple suit manifest parser for RIOT. The high level assumption is
 * that the raw manifest data is stored in a buffered location where raw values
 * or strings can be left during the lifetime of the suit_manifest_t struct.
 * This assumption is valid in the case where a (network based) transfer
 * mechanism is used to transfer the manifest to the node and an intermediate
 * buffer is necessary to validate the manifest.
 *
 * The parser is based on draft version 1 of the specification, restricted to
 * handling CBOR based manifests.
 *
 * @see https://tools.ietf.org/html/draft-moran-suit-manifest-01
 *
 * @{
 *
 * @brief       Decoding functions for a CBOR encoded SUIT manifest
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef SUIT_CBOR_H
#define SUIT_CBOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "cbor.h"
#include "uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUIT_CBOR_MANIFEST_MIN_LENGTH            9  /**< Minimum length of a
                                                         manifest array */
#define SUIT_CBOR_MANIFEST_PAYLOADINFO_LENGTH    7  /**< Length of the payload
                                                         info array   */

/**
 * @name Manifest array member indices
 * @{
 */
#define SUIT_CBOR_MANIFEST_IDX_MANIFESTVERSION   0  /**< Manifest version     */
#define SUIT_CBOR_MANIFEST_IDX_TEXT              1  /**< Text section         */
#define SUIT_CBOR_MANIFEST_IDX_NONCE             2  /**< Nonce                */
#define SUIT_CBOR_MANIFEST_IDX_SEQ_NO            3  /**< Sequence number      */
#define SUIT_CBOR_MANIFEST_IDX_CONDITIONS        4  /**< Conditionals         */
#define SUIT_CBOR_MANIFEST_IDX_DIRECTIVES        5  /**< Directives           */
#define SUIT_CBOR_MANIFEST_IDX_ALIASES           6  /**< Aliases              */
#define SUIT_CBOR_MANIFEST_IDX_DEPENDENCIES      7  /**< Dependencies         */
#define SUIT_CBOR_MANIFEST_IDX_EXTENSIONS        8  /**< Extensions           */
#define SUIT_CBOR_MANIFEST_IDX_PAYLOADINFO       9  /**< Optional PayloadInfo */
/** @} */

/**
 * @name Manifest PayloadInfo member indices
 * @{
 */
#define SUIT_CBOR_PAYLOADINFO_IDX_FORMAT         0  /**< Payload format       */
#define SUIT_CBOR_PAYLOADINFO_IDX_SIZE           1  /**< Firmware size        */
#define SUIT_CBOR_PAYLOADINFO_IDX_STORID         2  /**< Storage ID           */
#define SUIT_CBOR_PAYLOADINFO_IDX_URIS           3  /**< URI list             */
#define SUIT_CBOR_PAYLOADINFO_IDX_DIGESTALGO     4  /**< Digest algorithm     */
#define SUIT_CBOR_PAYLOADINFO_IDX_DIGESTS        5  /**< Digests              */
#define SUIT_CBOR_PAYLOADINFO_IDX_PAYLOAD        6  /**< Optional payload     */
/** @} */

/**
 * @brief TinyCBOR validation mode to use
 */
#define SUIT_TINYCBOR_VALIDATION_MODE       CborValidateStrictMode

/**
 * @brief SUIT conditionals
 */
enum {
    SUIT_COND_VENDOR_ID     = 1,    /**< Vendor ID match conditional */
    SUIT_COND_CLASS_ID      = 2,    /**< Class ID match conditional */
    SUIT_COND_DEV_ID        = 3,    /**< Device ID match conditional */
    SUIT_COND_BEST_BEFORE   = 4,    /**< Best before conditional */
};

/**
 * @brief SUIT payload digest algorithms
 *
 * Unofficial list from
 * [suit-manifest-generator](https://github.com/ARMmbed/suit-manifest-generator)
 */
typedef enum {
    SUIT_DIGEST_NONE    = 0,    /**< No digest algo supplied */
    SUIT_DIGEST_SHA256  = 1,    /**< SHA256 */
    SUIT_DIGEST_SHA384  = 2,    /**< SHA384 */
    SUIT_DIGEST_SHA512  = 3,    /**< SHA512 */
} suit_cbor_digest_t;

/**
 * @brief SUIT payload digest types
 *
 * Unofficial list from
 * [suit-manifest-generator](https://github.com/ARMmbed/suit-manifest-generator)
 */
typedef enum {
    SUIT_DIGEST_TYPE_RAW        = 1,    /**< Raw payload digest */
    SUIT_DIGEST_TYPE_INSTALLED  = 2,    /**< Installed firmware digest */
    SUIT_DIGEST_TYPE_CIPHERTEXT = 3,    /**< Ciphertext digest */
    SUIT_DIGEST_TYPE_PREIMAGE   = 4     /**< Pre-image digest */
} suit_cbor_digest_type_t;

/**
 * @brief SUIT manifest struct
 */
typedef struct {
    const uint8_t *buf; /**< ptr to the buffer of the manifest */
    size_t len;         /**< length of the manifest */
} suit_cbor_manifest_t;

/**
 * @brief Parse a buffer containing a cbor encoded manifest into a
 * suit_cbor_manifest_t struct
 *
 * @param   manifest    manifest to fill
 * @param   buf         Buffer to read from
 * @param   len         Size of the buffer
 *
 * return               @ref SUIT_OK on success
 * return               negative on parsing error
 */
int suit_cbor_parse(suit_cbor_manifest_t *manifest, const uint8_t *buf,
                    size_t len);

/**
 * @brief Copy the url from the manifest into the supplied buffer
 *
 * @param       manifest    manifest to retrieve the url from
 * @param[out]  buf         buffer to write into
 * @param       len         size of the buffer
 *
 * @return                  Length of the url
 * @return                  negative on error
 */
ssize_t suit_cbor_get_url(const suit_cbor_manifest_t *manifest, char *buf, size_t len);

/**
 * @brief Copy the version from the manifest into the supplied uint32_t
 *
 * @param       manifest    manifest to retrieve the version from
 * @param[out]  version     version number to set
 *
 * @return                  @ref SUIT_OK on success
 * @return                  negative on error
 */
int suit_cbor_get_version(const suit_cbor_manifest_t *manifest, uint32_t *version);

/**
 * @brief Retrieve the sequence number of the manifest
 *
 * @note The sequence number is called timestamp in the first version of the
 *       spec, but called sequence number in later versions.
 *
 * @param       manifest    manifest to retrieve the sequence number from
 * @param[out]  seq_no      sequence number
 *
 * @return                  @ref SUIT_OK on success
 * @return                  negative on error
 */
int suit_cbor_get_seq_no(const suit_cbor_manifest_t *manifest, uint32_t *seq_no);

/**
 * @brief Retrieve the condition type at position \p idx from the manifest
 *
 * @param[in]       manifest    manifest to retrieve the condition from
 * @param[in]       idx         index of the condition in the array
 * @param[out]      condition   condition type value
 *
 * @return                      Length of the parameter
 * @return                      Negative on error
 */
ssize_t suit_cbor_get_condition_type(const suit_cbor_manifest_t *manifest,
                                     unsigned idx, signed *condition);

/**
 * @brief Retrieve the condition parameter at position \p idx from the manifest
 *
 * @param[in]       manifest    manifest to retrieve the condition from
 * @param[in]       idx         index of the condition in the array
 * @param[out]      buf         buffer to store the parameter in
 * @param[inout]    len         length of the buffer and length of the returned
 *                              parameter
 *
 * @return                      Length of the parameter
 * @return                      Negative on error
 */
int suit_cbor_get_condition_parameter(const suit_cbor_manifest_t *manifest,
                                      unsigned idx, uint8_t *buf, size_t *len);

/**
 * @brief Retrieve the size from the payloadinfo in the manifest
 *
 * @param       manifest    manifest to retrieve the size from
 * @param[out]  size        Size of the firmware
 *
 * @return                  @ref SUIT_OK on success
 * @return                  negative on error
 */
int suit_cbor_payload_get_size(const suit_cbor_manifest_t *manifest, uint32_t *size);

/**
 * @brief Retrieve the digest algorithm from the payloadinfo in the manifest
 *
 * @param       manifest    manifest to retrieve the algo from
 * @param[out]  algo        algorithm used
 *
 * @return                  @ref SUIT_OK on success
 * @return                  negative on error
 */
int suit_cbor_payload_get_digestalgo(const suit_cbor_manifest_t *manifest,
                                     suit_cbor_digest_t *algo);

/**
 * @brief Retrieve the storage id from the payloadinfo in the manifest
 *
 * @param           manifest    manifest to retrieve the storage id from
 * @param[inout]    buf         buffer to write the storage id in
 * @param[inout]    len         size of the buffer and length of the storage id
 *
 * @return                      @ref SUIT_OK on success
 * @return                      negative on error
 */
int suit_cbor_payload_get_storid(const suit_cbor_manifest_t *manifest, uint8_t *buf, size_t *len);

/**
 * @brief Retrieve the digest from the payloadinfo in the manifest
 *
 * @param           manifest    manifest to retrieve the digest from
 * @param           digest      type of digest to retrieve
 * @param[inout]    buf         buffer to write the digest in
 * @param[inout]    len         size of the buffer and length of the digest
 *
 * @return                      @ref SUIT_OK on success
 * @return                      negative on error
 */
int suit_cbor_payload_get_digest(const suit_cbor_manifest_t *manifest,
                                 suit_cbor_digest_type_t digest, uint8_t *buf, size_t *len);


int suit_cbor_validator_verify_conditions(suit_cbor_manifest_t *manifest, uint64_t curtime);

/**
 * @brief Initialize the UUID's for conditionals
 */
void suit_cbor_uuid_init(void);

const uuid_t *suit_cbor_get_uuid_vendor(void);
const uuid_t *suit_cbor_get_uuid_class(void);
const uuid_t *suit_cbor_get_uuid_device(void);

/**
 * Check if a manifest is a valid update compared to the old manifest
 *
 * @param   old     Current manifest to compare
 * @param   new     New manifest to compare
 *
 * @return          True when the new manifest is a newer manifest
 */
bool suit_cbor_manifest_isnewer(const suit_cbor_manifest_t *old,
                                const suit_cbor_manifest_t *cur);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_CBOR_H */
/** @} */
