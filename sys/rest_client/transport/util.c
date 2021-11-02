/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @defgroup    sys_rest_client_util REST client CoAP transport
 * @ingroup     sys_rest_client
 * @brief       REST client utils
 *
 * @{
 * @file
 * @brief       REST client utils
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include "kernel_defines.h"
#include "assert.h"

#include "rest_client/util.h"

static const char *_scheme_strings[] = {
    "coap",
    "coaps",
    "http",
    "https",
    "mqtt",
    "mqtts",
    "mqttsn",
    "mqttsns"
};

static const char *_method_strings[] = {
    "GET",
    "POST",
    "PUT",
    "PATCH",
    "DELETE"
};

const char* rest_client_util_scheme_to_string(rest_client_scheme_t scheme)
{
    switch (scheme) {
        case REST_CLIENT_SCHEME_COAP:
            return _scheme_strings[0];
        case REST_CLIENT_SCHEME_COAP_SECURE:
            return _scheme_strings[1];
        case REST_CLIENT_SCHEME_HTTP:
            return _scheme_strings[2];
        case REST_CLIENT_SCHEME_HTTP_SECURE:
            return _scheme_strings[3];
        case REST_CLIENT_SCHEME_MQTT:
            return _scheme_strings[4];
        case REST_CLIENT_SCHEME_MQTT_SECURE:
            return _scheme_strings[5];
        case REST_CLIENT_SCHEME_MQTTSN:
            return _scheme_strings[6];
        case REST_CLIENT_SCHEME_MQTTSN_SECURE:
            return _scheme_strings[7];
        default:
            assert(!"The default case of switch was reached.");
            return NULL;
    }
}

const char* rest_client_util_method_to_string(rest_client_method_t method)
{
    switch (method) {
        case REST_CLIENT_METHOD_GET:
            return _method_strings[0];
        case REST_CLIENT_METHOD_POST:
            return _method_strings[1];
        case REST_CLIENT_METHOD_PUT:
            return _method_strings[2];
        case REST_CLIENT_METHOD_PATCH:
            return _method_strings[3];
        case REST_CLIENT_METHOD_DELETE:
            return _method_strings[4];
        default:
            assert(!"The default case of switch was reached.");
            return NULL;
    }
}
