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
 * @brief       EUI-48 address provider default values
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef EUI48_PROVIDER_PARAMS_H
#define EUI48_PROVIDER_PARAMS_H

#if __has_include("eui_provider_params.h")
#include "eui_provider_params.h"
#endif
#include "net/eui_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A `netdev_get_eui48_cb_t` function
 */
#ifndef EUI48_PROVIDER_FUNC
#define EUI48_PROVIDER_FUNC   NULL

/**
 * @brief Number of EUI-48 providers, determined automatically
 */
#ifndef EUI48_PROVIDER_PARAMS
#define EUI48_PROVIDER_NUMOF 0U
#endif
#endif

/**
 * @brief Driver type to match with EUI-48 provider
 */
#ifndef EUI48_PROVIDER_TYPE
#define EUI48_PROVIDER_TYPE   NETDEV_ANY
#endif

/**
 * @brief If multiple instances of a device exist, which one should
 *        be assigned the EUI-48
 */
#ifndef EUI48_PROVIDER_INDEX
#define EUI48_PROVIDER_INDEX  NETDEV_INDEX_ANY
#endif

/**
 * @brief Array of available EUI-48 providers
 */
#ifndef EUI48_PROVIDER_PARAMS
#define EUI48_PROVIDER_PARAMS     {                                \
                                 .provider = EUI48_PROVIDER_FUNC,  \
                                 .type     = EUI48_PROVIDER_TYPE,  \
                                 .index    = EUI48_PROVIDER_INDEX, \
                                },
#endif

/**
 * @name    EUI-48 sources on the board
 *
 * @{
 */
static const eui48_conf_t eui48_conf[] = {
    EUI48_PROVIDER_PARAMS
};

#ifndef EUI48_PROVIDER_NUMOF
#define EUI48_PROVIDER_NUMOF    ARRAY_SIZE(eui48_conf)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* EUI48_PROVIDER_PARAMS_H */
