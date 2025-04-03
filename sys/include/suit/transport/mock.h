/*
 * Copyright (C) 2020 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef SUIT_TRANSPORT_MOCK_H
#define SUIT_TRANSPORT_MOCK_H

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
#endif /* SUIT_TRANSPORT_MOCK_H */
