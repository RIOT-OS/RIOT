/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_eui_provider
 *
 * @file
 * @brief       EUI-64 address provider default values
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef EUI64_PROVIDER_PARAMS_H
#define EUI64_PROVIDER_PARAMS_H

#if __has_include("eui_provider_params.h")
#include "eui_provider_params.h"
#endif
#include "net/eui_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A `netdev_get_eui64_cb_t` function
 */
#ifndef EUI64_PROVIDER_FUNC
#define EUI64_PROVIDER_FUNC   NULL

/**
 * @brief Number of EUI-64 providers, determined automatically
 */
#ifndef EUI64_PROVIDER_PARAMS
#define EUI64_PROVIDER_NUMOF 0U
#endif
#endif

/**
 * @brief Driver type to match with EUI-64 provider
 */
#ifndef EUI64_PROVIDER_TYPE
#define EUI64_PROVIDER_TYPE   NETDEV_ANY
#endif

/**
 * @brief If multiple instances of a device exist, which one should
 *        be assigned the EUI-64
 */
#ifndef EUI64_PROVIDER_INDEX
#define EUI64_PROVIDER_INDEX  NETDEV_INDEX_ANY
#endif

/**
 * @brief Array of available EUI-64 providers
 */
#ifndef EUI64_PROVIDER_PARAMS
#define EUI64_PROVIDER_PARAMS     {                                \
                                 .provider = EUI64_PROVIDER_FUNC,  \
                                 .type     = EUI64_PROVIDER_TYPE,  \
                                 .index    = EUI64_PROVIDER_INDEX, \
                                },
#endif

/**
 * @name    EUI-64 sources on the board
 *
 * @{
 */
static const eui64_conf_t eui64_conf[] = {
    EUI64_PROVIDER_PARAMS
};

#ifndef EUI64_PROVIDER_NUMOF
#define EUI64_PROVIDER_NUMOF    ARRAY_SIZE(eui64_conf)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* EUI64_PROVIDER_PARAMS_H */
