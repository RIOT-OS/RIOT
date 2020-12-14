/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       EUI providers found on the board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef EUI_PROVIDER_PARAMS_H
#define EUI_PROVIDER_PARAMS_H

#include "edbg_eui.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    EDBG provides a EUI-64, the same that is printed on the board
 */
static inline int _edbg_get_eui64(uint8_t index, eui64_t *addr)
{
    (void)index;

    /* EDBG can take a while to respond on cold boot */
    unsigned tries = 10000;
    while (--tries && edbg_get_eui64(addr)) {}
    return tries ? 0 : -1;
}

/**
 * @name    EUI sources on the board
 *          EUI-64 inside EDBG for the internal radio
 * @{
 */
#define EUI64_PROVIDER_FUNC   _edbg_get_eui64
#define EUI64_PROVIDER_TYPE   NETDEV_AT86RF2XX
#define EUI64_PROVIDER_INDEX  0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* EUI_PROVIDER_PARAMS_H */
/** @} */
