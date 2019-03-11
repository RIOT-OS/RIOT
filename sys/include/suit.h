/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit
 * @brief       SUIT manifest handling
 *
 * @{
 *
 * @brief       Handler functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * SUIT manifests require validation before they can be used as an update
 * target. This module handles the validation process.
 *
 * At the moment, only CBOR based SUIT manifests are supported
 */

#ifndef SUIT_H
#define SUIT_H

#include <stddef.h>
#include <stdint.h>

#include "suit/cbor.h"
#include "uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The SUIT vendor ID source
 *
 * The basis of the UUID must be the vendor domain, please change this when
 * using this module in a product
 */
#ifndef SUIT_VENDOR_DOMAIN
#define SUIT_VENDOR_DOMAIN  "riot-os.org" /**< Device vendor domain */
#endif

/**
 * @brief The SUIT class ID source
 *
 * By default the RIOT_VERSION define is used for this
 */
#ifndef SUIT_CLASS_ID
#define SUIT_CLASS_ID  RIOT_VERSION
#endif

/**
 * @brief Supported SUIT manifest version
 */
#define SUIT_MANIFEST_VERSION        1

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
} suit_error_t;

/**
 * @brief SUIT condition parameters
 */
typedef struct {
    uuid_t vendor;  /**< Vendor url as UUID                  */
    uuid_t class;   /**< Device class UUID                   */
    uuid_t device;  /**< Device specific information as UUID */
} suit_condition_params_t;

/**
 * @brief Initialize boot-time conditions for SUIT manifests
 *
 * This initializes the device-based conditions for validating manifest
 * preconditions
 *
 * Vendor url as UUID:   UUID5(DNS_PREFIX, SUIT_VENDOR_DOMAIN)
 * Device class UUID:    UUID5(vendor, SUIT_CLASS_ID)
 * Device specific UUID: UUID5(vendor, Device ID)
 */
void suit_init_conditions(void);

/**
 * @brief Parse a manifest
 *
 * @note The buffer is still required after parsing, please don't reuse the
 * buffer while the @p manifest is used
 *
 * @param   manifest    manifest context to store information in
 * @param   buf         buffer to parse the manifest from
 * @param   len         length of the manifest data in the buffer
 *
 * @return              SUIT_OK on parseable manifest
 * @return              negative @ref suit_error_t code on error
 */
int suit_parse(suit_cbor_manifest_t *manifest, uint8_t *buf, size_t len);

/**
 * @brief Validate a manifest
 *
 * This validates the manifest based on the manifest version, sequence number
 * and conditions
 * when this function returns @ref SUIT_OK, the manifest contains a valid and
 * applicable update for this device
 *
 * @param[in]   manifest        manifest context to validate
 * @param[in]   highest_seq_no  highest sequence number among firmwares
 *                              installed on this device
 *
 * @return              SUIT_OK when the manifest is valid and applicable
 * @return              negative @ref suit_error_t code on error
 */
int suit_validate_manifest(const suit_cbor_manifest_t *manifest,
                           uint32_t highest_seq_no);

/**
 * @brief Retrieve the generated vendor ID
 *
 * @returns     The vendor ID as UUID
 */
uuid_t *suit_get_vendor_id(void);

/**
 * @brief Retrieve the generated class ID
 *
 * @returns     The class ID as UUID
 */
uuid_t *suit_get_class_id(void);

/**
 * @brief Retrieve the generated device ID
 *
 * @returns     The device ID as UUID
 */
uuid_t *suit_get_device_id(void);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_H */
/** @} */
