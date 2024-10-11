/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_same54-xpro
 * @{
 *
 * @file
 * @brief       EUI providers found on the board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef EUI_PROVIDER_PARAMS_H
#define EUI_PROVIDER_PARAMS_H

#include "at24mac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    AT24Mac provides a EUI-48
 */
static inline int _at24mac_get_eui48(uint8_t index, eui48_t *addr)
{
    return at24mac_get_eui48(index, addr);
}

/**
 * @name    EUI-48 sources on the board
 *          AT24Mac is present on the board
 * @{
 */
#define EUI48_PROVIDER_FUNC   _at24mac_get_eui48
#ifndef EUI48_PROVIDER_TYPE
#define EUI48_PROVIDER_TYPE   NETDEV_SAM0_ETH
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* EUI_PROVIDER_PARAMS_H */
