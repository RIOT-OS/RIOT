/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_native
 * @{
 *
 * @file
 * @brief       EUI providers found on the board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef EUI_PROVIDER_PARAMS_H
#define EUI_PROVIDER_PARAMS_H

#include "native_cli_eui_provider.h"
#include "netdev_tap.h"
#include "netdev_tap_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    EUI sources on the board
 *          EUI-64 can be provided with the -Z command line argument
 * @{
 */
#define EUI64_PROVIDER_FUNC   native_cli_get_eui64
#ifndef EUI64_PROVIDER_TYPE
#define EUI64_PROVIDER_TYPE   NETDEV_SOCKET_ZEP
#endif
#define EUI64_PROVIDER_INDEX  NETDEV_INDEX_ANY
/** @} */

/**
 * @brief   EUI is derived from host interface
 */
static inline int _get_tap_eui48(uint8_t index, eui48_t *addr)
{
#ifndef MODULE_NETDEV_TAP
    (void)index;
    (void)addr;
    return -1;
#else
    extern netdev_tap_t netdev_taps[NETDEV_TAP_MAX];
    if (index >= NETDEV_TAP_MAX) {
        return -1;
    }

    netdev_tap_get_host_eui48(&netdev_taps[index], addr);

    /* change mac addr so it differs from what the host is using */
    addr->uint8[5]++;

    return 0;
#endif
}

#ifndef EUI48_PROVIDER_FUNC
#define EUI48_PROVIDER_FUNC   _get_tap_eui48
#endif
#ifndef EUI48_PROVIDER_TYPE
#define EUI48_PROVIDER_TYPE   NETDEV_TAP
#endif

#ifdef __cplusplus
}
#endif

#endif /* EUI_PROVIDER_PARAMS_H */
/** @} */
