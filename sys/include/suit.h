/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit SUIT secure firmware OTA upgrade infrastructure
 * @ingroup     sys
 * @brief       SUIT manifest handling
 *
 * @experimental
 *
 * @note The current implementation of this specification is based on the
 *       IETF-SUIT-v3 draft. The module is still experimental and will change to
 *       match future draft specifications
 *
 * @see https://tools.ietf.org/html/draft-ietf-suit-manifest-03
 *
 * @{
 *
 * @brief       Handler functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef SUIT_H
#define SUIT_H

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
#define SUIT_COSE_BUF_SIZE                  (180U)
#endif

/**
 * @brief   Maximum number of components supported in a SUIT manifest
 */
#define SUIT_COMPONENT_MAX                  (1U)

/**
 * @brief Current SUIT serialization format version
 *
 * see https://tools.ietf.org/html/draft-ietf-suit-manifest-03#section-7 for
 * details
 */
#define SUIT_VERSION                        (1)

/**
 * @brief COSE signature OK
 */
#define SUIT_STATE_COSE_AUTHENTICATED       (1 << 1)

/**
 * @brief COSE payload matches SUIT manifest digest
 */
#define SUIT_STATE_FULLY_AUTHENTICATED      (1 << 2)

/**
 * @brief SUIT error codes
 */
typedef enum {
    SUIT_OK                   =  0, /**< Manifest parsed and validated */
    SUIT_ERR_INVALID_MANIFEST = -1, /**< Unexpected CBOR structure detected */
    SUIT_ERR_UNSUPPORTED      = -2, /**< Unsupported SUIT feature detected */
    SUIT_ERR_NOT_SUPPORTED    = -3, /**< Unsupported features detected */
    SUIT_ERR_COND             = -4, /**< Conditionals evaluate to false */
    SUIT_ERR_SEQUENCE_NUMBER  = -5, /**< Sequence number less or equal to
                                         current sequence number */
    SUIT_ERR_SIGNATURE        = -6, /**< Unable to verify signature */
    SUIT_ERR_DIGEST_MISMATCH  = -7, /**< Digest mismatch with COSE and SUIT */
} suit_error_t;

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
} suit_digest_t;

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
} suit_digest_type_t;

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
 * @brief SUIT component struct
 */
typedef struct {
    uint32_t size;                      /**< Size */
    nanocbor_value_t identifier;        /**< Identifier */
    nanocbor_value_t url;               /**< Url */
    nanocbor_value_t digest;            /**< Digest */
} suit_component_t;

/**
 * @brief SUIT manifest struct
 */
typedef struct {
    const uint8_t *buf;             /**< ptr to the buffer of the manifest */
    size_t len;                     /**< length of the manifest */
    const uint8_t *cose_payload;    /**< ptr to the payload of the COSE sign */
    size_t cose_payload_len;        /**< length of the COSE payload */
    uint32_t validated;             /**< bitfield of validated policies */
    uint32_t state;                 /**< bitfield holding state information */
    /** List of components in the manifest */
    suit_component_t components[SUIT_COMPONENT_MAX];
    unsigned components_len;        /**< Current number of components */
    uint32_t component_current;     /**< Current component index */
    riotboot_flashwrite_t *writer;  /**< Pointer to the riotboot flash writer */
    /** Manifest validation buffer */
    uint8_t validation_buf[SUIT_COSE_BUF_SIZE];
    char *urlbuf;                   /**< Buffer containing the manifest url */
    size_t urlbuf_len;              /**< Length of the manifest url */
} suit_manifest_t;

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
 * @return                  negative @ref suit_error_t code on error
 */
int suit_parse(suit_manifest_t *manifest, const uint8_t *buf, size_t len);

/**
 * @brief Check a manifest policy
 *
 * @param[in]   manifest    manifest context to check the policy for
 *
 * @return                  0 on valid manifest policy
 * @return                  -1 on invalid manifest policy
 */
int suit_policy_check(suit_manifest_t *manifest);

/**
 * @brief Helper function for writing bytes on flash a specified offset
 *
 * @param[in]   arg     ptr to the SUIT manifest
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

#endif /* SUIT_H */
/** @} */
