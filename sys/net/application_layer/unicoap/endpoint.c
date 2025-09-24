/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup net_unicoap_transport
 * @brief   Endpoint abstraction and URI support implementation
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "net/af.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "uri_parser.h"
#include "net/netif.h"
#include "net/unicoap/transport.h"
#include "net/unicoap/application.h"
#include "net/unicoap/message.h"

#if IS_USED(MODULE_DNS)
#  include "net/dns.h"
#endif

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define STRLEN(str) (sizeof(str) - 1)

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
void unicoap_print_sock_tl_ep(const struct _sock_tl_ep* ep)
{
    (void)ep;
    printf("<sock_tl_ep port=%" PRIu16 " netif=%" PRIu16 " ", ep->port, ep->netif);
    switch (ep->family) {
    case AF_INET6:
        printf("ipv6=");
#  if SOCK_HAS_IPV6 && IS_USED(MODULE_IPV6_ADDR)
        ipv6_addr_print((ipv6_addr_t*)ep->addr.ipv6);
#  else
        UNICOAP_DEBUG("SOCK_HAS_IPV6: v6 missing\n");
        assert(false);
        printf("?");
#  endif /* SOCK_HAS_IPV6 && IS_USED(MODULE_IPV6_ADDR) */
        break;
    case AF_INET:
        printf("ipv4=");
#  if SOCK_HAS_IPV6 && IS_USED(MODULE_IPV4_ADDR)
        ipv4_addr_print((ipv4_addr_t*)ep->addr.ipv4);
#  else
        UNICOAP_DEBUG("SOCK_HAS_IPV4: v4 missing\n");
        assert(false);
        printf("?");
#  endif /* SOCK_HAS_IPV6 && IS_USED(MODULE_IPV4_ADDR) */
        break;

    default:
        printf("family=%i", ep->family);
        break;
    }
    printf(">");
}
#endif

void unicoap_print_endpoint(const unicoap_endpoint_t* endpoint)
{
    printf("%s ", unicoap_string_from_proto(endpoint->proto));

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
    if (unicoap_transport_uses_sock_tl_ep(endpoint->proto)) {
        unicoap_print_sock_tl_ep(&endpoint->_tl_ep);
        return;
    }
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) */

    printf("<endpoint (unprintable) proto=%u>", endpoint->proto);
}

const char* unicoap_string_from_proto(unicoap_proto_t proto)
{
    switch (proto) {
    case UNICOAP_PROTO_UDP:
        return "UDP";
    case UNICOAP_PROTO_DTLS:
        return "DTLS";
        /* MARK: unicoap_driver_extension_point */
    default:
        return "?";
    }
}

bool unicoap_endpoint_is_equal(const unicoap_endpoint_t* source,
                               const unicoap_endpoint_t* destination)
{
    (void)source;
    (void)destination;
    if (source->proto != destination->proto) {
        return false;
    }
    switch (source->proto) {
#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS:
        return sock_tl_ep_equal(&source->_tl_ep, &destination->_tl_ep);
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) */
    /* MARK: unicoap_driver_extension_point */
    default:
        assert(false);
        return false;
    }
}

bool unicoap_endpoint_is_multicast(const unicoap_endpoint_t* endpoint)
{
    switch (endpoint->proto) {
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS:
#if IS_USED(MODULE_SOCK_UDP)
        return sock_udp_ep_is_multicast(&endpoint->udp_ep);
#else
        UNICOAP_DEBUG("UDP/DTLS driver is missing, cannot check if multicast addr\n");
        return false;
#  endif
    /* MARK: unicoap_driver_extension_point */
    default:
        assert(false);
        return false;
    }
}

/* TODO: Client: URI */
