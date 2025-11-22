/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
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
 * @brief  Packet
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

/* We don't require a DTLS implementation to be present when the CoAP over DTLS driver is not used.
 * Including DTLS headers without an implementation fails. Hence, we create a local alias
 * that either maps to the system-provided DTLS session struct or to nothing. */

#ifndef DOXYGEN
#  if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
typedef sock_dtls_session_t unicoap_sock_dtls_session_t;
#  else
typedef void unicoap_sock_dtls_session_t;
#  endif
#endif

/**
 * @brief A type acting as an envelope for a message and data connected, like endpoints
 *
 * Using this type, you can allocate everything at once without needing to pass endpoints and
 * message objects as parameters.
 */
typedef struct {
    /**
     * @brief The remote endpoint this packet was received from or is destined for
     */
    const unicoap_endpoint_t* remote;

    /**
     * @brief The local endpoint this packet was received at or is to be sent from
     */
    const unicoap_endpoint_t* local;

    union {
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS) || defined(DOXYGEN)
        /**
         * @brief DTLS session
         *
         * Stored here to avoid looking up sessions in digital session management (DSM)
         */
        const sock_dtls_session_t* dtls_session;
#endif

        /* MARK: unicoap_driver_extension_point */
    };

    /**
     * @brief Message, public API, also used by PDU header parser
     */
    unicoap_message_t* message;

    /**
     * @brief Backing storage for properties, used by PDU header parser
     */
    unicoap_message_properties_t properties;
} unicoap_packet_t;

/**
 * @brief Retrieves protocol number from packet
 *
 * @param[in] packet Packet to read protocol number from
 *
 * @returns Protocol number
 */
static inline unicoap_proto_t unicoap_packet_proto(const unicoap_packet_t* packet)
{
    return packet->remote->proto;
}

/**
 * @brief Retrieves transport session from packet
 *
 * @param[in,out] packet Packet to retrieve session from
 *
 * @returns Transport session
 */
static inline const void* _packet_get_dtls_session(unicoap_packet_t* packet)
{
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    return unicoap_packet_proto(packet) == UNICOAP_PROTO_DTLS ? packet->dtls_session : NULL;
#else
    (void)packet;
    return NULL;
#endif
}

/**
 * @brief Sets DTLS transport session of packet
 *
 * @param[in,out] packet whose session to set
 * @param dtls_session DTLS session
 */
static inline void _packet_set_dtls_session(unicoap_packet_t* packet,
                                            const unicoap_sock_dtls_session_t* dtls_session)
{
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    packet->dtls_session = dtls_session;
#else
    (void)packet;
    (void)dtls_session;
#endif
}

#ifdef __cplusplus
}
#endif

/** @} */
