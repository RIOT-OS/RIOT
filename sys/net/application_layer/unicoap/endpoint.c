/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "net/af.h"
#include "uri_parser.h"
#include "net/netif.h"
#include "net/unicoap/transport.h"
#include "net/unicoap/message.h"
#include "net/unicoap/server.h"

#if IS_USED(MODULE_DNS)
#  include "net/dns.h"
#endif

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define STRLEN(str) (sizeof(str) - 1)

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
void unicoap_print_sock_tl_ep(const struct _sock_tl_ep* ep) {
    (void)ep;
    printf("<sock_tl_ep port=%" PRIu16 " netif=%" PRIu16 " ", ep->port, ep->netif);
    switch (ep->family) {
    case AF_INET6:
        printf("ipv6=");
#  if SOCK_HAS_IPV6 && IS_USED(MODULE_IPV6_ADDR)
        ipv6_addr_print((ipv6_addr_t*)ep->addr.ipv6);
#  else
        UNICOAP_DEBUG("SOCK_HAS_IPV6: v6 support missing, cannot print\n");
        printf("?");
#  endif /* SOCK_HAS_IPV6 && IS_USED(MODULE_IPV6_ADDR) */
        break;
    case AF_INET:
        printf("ipv4=");
#  if SOCK_HAS_IPV4 && IS_USED(MODULE_IPV4_ADDR)
        ipv4_addr_print((ipv4_addr_t*)ep->addr.ipv4);
#  else
        UNICOAP_DEBUG("SOCK_HAS_IPV4: v4 support missing, cannot print\n");
        printf("?");
#  endif /* SOCK_HAS_IPV6 && IS_USED(MODULE_IPV4_ADDR) */
        break;

    default:
        printf("family=%i", ep->family);
        break;
    }
    printf(">");
}
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) */

void unicoap_print_endpoint(const unicoap_endpoint_t* endpoint) {
    printf("%s ", unicoap_string_from_proto(endpoint->proto));

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
    if (unicoap_transport_uses_sock_tl_ep(endpoint->proto)) {
        unicoap_print_sock_tl_ep(&endpoint->_tl_ep);
        return;
    }
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) */

    printf("<endpoint (unprintable) proto=%u>", endpoint->proto);
}

const char* unicoap_string_from_proto(unicoap_proto_t proto) {
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

bool unicoap_endpoint_is_equal(const unicoap_endpoint_t* lhs,
                               const unicoap_endpoint_t* rhs) {
    (void)lhs;
    (void)rhs;
    if (lhs->proto != rhs->proto) {
        return false;
    }
    switch (lhs->proto) {
#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS:
        return sock_tl_ep_equal(&lhs->_tl_ep, &rhs->_tl_ep);
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) */
    /* MARK: unicoap_driver_extension_point */
    default:
        assert(false);
        return false;
    }
}

bool unicoap_endpoint_is_multicast(const unicoap_endpoint_t* endpoint) {
    switch (endpoint->proto) {
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS:
#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
        return sock_udp_ep_is_multicast(&endpoint->udp_ep);
#else
        UNICOAP_DEBUG("sock support is missing, cannot check if multicast addr, driver missing?\n");
        return false;
#endif
    /* MARK: unicoap_driver_extension_point */
    default:
        assert(false);
        return false;
    }
}

#define _UNICOAP_BLE_DOMAIN_LENGTH (sizeof(UNICOAP_DOMAIN_BLE) - 1)

int unicoap_proto_from_scheme_and_host(const char *scheme, size_t scheme_length, const char *host, size_t host_length) {
    (void)host;
    (void)host_length;
    if (strncmp(scheme, UNICOAP_SCHEME, scheme_length) == 0) {
        /* MARK: unicoap_driver_extension_point */
        /* CoAP over GATT/BLE domain checks would happen here. */
        return UNICOAP_PROTO_UDP;
    }
    else if (strncmp(scheme, UNICOAP_SCHEME_DTLS, scheme_length) == 0) {
        return UNICOAP_PROTO_DTLS;
    }
    /* MARK: unicoap_driver_extension_point */
    return -ENOTSUP;
}

const char* unicoap_scheme_from_proto(unicoap_proto_t proto) {
    switch (proto) {
    case UNICOAP_PROTO_UDP:
        return UNICOAP_SCHEME_UDP;
    case UNICOAP_PROTO_DTLS:
        return UNICOAP_SCHEME_DTLS;
        /* MARK: unicoap_driver_extension_point */
    default:
        return NULL;
    }
}

#define _UNICOAP_URI_RESOLVED_HOST (1)

static int _populate_address(uri_parser_result_t* parsed, unicoap_endpoint_t* endpoint) {
    int16_t netif_id = SOCK_ADDR_ANY_NETIF;
    uint16_t* _netif_id = unicoap_endpoint_get_netif_id(endpoint);
    if (_netif_id && parsed->zoneid && parsed->zoneid_len > 0) {
        if (IS_USED(MODULE_NETIF)) {
            netif_t* netif = netif_get_by_name_buffer(parsed->zoneid, parsed->zoneid_len);
            if (!netif || (netif_id = netif_get_id(netif)) < 0) {
                _URI_DEBUG("error: unknown zone ID\n");
                return -ENOENT;
            }
            *_netif_id = netif_id;
        } else {
            unicoap_assist(API_ERROR("cannot resolve zone id") FIXIT("USEMODULE += netif"));
            return -ENOTSUP;
        }
    }

    uint16_t* port = unicoap_endpoint_get_port(endpoint);
    if (port) {
        *port = parsed->port;
    }

    ipv6_addr_t* v6addr = unicoap_endpoint_get_ipv6_addr(endpoint);
    ipv4_addr_t* v4addr = unicoap_endpoint_get_ipv4_addr(endpoint);
    if (parsed->ipv6addr && (parsed->ipv6addr_len > 0)) {
        ipv6_addr_from_buf(v6addr, parsed->ipv6addr, parsed->ipv6addr_len);
        *unicoap_endpoint_get_address_family(endpoint) = AF_INET6;
        return 0;
    } else if (ipv4_addr_from_buf(v4addr, parsed->host, parsed->host_len)) {
        /* The host part is actually an IPv4 address literal. */
        *unicoap_endpoint_get_address_family(endpoint) = AF_INET;
        return 0;
    } else {
#if IS_USED(MODULE_DNS)
        /* Only thing left to try is DNS resolution */
        /* FIXME: DNS query function requires null-terminated string, 
                    forcing us to copy the URI host */
        if (parsed->host_len > CONFIG_UNICOAP_URI_HOST_PART_LENGTH_MAX) {
            _URI_DEBUG("no buffer space to copy '%.*s', " _UNICOAP_NEED_HAVE "\n",
                (int)parsed->host_len, parsed->host,
                parsed->host_len, CONFIG_UNICOAP_URI_HOST_PART_LENGTH_MAX);
        }
        char* domain = alloca(CONFIG_UNICOAP_URI_HOST_PART_LENGTH_MAX + 1);
        memcpy(domain, parsed->host, parsed->host_len);
        domain[parsed->host_len] = '\0';

        if ((res = dns_query(domain, &tl_ep->addr, AF_UNSPEC)) < 0) {
            _URI_DEBUG("DNS resolution of '%.*s' failed: %i\n",
                        (int)parsed->host_len, parsed->host, res);
            return res;
        }
        return _UNICOAP_URI_RESOLVED_HOST;
#else
        unicoap_assist(API_ERROR("DNS support missing") FIXIT("USEMODULE += dns"));
        return -ENOTSUP;
#endif
    }
    /* MARK: unicoap_driver_extension_point */
}

int unicoap_uri_populate(
    uri_parser_result_t* parsed, 
    unicoap_endpoint_t* endpoint,
    unicoap_options_t* options
) {
    assert(parsed);
    assert(endpoint);
    assert(options);
    int res = 0;
    int proto = unicoap_proto_from_scheme_and_host(
        parsed->scheme, parsed->scheme_len, parsed->host, parsed->host_len);

    if (proto < 0) {
        _URI_DEBUG("error: invalid protocol scheme: '%.*s'\n", (int)parsed->scheme_len,
                  parsed->scheme);
        return -EPROTO;
    }
    endpoint->proto = proto;

    if (parsed->port == 0 && unicoap_transport_uses_sock_tl_ep(proto)) {
        /* use default CoAP port for coap://, coap+tcp://, coap+ws:// */
        /* use default CoAPS port for coaps://, coaps+tcp://, coaps+ws:// */
        if (parsed->scheme_len >= STRLEN(UNICOAP_SCHEME_DTLS) &&
            strncmp(parsed->scheme, UNICOAP_SCHEME_DTLS, STRLEN(UNICOAP_SCHEME_DTLS)) == 0) {
            parsed->port = UNICOAP_DEFAULT_COAPS_PORT;
        }
        else if (parsed->scheme_len >= STRLEN(UNICOAP_SCHEME_UDP) &&
                 strncmp(parsed->scheme, UNICOAP_SCHEME_UDP, STRLEN(UNICOAP_SCHEME_UDP)) == 0) {
            parsed->port = UNICOAP_DEFAULT_COAP_PORT;
        }
        else {
            _URI_DEBUG("error: cannot infer default port from scheme '%.*s', no port provided\n",
                       (int)parsed->scheme_len, parsed->scheme);
            assert(false);
            return -EINVAL;
        }
    }
    /* MARK: unicoap_driver_extension_point */
    /* CoAP over GATT/BLE domain checks would happen here. */

    if ((res = _populate_address(parsed, endpoint)) < 0) {
        _URI_DEBUG("endpoint address resolution from URI failed: %i (%s)\n", res, strerror(-res));
        return res;
    }

    /* MARK: unicoap_driver_extension_point */
    /* Other transport may also use host names, hence, other conditions may be added below. */
    /* E.g., CoAP over BLE/GATT uses .ble.arpa and in general .alt may represent devices etc. */
    if (res == _UNICOAP_URI_RESOLVED_HOST) {
        if ((res = unicoap_options_set_uri_host(options, parsed->host, parsed->host_len)) < 0) {
            return res;
        }
    }

    if (parsed->path && (parsed->path_len > 0)) {
        if ((res = 
            unicoap_options_add_uri_path(options, (char*)parsed->path, parsed->path_len)) < 0) {
            return res;
        }
    }
    if (parsed->query && (parsed->query_len > 0)) {
        if ((res = unicoap_options_add_uri_queries(options, parsed->query, parsed->query_len)) < 0) {
            return res;
        }
    }
    return 0;
}
