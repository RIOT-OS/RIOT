/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit_v4 SUIT draft v4
 * @ingroup     sys_suit
 * @brief       SUIT manifest handling
 *
 * @{
 *
 * @brief       Handler functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef SUIT_V4_SUIT_H
#define SUIT_V4_SUIT_H

#include <stddef.h>
#include <stdint.h>

#include "cose/sign.h"
#include "nanocbor/nanocbor.h"
#include "uuid.h"
#include "riotboot/flashwrite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Buffer size used for Cose
 */
#ifndef SUIT_COSE_BUF_SIZE
#define SUIT_COSE_BUF_SIZE              (512U)
#endif

/**
 * @brief   Maximum number of components used for SUIT v4
 */
#define SUIT_V4_COMPONENT_MAX           (1U)

/**
 * @brief Supported SUIT manifest version
 */
#define SUIT_MANIFEST_VERSION           (4)

/**
 * @brief Current SUIT serialization format version
 *
 * see https://tools.ietf.org/html/draft-moran-suit-manifest-04#section-8.2 for
 * details
 */
#define SUIT_VERSION                    (1)

/**
 * @brief SUIT error codes
 */
typedef enum {
    SUIT_OK                     = 0,    /**< Manifest parsed and validated */
    SUIT_ERR_INVALID_MANIFEST   = -1,   /**< Unexpected CBOR structure detected */
    SUIT_ERR_UNSUPPORTED        = -2,   /**< Unsupported SUIT feature detected */
    SUIT_ERR_NOT_SUPPORTED      = -3,   /**< Unsupported manifest features detected */
    SUIT_ERR_COND               = -4,   /**< Conditionals evaluate to false */
    SUIT_ERR_SEQUENCE_NUMBER    = -5,   /**< Sequence number less or equal to
                                             current sequence number */
    SUIT_ERR_SIGNATURE          = -6,   /**< Unable to verify signature */
} suit_v4_error_t;

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
} suit_v4_digest_t;

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
} suit_v4_digest_type_t;

/**
 * @brief SUIT component types
 *
 * Unofficial list from
 * [suit-manifest-generator](https://github.com/ARMmbed/suit-manifest-generator)
 */
enum {
    SUIT_COMPONENT_IDENTIFIER   = 1,    /**< Identifier component */
    SUIT_COMPONENT_SIZE         = 2,    /**< Size component */
    SUIT_COMPONENT_DIGEST       = 3,    /**< Digest component */
};

/**
 * @brief SUIT v4 component struct
 */
typedef struct {
    uint32_t size;                      /**< Size */
    nanocbor_value_t identifier;        /**< Identifier*/
    nanocbor_value_t url;               /**< Url */
    nanocbor_value_t digest;            /**< Digest */
} suit_v4_component_t;

/**
 * @brief SUIT manifest struct
 */
typedef struct {
    cose_sign_dec_t verify;         /**< COSE signature validation struct */
    const uint8_t *buf;             /**< ptr to the buffer of the manifest */
    size_t len;                     /**< length of the manifest */
    uint32_t validated;             /**< bitfield of validated policies */
    uint32_t state;                 /**< bitfield holding state information */

    /** List of components in the manifest */
    suit_v4_component_t components[SUIT_V4_COMPONENT_MAX];
    unsigned components_len;        /**< Current number of components */
    int32_t component_current;      /**< Current component index */
    riotboot_flashwrite_t *writer;  /**< Pointer to the riotboot flash writer */
    /** Manifest validation buffer */
    uint8_t validation_buf[SUIT_COSE_BUF_SIZE];
    cose_key_t *key;                /**< Ptr to the public key for validation */
    char *urlbuf;                   /**< Buffer containing the manifest url */
    size_t urlbuf_len;              /**< Length of the manifest url */
} suit_v4_manifest_t;

/**
 * @brief Bit flags used to determine if SUIT manifest contains components
 */
#define SUIT_MANIFEST_HAVE_COMPONENTS   (0x1)
/**
 * @brief Bit flags used to determine if SUIT manifest contains an image
 */
#define SUIT_MANIFEST_HAVE_IMAGE        (0x2)

/**
 * @brief Parse a manifest
 *
 * @note The buffer is still required after parsing, please don't reuse the
 * buffer while the @p manifest is used
 *
 * @param[in]   manifest    manifest context to store information in
 * @param[in]   buf         buffer to parse the manifest from
 * @param[in]   len         length of the manifest data in the buffer
 *
 * @return                  SUIT_OK on parseable manifest
 * @return                  negative @ref suit_v4_error_t code on error
 */
int suit_v4_parse(suit_v4_manifest_t *manifest, const uint8_t *buf, size_t len);

/**
 * @brief Check a manifest policy
 *
 * @param[in]   manifest    manifest context to check the policy for
 *
 * @return                  0 on valid manifest policy
 * @return                  -1 on invalid manifest policy
 */
int suit_v4_policy_check(suit_v4_manifest_t *manifest);

/**
 * @brief Initialize a cbor iterator for SUIT cbor map container parsing
 *
 * @param[in]   map     the cbor container
 * @param[in]   it      the cbor iterator
 *
 * @return              SUIT_OK when initialization is successful
 * @return              SUIT_ERR_INVALID_MANIFEST if the manifest is not a cbor container
 */
int suit_cbor_map_iterate_init(nanocbor_value_t *map, nanocbor_value_t *it);

/**
 * @brief Iterate over a cbor map container
 *
 * @param[in]   it      cbor container iterator
 * @param[out]  key     the returned key
 * @param[out]  value   the returned value
 *
 * @return              0 when the iterator is already at the end of the container
 * @return              the number of returned (key, value) pair, e.g. 1
 */
int suit_cbor_map_iterate(nanocbor_value_t *it, nanocbor_value_t *key, nanocbor_value_t *value);

/**
 * @brief Get cbor value as int32_t
 *
 * @param[in]   it      cbor container iterator
 * @param[out]  out     address of the returned integer
 *
 * @return              SUIT_OK on success
 * @return              SUIT_ERR_INVALID_MANIFEST if value doesn't fit in an int
 */
int suit_cbor_get_int32(nanocbor_value_t *it, int32_t *out);

/**
 * @brief Get cbor value as unsigned
 *
 * @param[in]   it      cbor container iterator
 * @param[out]  out     address of the returned unsigned
 *
 * @return              SUIT_OK on success
 * @return              SUIT_ERR_INVALID_MANIFEST if value doesn't fit or cannot
 *                      be converted to unsigned
 */
int suit_cbor_get_uint(nanocbor_value_t *it, unsigned *out);

/**
 * @brief Get cbor value as unsigned long
 *
 * @param[in]   it      cbor container iterator
 * @param[out]  out     address of the returned unsigned long
 *
 * @return              SUIT_OK on success
 * @return              SUIT_ERR_INVALID_MANIFEST if value doesn't fit or cannot
 *                      be converted to unsigned long
 */
int suit_cbor_get_uint32(nanocbor_value_t *it, uint32_t *out);

/**
 * @brief Get cbor value as string
 *
 * @param[in]   it      cbor container iterator
 * @param[out]  buf     address of the string buffer
 * @param[out]  len     address of the len of the string
 *
 * @return              SUIT_OK on success
 * @return              SUIT_ERR_INVALID_MANIFEST if value is not a valid string
 */
int suit_cbor_get_string(nanocbor_value_t *it, const uint8_t **buf, size_t *len);

/**
 * @brief Parser a cbor subsequence
 *
 * @param[in]   bseq        subsequence value
 * @param[out]  it          cbor iterator
 *
 * @return                  0 on success
 * @return                  -1 if bseq is not a cbor string
 * @return                  CborError code on other cbor parser errors
 */
int suit_cbor_subparse(nanocbor_value_t *bseq, nanocbor_value_t *it);

/**
 * @brief Helper function for writing bytes on flash a specified offset
 *
 * @param[in]   arg     ptr to flash writer
 * @param[in]   offset  offset to write to on flash
 * @param[in]   buf     bytes to write
 * @param[in]   len     length of bytes to write
 * @param[in]   more    whether more data is coming
 *
 * @return              0 on success
 * @return              <0 on error
 */
int suit_flashwrite_helper(void *arg, size_t offset, uint8_t *buf, size_t len,
                           int more);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_V4_SUIT_H */
/** @} */
