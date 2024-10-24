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
 *       IETF-SUIT-v9 draft. The module is still experimental and will change to
 *       match future draft specifications
 *
 * @see https://tools.ietf.org/html/draft-ietf-suit-manifest-09
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
#ifndef CONFIG_SUIT_COMPONENT_MAX
#define CONFIG_SUIT_COMPONENT_MAX                  (1U)
#endif

/**
 * @brief Maximum name of component, includes separator
 */
#ifndef CONFIG_SUIT_COMPONENT_MAX_NAME_LEN
#define CONFIG_SUIT_COMPONENT_MAX_NAME_LEN          (32U)
#endif

/**
 * @brief Current SUIT serialization format version
 *
 * see https://tools.ietf.org/html/draft-ietf-suit-manifest-03#section-7 for
 * details
 */
#define SUIT_VERSION                        (1)

/**
 * @name SUIT manifest status flags
 *
 * These flags apply to the full manifest.
 * @{
 */
/**
 * @brief Bit flags used to determine if SUIT manifest contains components
 */
#define SUIT_STATE_HAVE_COMPONENTS          (1 << 0)

/**
 * @brief COSE signature OK
 */
#define SUIT_STATE_COSE_AUTHENTICATED       (1 << 1)

/**
 * @brief COSE payload matches SUIT manifest digest
 */
#define SUIT_STATE_FULLY_AUTHENTICATED      (1 << 2)
/** @} */

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
    SUIT_ERR_POLICY_FORBIDDEN = -8, /**< Denied because of policy mismatch */
    SUIT_ERR_NO_MEM           = -9, /**< Out of memory condition */
    SUIT_ERR_STORAGE          = -50, /**< Backend returned an error */
    SUIT_ERR_STORAGE_EXCEEDED = -51, /**< Backend out of space */
    SUIT_ERR_STORAGE_UNAVAILABLE = -52, /**< Backend location not available */
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
 * @name SUIT parameters
 * @{
 */
typedef enum {
    SUIT_PARAMETER_VENDOR_IDENTIFIER = 1,
    SUIT_PARAMETER_CLASS_IDENTIFIER  = 2,
    SUIT_PARAMETER_IMAGE_DIGEST      = 3,
    SUIT_PARAMETER_USE_BEFORE        = 4,
    SUIT_PARAMETER_COMPONENT_OFFSET  = 5,
    SUIT_PARAMETER_STRICT_ORDER      = 12,
    SUIT_PARAMETER_SOFT_FAILURE      = 13,
    SUIT_PARAMETER_IMAGE_SIZE        = 14,
    SUIT_PARAMETER_ENCRYPTION_INFO   = 18,
    SUIT_PARAMETER_COMPRESSION_INFO  = 19,
    SUIT_PARAMETER_UNPACK_INFO       = 20,
    SUIT_PARAMETER_URI               = 21,
    SUIT_PARAMETER_SOURCE_COMPONENT  = 22,
    SUIT_PARAMETER_RUN_ARGS          = 23,
    SUIT_PARAMETER_DEVICE_IDENTIFIER = 24,
    SUIT_PARAMETER_MINIMUM_BATTERY   = 26,
    SUIT_PARAMETER_UPDATE_PRIORITY   = 27,
    SUIT_PARAMETER_VERSION           = 28,
    SUIT_PARAMETER_WAIT_INFO         = 29,
    SUIT_PARAMETER_URI_LIST          = 30,
} suit_parameter_t;
/** @} */

/**
 * @brief SUIT parameter reference
 *
 * A 16-bit offset is enough to reference content inside the manifest itself.
 */
typedef struct {
    uint16_t offset;    /**< offset to the start of the content */
} suit_param_ref_t;

/**
 * @name SUIT component flags.
 *
 * These state flags apply to individual components inside a manifest.
 * @{
 */
#define SUIT_COMPONENT_STATE_FETCHED       (1 << 0) /**< Component is fetched */
#define SUIT_COMPONENT_STATE_FETCH_FAILED  (1 << 1) /**< Component fetched but failed */
#define SUIT_COMPONENT_STATE_VERIFIED      (1 << 2) /**< Component is verified */
#define SUIT_COMPONENT_STATE_INSTALLED     (1 << 3) /**< Component is installed, but has not been verified */
#define SUIT_COMPONENT_STATE_FINALIZED     (1 << 4) /**< Component successfully installed */
/** @} */

/**
 * @brief   Forward declaration for storage struct
 *
 * Breaks a dependency chain
 */
typedef struct suit_storage suit_storage_ref_t;

/**
 * @brief SUIT component struct as decoded from the manifest
 *
 * The parameters are references to CBOR-encoded information in the manifest.
 */
typedef struct {
    suit_storage_ref_t *storage_backend;        /**< Storage backend used */
    uint16_t state;                             /**< Component status flags */
    suit_param_ref_t identifier;                /**< Component identifier */
    suit_param_ref_t param_vendor_id;           /**< Vendor ID */
    suit_param_ref_t param_class_id;            /**< Class ID */
    suit_param_ref_t param_digest;              /**< Payload verification digest */
    suit_param_ref_t param_uri;                 /**< Payload fetch URI */
    suit_param_ref_t param_size;                /**< Payload size */

    /**
     * @brief Component offset inside the device memory.
     */
    suit_param_ref_t param_component_offset;
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
    suit_component_t components[CONFIG_SUIT_COMPONENT_MAX];
    unsigned components_len;        /**< Current number of components */
    uint8_t component_current;      /**< Current component index */
    /** Manifest validation buffer */
    uint8_t validation_buf[SUIT_COSE_BUF_SIZE];
    char *urlbuf;                   /**< Buffer containing the manifest url */
    size_t urlbuf_len;              /**< Length of the manifest url */
    uint32_t seq_number;            /**< Set sequence number */
} suit_manifest_t;

/**
 * @brief Component index representing all components
 *
 * Used when suit-directive-set-component-index = True
 */
#define SUIT_MANIFEST_COMPONENT_ALL     (UINT8_MAX)

/**
 * @brief Component index representing no components
 *
 * Used when suit-directive-set-component-index = False
 */
#define SUIT_MANIFEST_COMPONENT_NONE    (SUIT_MANIFEST_COMPONENT_ALL - 1)

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
 * @brief Set a component flag
 *
 * @param   component   Component to set flag for
 * @param   flag        Flag to set
 */
static inline void suit_component_set_flag(suit_component_t *component,
                                           uint16_t flag)
{
    component->state |= flag;
}

/**
 * @brief Check a component flag
 *
 * @param   component   Component to check a flag for
 * @param   flag        Flag to check
 *
 * @returns             True if the flag is set
 */
static inline bool suit_component_check_flag(suit_component_t *component,
                                             uint16_t flag)
{
    return (component->state & flag);
}

/**
 * @brief Convert a component name to a string
 *
 * Each component part is prefixed with @p separator
 *
 * @return          SUIT_OK if successful
 * @return          negative error code on error
 */
int suit_component_name_to_string(const suit_manifest_t *manifest,
                                  const suit_component_t *component,
                                  char separator, char *buf, size_t buf_len);
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SUIT_H */
