/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "iolist.h"

#include "timex.h"
#include "uri_parser.h"
#include "modules.h"

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN)
#  include "net/sock.h"
#  include "net/sock/async/event.h"
#  include "net/sock/util.h"
#  include "net/sock/udp.h"
#  include "net/sock/tcp.h"
#endif

#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS) || defined(DOXYGEN)
#  include "net/sock/dtls.h"
#endif

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
#  if !SOCK_HAS_ASYNC
#    error "currently enabled unicoap drivers require SOCK_HAS_ASYNC"
#  endif
#  if IS_ACTIVE(CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS) && !IS_USED(MODULE_SOCK_AUX_LOCAL)
#    error "CONFIG_UNICOAP_GET_LOCAL_ENDPOINTS requires sock_aux_local module"
#  endif
#endif

/* MARK: unicoap_driver_extension_point */

#include "net/unicoap/message.h"
#include "net/unicoap/options.h"

/**
 * @defgroup net_unicoap_transport Transport Protocol Abstractions
 * @ingroup  net_unicoap
 * @brief    CoAP endpoint, resource identifiers, and schemes
 * @{
 */

/**
 * @file
 * @brief  CoAP transport protocol abstraction layer
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - CoAP URI schemes */
/**
 * @name CoAP URI schemes
 * @{
 * The goal for CoAP is to have `coap` as the only scheme, with the underlying transport being
 * inferred using
 * [CoAP Transport Indication](https://datatracker.ietf.org/doc/draft-ietf-core-transport-indication).
 */
/**
 * @brief Default CoAP URI scheme
 */
#define UNICOAP_SCHEME                        "coap"

/**
 * @brief URI scheme for CoAP over UDP
 */
#define UNICOAP_SCHEME_UDP                    "coap"

/**
 * @brief URI scheme for CoAP over DTLS
 */
#define UNICOAP_SCHEME_DTLS                   "coaps"

/**
 * @brief URI scheme for CoAP over TCP
 */
#define UNICOAP_SCHEME_TCP                    "coap+tcp"

/**
 * @brief URI scheme for CoAP over TLS
 */
#define UNICOAP_SCHEME_TLS                    "coaps+tcp"

/**
 * @brief URI scheme for CoAP over WebSockets
 */
#define UNICOAP_SCHEME_WEBSOCKET              "coap+ws"

/**
 * @brief URI scheme for CoAP over secure WebSockets (over TLS)
 */
#define UNICOAP_SCHEME_WEBSOCKET_TLS          "coaps+ws"

/**
 * @brief URI scheme for CoAP over GATT over Bluetooth Low Energy (BLE)
 */
#define UNICOAP_SCHEME_BLE_GATT               "coap"

/**
 * @brief Domain for for CoAP over GATT over Bluetooth Low Energy (BLE)
 *
 * Example URI: `coap://001122334455.ble.arpa/.well-known/core`,
 * with `001122334455` being the MAC address of the BLE device.
 */
#define UNICOAP_DOMAIN_BLE                    ".ble.arpa"

/* MARK: unicoap_driver_extension_point */
/** @}  */

/* MARK: - Endpoints */
/**
 * @name Endpoints
 * @{
 */
/**
 * @brief Unspecified protocol number
 *
 * Use this protocol number to mark endpoints uninitialized.
 */
#define UNICOAP_PROTO_UNSPECIFIED             (0)

/**
 * @brief Protocol number flag indicating the underlying endpoint inherits from @ref _sock_tl_ep
 */
#define UNICOAP_PROTO_FLAG_RELIABLE_TRANSPORT (1)

/**
 * @brief Transport protocol CoAP is used over.
 */
typedef enum {
    /** @brief CoAP over UDP endpoint */
    UNICOAP_PROTO_UDP = 1 << 1,

    /** @brief CoAP over DTLS over UDP endpoint */
    UNICOAP_PROTO_DTLS = 2 << 1,

    /* MARK: unicoap_driver_extension_point */
} __attribute__((__packed__)) unicoap_proto_t;

/**
 * @brief Determines whether the given transport protocol is reliable
 *
 * Used for enabling the Block-wise Extension for Reliable Transports (BERT).
 *
 * @note BERT allows multiple blocks of size 1024 bytes to be included in a single CoAP message.
 */
static inline bool unicoap_transport_is_reliable(unicoap_proto_t proto)
{
    return proto & UNICOAP_PROTO_FLAG_RELIABLE_TRANSPORT;
}

/**
 * @brief Determines whether the given transport driver uses a transport layer socket in RIOT
 *
 * Used when parsing URIs and when printing an endpoint. If true, the socket endpoint can be
 * casted `struct _sock_tl_ep*`.
 *
 * ### Examples
 * - @ref sock_udp
 * - @ref sock_dtls
 * - @ref sock_tcp
 *
 */
static inline bool unicoap_transport_uses_sock_tl_ep(unicoap_proto_t proto)
{
    (void)proto;
    /* If a new transport driver does not use RIOT's socket API,
     * such as CoAP over GATT, return false here. */
    /* MARK: unicoap_driver_extension_point */
    return true;
}

/**
 * @brief A CoAP endpoint
 */
typedef struct {
    /** @brief Protocol number */
    unicoap_proto_t proto;

    union {
        /* union members are guaranteed to start at offset zero */

#     if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN)
        /** @brief Transport layer endpoint */
        struct _sock_tl_ep _tl_ep;

        /* _tl_ep acts as a view on both udp_ep and dtls_ep. */

        /** @brief RIOT sock UDP endpoint */
        sock_udp_ep_t udp_ep;

        /** @brief RIOT sock DTLS endpoint */
        sock_udp_ep_t dtls_ep;
#      endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN) */

        /* MARK: unicoap_driver_extension_point */
    };
} unicoap_endpoint_t;

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN)
/**
 * @brief Retrieves UDP endpoint from CoAP endpoint
 * @pre @p endpoint is a CoAP over UDP endpoint (proto == @ref UNICOAP_PROTO_UDP)
 *
 * @memberof unicoap_endpoint_t
 *
 * @param[in,out] endpoint CoAP over UDP endpoint
 *
 * @returns @ref sock_udp_ep_t
 */
static inline sock_udp_ep_t* unicoap_endpoint_get_udp(unicoap_endpoint_t* endpoint)
{
    return &endpoint->udp_ep;
}

/**
 * @brief Retrieves DTLS endpoint from CoAP endpoint
 * @pre @p endpoint is a CoAP over DTLS endpoint (proto == @ref UNICOAP_PROTO_DTLS)
 *
 * @memberof unicoap_endpoint_t
 *
 * @param[in,out] endpoint CoAP over DTLS endpoint
 *
 * @returns @ref sock_udp_ep_t
 */
static inline sock_udp_ep_t* unicoap_endpoint_get_dtls(unicoap_endpoint_t* endpoint)
{
    return &endpoint->dtls_ep;
}

#  ifndef DOXYGEN
/**
 * @brief Private API. Retrieves transport layer endpoint from CoAP endpoint
 *
 * @memberof unicoap_endpoint_t
 *
 * @param[in,out] endpoint CoAP endpoint, pre-allocated
 *
 * @returns @ref _sock_tl_ep
 */
static inline struct _sock_tl_ep* _unicoap_endpoint_get_tl(unicoap_endpoint_t* endpoint)
{
    return &endpoint->_tl_ep;
}
#  endif /* !defined(DOXYGEN) */
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN) */
/** @} */

/* MARK: - Conversions and Tools */
/**
 * @name Conversions and Tools
 * @{
 */

/* TODO: Client: URI */

/**
 * @brief Returns scheme from protocol number
 *
 * @param proto Protocol number
 * @returns Null-terminated transport description, such as `"UDP"` or `"DTLS"`
 * @returns `"?"` if protocol number is unknown
 */
const char* unicoap_string_from_proto(unicoap_proto_t proto);

/**
 * @brief Compares two endpoints
 *
 * @param[in] source Endpoint 1
 * @param[in] destination Endpoint 2
 * @returns A boolean value indicating whether the two endpoints are considered identical
 * semantically
 */
bool unicoap_endpoint_is_equal(const unicoap_endpoint_t* source,
                               const unicoap_endpoint_t* destination);

/**
 * @brief Determines whether the given endpoint is a multicast endpoint
 *
 * @param[in] endpoint Endpoint to check
 * @returns A boolean value indicating whether the endpoints is considered a multicast endpoint
 */
bool unicoap_endpoint_is_multicast(const unicoap_endpoint_t* endpoint);

/**
 * @brief Prints the given transport layer endpoint
 *
 * @param[in] ep Transport layer endpoint
 */
#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN)
void unicoap_print_sock_tl_ep(const struct _sock_tl_ep* ep);
#else
static inline void unicoap_print_sock_tl_ep(const void* ep)
{
    (void)ep;
    assert(false);
}
#endif
/**
 * @brief Prints the given CoAP endpoint
 *
 * @param[in] endpoint Endpoint
 */
void unicoap_print_endpoint(const unicoap_endpoint_t* endpoint);
/** @} */
/** @} */

/**
 * @addtogroup net_unicoap_drivers_udp
 * @{
 */
/* MARK: - Sockets */
/**
 * @name Sockets
 * @{
 */
/** @brief Returns the internal UDP socket */
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP) || defined(DOXYGEN)
sock_udp_t* unicoap_transport_udp_get_socket(void);
#else
static inline
void* unicoap_transport_udp_get_socket(void)
{
    return NULL;
}
#endif

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN)
/* sock_udp_t requires an implementation which is not present if someone uses unicoap
 * without a driver, i.e., just message APIs. In this case, unicoap_sock_support is not imported. */

/**
 * @brief Adds UDP socket for client and server functionality
 * @pre Socket must be pre-allocated and must remain allocated until the UDP driver has been
 * deinitialized or the socket is closed by you.
 *
 * @param[in,out] socket Pre-allocated socket to use
 * @param[in]     local  Initialized endpoint, does not need to outlive this function call
 *
 * You can call this function at any time after `unicoap` has been initialized.
 */
#  if IS_USED(MODULE_UNICOAP_DRIVER_UDP) || defined(DOXYGEN)
int unicoap_transport_udp_add_socket(sock_udp_t* socket, sock_udp_ep_t* local);
#  else
static inline
int unicoap_transport_udp_add_socket(sock_udp_t* socket, sock_udp_ep_t* local)
{
    (void)socket;
    (void)local;
    return 0;
}
#  endif

/**
 * @brief Removes UDP socket previously added manually
 *
 * @param[in,out] socket The socket pointer you have previously passed to
 *                       @ref unicoap_transport_udp_add_socket
 *
 */
#  if IS_USED(MODULE_UNICOAP_DRIVER_UDP) || defined(DOXYGEN)
int unicoap_transport_udp_remove_socket(sock_udp_t* socket);
#  else
static inline
int unicoap_transport_udp_remove_socket(sock_udp_t* socket)
{
    (void)socket;
    return 0;
}
#  endif
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN) */
/** @} */
/** @} */

/**
 * @addtogroup net_unicoap_drivers_dtls
 * @{
 */
/* MARK: - Sockets */
/**
 * @name Sockets
 * @{
 */
/** @brief Returns the internal DTLS socket */
#  if IS_USED(MODULE_UNICOAP_DRIVER_DTLS) || defined(DOXYGEN)
sock_dtls_t* unicoap_transport_dtls_get_socket(void);
#  else
static inline
void* unicoap_transport_dtls_get_socket(void)
{
    return NULL;
}
#  endif

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN)
/* sock_udp_t requires an implementation which is not present if someone uses unicoap
 * without a driver, i.e., just message APIs. In this case, unicoap_sock_support is not imported. */

/**
 * @brief Adds DTLS socket for client and server functionality
 * @pre Sockets must be pre-allocated and must remain allocated until the DTLS driver has been
 * deinitialized or the socket is closed by you.
 *
 * @param[in,out] socket      Pre-allocated socket to use
 * @param[in,out] base_socket Pre-allocated UDP base socket to use
 * @param[in]     local       Initialized endpoint, does not need to outlive this function call
 *
 * You can call this function at any time after `unicoap` has been initialized.
 */
#  if IS_USED(MODULE_UNICOAP_DRIVER_DTLS) || defined(DOXYGEN)
int unicoap_transport_dtls_add_socket(sock_dtls_t* socket, sock_udp_t* base_socket,
                                      sock_udp_ep_t* local);
#  else
static inline
int unicoap_transport_dtls_add_socket(sock_dtls_t* socket, sock_udp_t* base_socket,
                                      sock_udp_ep_t* local)
{
    (void)socket;
    (void)base_socket;
    (void)local;
    return 0;
}
#  endif

/**
 * @brief Removes DTLS socket previously added manually
 *
 * @param[in,out] socket The socket pointer you have previously passed to
 *                       @ref unicoap_transport_dtls_add_socket
 *
 */
#  if IS_USED(MODULE_UNICOAP_DRIVER_DTLS) || defined(DOXYGEN)
int unicoap_transport_dtls_remove_socket(sock_dtls_t* socket);
#  else
static inline
int unicoap_transport_dtls_remove_socket(sock_dtls_t* socket)
{
    (void)socket;
    return 0;
}
#  endif
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) || defined(DOXYGEN) */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif
