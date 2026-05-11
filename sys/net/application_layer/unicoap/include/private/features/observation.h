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
 * @brief Registration for notifications from the specified resource
 */
typedef struct {
    /** @brief Client in internal registered observers array */
    unicoap_endpoint_t* client;

    /** @brief The resource the client registered for  */
    const unicoap_resource_t* resource;

    /** @brief Token length  */

    /** @brief The token of the registration request received from the client */
    uint8_t token[CONFIG_UNICOAP_EXTERNAL_TOKEN_LENGTH_MAX];

    /**
     * @brief Message ID of most recently sent notification, used to deregister when receiving a `RST`
     *
     * RFC 7252 only.
     **/
    uint16_t last_notification_id;

    /**
     * @brief Length of @ref token
     */
    uint8_t token_length : UNICOAP_TOKEN_LENGTH_FIXED_WIDTH;

} unicoap_observation_registration_t;

/**
 * @brief An enum indicating the result of a resource observation client lookup
 */
typedef enum {
    /**
     * @brief Client has already registered for a resource hosted by unicoap.
     *
     * @note `client` points to client in the list of known clients.
     */
    UNICOAP_OBSERVATION_CLIENT_KNOWN = 0,

    /**
     * @brief Client has not yet registered for any resource hosted by unicoap,
     * but can be added to the list of known clients.
     *
     * @note `client` points to a free element in the list of known clients.
     */
    UNICOAP_OBSERVATION_CLIENT_AVAILABLE = 8,

    /**
     * @brief Client has not yet registered for any resource hosted by unicoap,
     * and **cannot** be added to the list of known clients.
     *
     * @note `client` is `NULL`.
     */
    UNICOAP_OBSERVATION_CLIENT_UNAVAILABLE = 9
} __attribute__((__packed__)) unicoap_observation_client_result_t;

/**
 * @brief An enum indicating the result of an observation registration attempt
 */
typedef enum {
    /**
     * @brief Client has already registered for this resource.
     *
     * @note `registration` points to existing registration.
     */
    UNICOAP_OBSERVATION_REGISTRATION_EXISTS = 0,

    /**
     * @brief Client has already registered for this resource with another token,
     * cannot re-register.
     *
     * @note `registration` is `NULL`.
     */
    UNICOAP_OBSERVATION_REGISTRATION_TOKEN_MISMATCH = 4,

    /**
     * @brief Client has not yet registered for this resource, but can register.
     *
     * @note `registration` points to a free element in list of registrations.
     */
    UNICOAP_OBSERVATION_REGISTRATION_AVAILABLE = 8,

    /**
     * @brief Client has not yet registered for this resource, and **cannot** register.
     *
     * @note `registration`is `NULL`.
     */
    UNICOAP_OBSERVATION_REGISTRATION_UNAVAILABLE = 9,

    /**
     * @brief Client has not yet registered for this resource, and **cannot** register. Resource is
     * already being observed by another client.
     *
     * @note `registration`is `NULL`.
     * @note Only possible in @ref CONFIG_UNICOAP_OBSERVATION_SINGLE_CLIENT_PER_RESOURCE mode.
     */
    UNICOAP_OBSERVATION_REGISTRATION_RESOURCE_IN_USE = 3

} __attribute__((__packed__)) unicoap_observation_registration_result_t;

/**
 * @brief Checks if given @p client has registered with any resources.
 * @warning @p client must be present in the client array in the state object.
 */
bool unicoap_observation_client_has_any_registrations(const unicoap_endpoint_t* client);

/**
 * @brief Looks up @p remote in the state's client array, sets @p client to client, if found.
 *
 * @param[in] remote Remote endpoint
 * @param[out] client Known client, if found, or free client element to use.
 *
 * @returns @ref UNICOAP_OBSERVATION_CLIENT_KNOWN
 * @returns @ref UNICOAP_OBSERVATION_CLIENT_AVAILABLE
 * @returns @ref UNICOAP_OBSERVATION_CLIENT_UNAVAILABLE
 */
unicoap_observation_client_result_t unicoap_observation_find_client(const unicoap_endpoint_t* remote,
                                                            unicoap_endpoint_t** client);

/**
 * @brief Checks if @p client is registered for the notifications from the given @p resource .
 *
 * @param[in] client found with @ref unicoap_observation_find_client
 * @param[in] resource Resource the client has potentially registered for
 * @param[in] properties Request properties, required to get token and ID
 * @param[out] registration Registration, if found, or free registration element to use.
 *
 * @returns @ref UNICOAP_OBSERVATION_REGISTRATION_EXISTS
 * @returns @ref UNICOAP_OBSERVATION_REGISTRATION_TOKEN_MISMATCH
 * @returns @ref UNICOAP_OBSERVATION_REGISTRATION_AVAILABLE
 * @returns @ref UNICOAP_OBSERVATION_REGISTRATION_UNAVAILABLE
 * @returns @ref UNICOAP_OBSERVATION_REGISTRATION_RESOURCE_IN_USE
 *
 */
unicoap_observation_registration_result_t unicoap_observation_find_registration(
    const unicoap_endpoint_t* client, const unicoap_resource_t* resource,
    const unicoap_message_properties_t* properties, unicoap_observation_registration_t** registration);

/**
 * @brief Registers the @p remote endpoint for notifications from the given @p resource
 *
 * @param[in] remote Endpoint, does not need to be in the state's client array.
 * @param[in] resource Resource to register @p remote for
 * @param[in] properties Request properties
 * @param[in] profile Profile
 *
 * @returns `0` on success, negative integer on error.
 */
int unicoap_observation_register(const unicoap_endpoint_t* remote, const unicoap_resource_t* resource,
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
int unicoap_observation_deregister(const unicoap_endpoint_t* remote, const unicoap_resource_t* resource,
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
