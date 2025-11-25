/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanosock
 * @brief       NanoCoAP Link Format helper functions
 *
 * @{
 *
 * @file
 * @brief       NanoCoAP Link Format ([RFC 6690](https://www.rfc-editor.org/rfc/rfc6690.html))
 *              helper functions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef NET_NANOCOAP_LINK_FORMAT_H
#define NET_NANOCOAP_LINK_FORMAT_H

#include "net/nanocoap_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Callback function called for each resource on the directory
 *
 * @param[in]   entry   Resource entry from the server
 * @param[in]   ctx     Optional function context
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
typedef int (*coap_link_format_handler_t)(char *entry, void *ctx);

/**
 * @brief   Downloads the resource behind @p path via blockwise GET
 *
 * @param[in]   sock    Connection to the server
 * @param[in]   path    path of the resource
 * @param[in]   cb      Callback to execute for each resource entry
 * @param[in]   arg     Optional callback argument
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_link_format_get(nanocoap_sock_t *sock, const char *path,
                             coap_link_format_handler_t cb, void *arg);

/**
 * @brief   Downloads the resource behind @p url via blockwise GET
 *
 * @param[in]   url     URL to the resource
 * @param[in]   cb      Callback to execute for each resource entry
 * @param[in]   arg     Optional callback argument
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_link_format_get_url(const char *url,
                                 coap_link_format_handler_t cb, void *arg);

#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_LINK_FORMAT_H */
/** @} */
