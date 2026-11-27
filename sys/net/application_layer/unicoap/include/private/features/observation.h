/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>

#include "net/unicoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup net_unicoap_private
 * @{
 */

/**
 * @file
 * @brief  Observation API
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

#ifndef DOXYGEN
/* 1 Type/Length byte (length 3 is smaller than 15), 3 bytes max */
#  define _UNICOAP_OPTIONS_STORAGE_CAPACITY_JUST_GENERATED_OBSERVE (1 + 3)
#endif

/* MARK: - Resource observation in servers */
/**
 * @name Resource observation in servers
 * @{
 */
/**
 * @brief Registers the @p remote endpoint for notifications from the given @p resource
 *
 * @param[in] remote Endpoint, does not need to be in the state's client array.
 * @param[in] resource Resource to register @p remote for
 * @param[in] properties Request properties
 *
 * @returns `0` on success, negative integer on error.
 */
int unicoap_observation_register(const unicoap_endpoint_t* remote,
                                 const unicoap_resource_t* resource,
                                 const unicoap_message_properties_t* properties);

/**
 * @brief Registers the @p remote endpoint from notifications from the given @p resource
 *
 * @param[in] remote Endpoint, does not need to be in the state's client array.
 * @param[in] resource Resource to register @p remote for
 * @param[in] properties Request properties
 *
 * @returns `0` on success, negative integer on error.
 */
int unicoap_observation_deregister(const unicoap_endpoint_t* remote,
                                   const unicoap_resource_t* resource,
                                   const unicoap_message_properties_t* properties);

/**
 * @brief Tries to deregister @p remote endpoint from notifications.
 *
 * Use this function after receiving an `RST` message. This function tries to find a registration
 * with its last notification message id set to the given @p message_id .
 *
 * @param[in] remote Endpoint, does not need to be in the state's client array.
 * @param[in] message_id Message ID of potential notification sent by unicoap.
 */
void unicoap_observation_deregister_by_id(const unicoap_endpoint_t* remote, uint16_t message_id);
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif
