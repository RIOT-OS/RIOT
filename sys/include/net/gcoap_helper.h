/*
 * Copyright (c) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gcoap_helper GCoAP helper functions
 * @ingroup     net_gcoap
 * @{
 *
 * @file
 * @brief       Helper functions for GCoAP
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef NET_GCOAP_HELPER_H
#define NET_GCOAP_HELPER_H

#include "net/gcoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Issue a blocking CoAP GET request
 *
 * @param[in]     host          The hostname or IP string of the CoAP server
 * @param[in]     path          The CoAP request path
 * @param[in]     confirmable   Whether the request should be confirmable
 * @param[out]    response      Buffer for the response payload
 * @param[in,out] response_size Size of the response buffer. Will contain the
 *                              actual length of the response when finished.
 *
 * @return        CoAP response code or negative error
 */
int coap_get(const char *host, const char *path, bool confirmable,
             void *response, size_t *response_size);

/**
 * @brief   Issue a blocking CoAP POST request
 *
 * @param[in]     host          The hostname or IP string of the CoAP server
 * @param[in]     path          The CoAP request path
 * @param[in]     confirmable   Whether the request should be confirmable
 * @param[in]     payload       Payload for the POST request, may be NULL
 * @param[in]     payload_size  Size of the payload buffer
 * @param[out]    response      Buffer for the response payload
 * @param[in,out] response_size Size of the response buffer. Will contain the
 *                              actual length of the response when finished.
 *
 * @return        CoAP response code or negative error
 */
int coap_post(const char *host, const char *path, bool confirmable,
              const void *payload, size_t payload_size,
              void *response, size_t *response_size);

#ifdef __cplusplus
}
#endif

#endif /* NET_GCOAP_HELPER_H */
/** @} */
