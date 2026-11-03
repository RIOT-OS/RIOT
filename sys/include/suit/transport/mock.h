/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_suit_transport_mock SUIT secure firmware OTA mock transport
 * @ingroup     sys_suit
 * @brief       SUIT firmware mock transport
 *
 * @{
 *
 * @brief       Mock transport backend definitions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * The mock transport is a noop transport. Payloads are preloaded in flash and
 * provided as an array of @ref suit_transport_mock_payload_t to the module.
 *
 * Both the array of payloads named `payloads` and the size with name
 * `num_payloads` must be provided.
 */

#include <sys/types.h>

#include "suit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mock payload.
 */
typedef struct {
    const uint8_t *buf; /**< Ptr to the memory space containing the payload */
    size_t len;         /**< Length of the payload in bytes */
} suit_transport_mock_payload_t;

#ifdef DOXYGEN
/**
 * @brief Enable to provide a mocked manifest file from an application
 *
 * @see @ref suit_transport_mock_get_manifest()
 *
 * Set this if you want to provide a manifest in @ref suit_handle_url()
 * from a test application. The test application then needs to implement
 * the function @ref suit_transport_mock_get_manifest().
 */
#define SUIT_TRANSPORT_MOCK_APP_MANIFEST
#endif

#if defined(SUIT_TRANSPORT_MOCK_APP_MANIFEST) || defined(DOXYGEN)
/**
 * @brief  Get mock manifest from an application
 *
 * @param[out] buf      Manifest buffer
 * @param[in] buf_len   Maximum length of @p buf
 *
 * @see @ref SUIT_TRANSPORT_MOCK_APP_MANIFEST
 *
 * If @ref SUIT_TRANSPORT_MOCK_APP_MANIFEST is not defined, the function is a
 * macro that returns `0U`.
 *
 * @return Actual length of the manifest.
 * @return -ENOBUFS, if @p buf_len does not fit the manifest.
 */
ssize_t suit_transport_mock_get_manifest(uint8_t *buf, size_t buf_len);
#else
#define suit_transport_mock_get_manifest(buf, buf_len)  0U
#endif

/**
 * @brief 'fetch' a payload
 *
 * The payload fetched from the payloads array is indicated by the @ref
 * suit_manifest_t::component_current member
 *
 * @param[in]   manifest    suit manifest context
 *
 * @returns     SUIT_OK if valid
 * @returns     negative otherwise
 */
int suit_transport_mock_fetch(const suit_manifest_t *manifest);

#ifdef __cplusplus
}
#endif

/** @} */
