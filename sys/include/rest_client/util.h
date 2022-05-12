/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_util
 * @{
 *
 * @file
 * @brief       REST client utility
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#ifndef REST_CLIENT_UTIL_H
#define REST_CLIENT_UTIL_H

#include "kernel_defines.h"

#include "rest_client.h"

const char* rest_client_util_scheme_to_string(rest_client_scheme_t scheme);

const char* rest_client_util_method_to_string(rest_client_method_t method);

#endif /* REST_CLIENT_UTIL_H */
