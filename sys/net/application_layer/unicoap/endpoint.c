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

int unicoap_uri_populate(uri_parser_result_t* parsed, unicoap_endpoint_t* endpoint,
                         unicoap_options_t* options) {
    assert(parsed);
    assert(endpoint);
    assert(options);
    int res = 0;
    int proto = unicoap_proto_from_scheme_and_host(parsed->scheme, parsed->scheme_len, parsed->host, parsed->host_len);
    if (proto < 0) {
        _URI_DEBUG("error: invalid protocol scheme: '%.*s'\n", (int)parsed->scheme_len,
                  parsed->scheme);
        return -EPROTO;
    }

    /* MARK: unicoap_driver_extension_point */
    /* CoAP over GATT/BLE domain checks would happen here. */

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
            _URI_DEBUG("error: cannot infer default port from scheme '%.*s', "
                      "no port provided\n",
                      (int)parsed->scheme_len, parsed->scheme);
            assert(false);
            return -EINVAL;
        }
    }

#if IS_USED(MODULE_UNICOAP_SOCK_SUPPORT)
    int16_t netif_id = SOCK_ADDR_ANY_NETIF;
    if (parsed->zoneid && parsed->zoneid_len > 0) {
        netif_t *netif = netif_get_by_name_buffer(parsed->zoneid, parsed->zoneid_len);
        if (!netif || (netif_id = netif_get_id(netif)) < 0) {
            _URI_DEBUG("error: unknown zone ID\n");
            return -EINVAL;
        }
    }

    if (unicoap_transport_uses_sock_tl_ep(proto)) {
        struct _sock_tl_ep *tl_ep = _unicoap_endpoint_get_tl(endpoint);
        tl_ep->port = parsed->port;
        tl_ep->netif = netif_id;

        if (parsed->ipv6addr && (parsed->ipv6addr_len > 0)) {
#  if SOCK_HAS_IPV6 && IS_USED(MODULE_IPV6_ADDR)
            ipv6_addr_t *v6 = ipv6_addr_from_buf((ipv6_addr_t *)tl_ep->addr.ipv6, parsed->ipv6addr,
                                                 parsed->ipv6addr_len);
            assert(v6);
            (void)v6;
            tl_ep->family = AF_INET6;
            static_assert(sizeof(tl_ep->addr.ipv6) == sizeof(v6->u8),
                          "Programmer error: IPv6 size mismatch");

#  else /* if SOCK_HAS_IPV6 && IS_USED(MODULE_IPV6_ADDR) */
            _URI_DEBUG("cannot read IPv6 literal, SOCK_HAS_IPV6 ipv6_addr)\n");
            return -EINVAL;

#  endif /* if SOCK_HAS_IPV6 && IS_USED(MODULE_IPV6_ADDR) */
        }
        else {
            if (parsed->host && (parsed->host_len > 0)) {

#  if SOCK_HAS_IPV4 && IS_USED(MODULE_IPV4_ADDR)
                /* _sock_tl_ep always has v4 support */
                ipv4_addr_t *v4 = NULL;
                _URI_DEBUG("trying to read IPv4 literal (ipv4_addr module used)\n");
                if ((v4 = ipv4_addr_from_buf((ipv4_addr_t *)tl_ep->addr.ipv4, parsed->host,
                                             parsed->host_len))) {
                    tl_ep->family = AF_INET;
                    static_assert(sizeof(tl_ep->addr.ipv4) == sizeof(v4->u8),
                                  "Programmer error: IPv5 size mismatch");
                }

#  else  /* SOCK_HAS_IPV4 && IS_USED(MODULE_IPV4_ADDR) */
                if (false) {
                }
#  endif /* SOCK_HAS_IPV4 && IS_USED(MODULE_IPV4_ADDR) */
                else {
#  if IS_USED(MODULE_DNS)
                    /* FIXME: DNS query function requires null-terminated string, forcing us to copy the URI host */

                    char domain[parsed->host_len + 1];
                    memcpy(domain, parsed->host, parsed->host_len);
                    domain[parsed->host_len] = '\0';

                    if ((res = dns_query(domain, &tl_ep->addr, AF_UNSPEC)) < 0) {
                        _URI_DEBUG("error: DNS resolution of '%.*s' failed: %i\n",
                                  (int)parsed->host_len, parsed->host, res);
                        return res;
                    }

                    if (options &&
                        (res = unicoap_options_set_string(options, UNICOAP_OPTION_URI_HOST,
                                                          parsed->host, parsed->host_len)) < 0) {
                        return res;
                    }
#  else  /* IS_USED(MODULE_DNS) */
                    _URI_DEBUG("error: cannot resolve FQDN, dns module missing\n");
                    assert(false);
                    return -ENOTSUP;
#  endif /* IS_USED(MODULE_DNS) */
                }
            }
            else {
                _URI_DEBUG("error: malformed URI\n");
                return -EINVAL;
            }
        }
    }
    else {
        _URI_DEBUG("error: unsupported endpoint\n");
        return -ENOTSUP;
    }
#endif /* IS_USED(MODULE_UNICOAP_SOCK_SUPPORT) */

    /* MARK: unicoap_driver_extension_point */

    endpoint->proto = proto;

    if (options) {
        if (parsed->path && (parsed->path_len > 0)) {
            if ((res = 
                unicoap_options_add_uri_path(options, (char*)parsed->path, parsed->path_len)) < 0) {
                _URI_DEBUG("error: setting Uri-Path failed\n");
                return res;
            }
        }

        if (parsed->query && (parsed->query_len > 0)) {
            if ((res = unicoap_options_add_values_joined(options, UNICOAP_OPTION_URI_QUERY,
                                                  (uint8_t *)parsed->query, parsed->query_len,
                                                  '&')) < 0) {
                _URI_DEBUG("error: setting Uri-Query failed\n");
                return res;
            }
        }
    }
    return 0;
}

int unicoap_uri_parse(const char *uri, size_t uri_length, unicoap_endpoint_t *endpoint,
                      unicoap_options_t *options) {
    assert(uri_parser_is_absolute(uri, uri_length));

    uri_parser_result_t parsed = { 0 };
    int res = 0;
    if ((res = uri_parser_process(&parsed, uri, uri_length)) < 0) {
        return res;
    }

    if (!parsed.scheme) {
        _URI_DEBUG("error: missing scheme\n");
        return -1;
    }

    return unicoap_uri_populate(&parsed, endpoint, options);
}
