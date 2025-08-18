/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_avr-rss2
 * @{
 *
 * @file
 * @brief       EUI providers found on the board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#include "at24mac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    AT24Mac provides a EUI-64, this is also printed on the board
 */
static inline int _at24mac_get_eui64(uint8_t index, eui64_t *addr)
{
    return at24mac_get_eui64(index, addr);
}

/**
 * @name    EUI-64 sources on the board
 *          AT24Mac is present on the board
 * @{
 */
#define EUI64_PROVIDER_FUNC   _at24mac_get_eui64
#define EUI64_PROVIDER_TYPE   NETDEV_AT86RF2XX
#define EUI64_PROVIDER_INDEX  0
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
