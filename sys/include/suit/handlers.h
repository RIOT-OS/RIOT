/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_suit
 * @brief       SUIT draft-ietf-suit-manifest-03 manifest handlers
 *
 * @experimental
 *
 * @{
 *
 * @brief       Handler functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stddef.h>
#include <stdint.h>

#include "suit.h"
#include "uuid.h"
#include "nanocbor/nanocbor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SUIT outer wrapper identifiers
 * @{
 */
#define SUIT_WRAPPER_AUTHENTICATION     (2)
#define SUIT_WRAPPER_MANIFEST           (3)
/** @} */

/**
 * @name SUIT container identifiers
 * @{
 */
#define SUIT_CONTAINER_VERSION          (1)
#define SUIT_CONTAINER_SEQ_NO           (2)
#define SUIT_CONTAINER_COMMON           (3)
#define SUIT_CONTAINER_DEPS_RESOLUTION  (7)
#define SUIT_CONTAINER_PAYLOAD_FETCH    (8)
#define SUIT_CONTAINER_INSTALL          (9)
#define SUIT_CONTAINER_VALIDATE        (10)
#define SUIT_CONTAINER_LOAD            (11)
#define SUIT_CONTAINER_RUN             (12)
#define SUIT_CONTAINER_TEXT            (13)
/** @} */

/**
 * @name SUIT common section identifiers
 * @{
 */
#define SUIT_COMMON_DEPENDENCIES        (1)
#define SUIT_COMMON_COMPONENTS          (2)
#define SUIT_COMMON_DEP_COMPONENTS      (3)
#define SUIT_COMMON_COMMAND_SEQUENCE    (4)
/** @} */

/**
 * @name SUIT condition identifiers
 * @{
 */
#define SUIT_COND_VENDOR_ID             (1)
#define SUIT_COND_CLASS_ID              (2)
#define SUIT_COND_IMAGE_MATCH           (3)
#define SUIT_COND_USE_BEFORE            (4)
#define SUIT_COND_COMPONENT_OFFSET      (5)
#define SUIT_COND_DEVICE_ID            (24)
#define SUIT_COND_IMAGE_NOT_MATCH      (25)
#define SUIT_COND_MIN_BATTERY          (26)
#define SUIT_COND_UPDATE_AUTHZ         (27)
#define SUIT_COND_VERSION              (28)
/** @} */

/**
 * @name SUIT directive identifiers
 * @{
 */
#define SUIT_DIR_SET_COMPONENT_IDX     (12)
#define SUIT_DIR_SET_DEPENDENCY_IDX    (13)
#define SUIT_DIR_ABORT                 (14)
#define SUIT_DIR_TRY_EACH              (15)
#define SUIT_DIR_PROCESS_DEPS          (18)
#define SUIT_DIR_SET_PARAM             (19)
#define SUIT_DIR_OVERRIDE_PARAM        (20)
#define SUIT_DIR_FETCH                 (21)
#define SUIT_DIR_COPY                  (22)
#define SUIT_DIR_RUN                   (23)
#define SUIT_DIR_WAIT                  (29)
#define SUIT_DIR_RUN_SEQUENCE          (30)
#define SUIT_DIR_RUN_WITH_ARGS         (31)
#define SUIT_DIR_SWAP                  (32)
/** @} */

/**
 * @brief suit handler prototype
 *
 * @param manifest  SUIT manifest context
 * @param key       SUIT map index of this content
 * @param it        nanocbor_value_t iterator to the content
 *
 * @return          SUIT_OK on success
 * @return          negative on error
 */
typedef int (*suit_manifest_handler_t)(suit_manifest_t *manifest, int key,
                                       nanocbor_value_t *it);

/**
 * @brief global handler reference
 */
extern const suit_manifest_handler_t suit_global_handlers[];
extern const size_t suit_global_handlers_len;

/**
 * @brief SUIT sequence handler reference
 */
extern const suit_manifest_handler_t suit_command_sequence_handlers[];

/**
 * @brief SUIT sequence handler length
 */
extern const size_t suit_command_sequence_handlers_len;

/**
 * @brief SUIT envelope handlers reference
 */
extern const suit_manifest_handler_t suit_envelope_handlers[];

/**
 * @brief length of the SUIT envelope handlers
 */
extern const size_t suit_envelope_handlers_len;

/**
 * @brief SUIT common handlers reference
 */
extern const suit_manifest_handler_t suit_common_handlers[];

/**
 * @brief length of the SUIT common handlers
 */
extern const size_t suit_common_handlers_len;

/**
 * @brief Manifest structure handler function
 *
 * Iterates over the supplied nanocbor map or array and calls the manifest
 * handler function for every key.
 *
 * @param   manifest        SUIT manifest context
 * @param   it              Nanocbor map/array element
 * @param   handlers        Array of SUIT manifest handlers to use
 * @param   handlers_len    Length of the SUIT manifest handlers
 *
 * @returns     SUIT_OK if all handlers executed successfully
 * @returns     negative on error, see @ref suit_error_t
 */
int suit_handle_manifest_structure(suit_manifest_t *manifest,
                                   nanocbor_value_t *it,
                                   const suit_manifest_handler_t *handlers,
                                   size_t handlers_len);

/**
 * @brief Byte string wrapped manifest structure handler function
 *
 * Extracts the nanocbor byte string and Iterates over the CBOR map or array
 * contained in the bytestring and calls the manifest handler function for
 * every key.
 *
 * @param   manifest        SUIT manifest context
 * @param   bseq            Nanocbor byte string
 * @param   handlers        Array of SUIT manifest handlers to use
 * @param   handlers_len    Length of the SUIT manifest handlers
 *
 * @returns     SUIT_OK if all handlers executed successfully
 * @returns     negative on error, see @ref suit_v3_error_t
 */
int suit_handle_manifest_structure_bstr(suit_manifest_t *manifest,
                                        nanocbor_value_t *bseq,
                                        const suit_manifest_handler_t *handlers,
                                        size_t handlers_len);

/**
 * @brief Create an internal @ref suit_param_ref_t from a NanoCBOR value
 *        reference.
 *
 * The resulting @p ref only contains a 16 bit offset to the location of the
 * NanoCBOR value, saving some RAM for each stored reference.
 *
 * @param   manifest    SUIT manifest context
 * @param   ref         reference to store
 * @param   val         NanoCBOR value to convert to a reference
 *
 * @returns             The offset of the nanocbor value inside the manifest
 *                      bytestring
 */
uint16_t suit_param_ref_to_cbor(const suit_manifest_t *manifest,
                                const suit_param_ref_t *ref,
                                nanocbor_value_t *val);

/**
 * @brief Create a NanoCBOR value reference from an internal
 *        @ref suit_param_ref_t.
 *
 * @param   manifest    SUIT manifest context
 * @param   ref         reference to parse
 * @param   val         NanoCBOR value to restore
 */
void suit_param_cbor_to_ref(const suit_manifest_t *manifest,
                           suit_param_ref_t *ref,
                           const nanocbor_value_t *val);

#ifdef __cplusplus
}
#endif

/** @} */
