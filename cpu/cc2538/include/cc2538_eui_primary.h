/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_cc2538
 * @{
 *
 * @file
 * @brief           CC2538 EUI-64 provider
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC2538_EUI64_LOCATION_PRI   (0x00280028) /**< Primary EUI-64 address location */
#define CC2538_EUI64_LOCATION_SEC   (0x0027FFCC) /**< Secondary EUI-64 address location */

/**
 * @brief   Get the primary (burned-in) EUI-64 of the device
 *
 * @param index     unused
 * @param[out] addr The EUI-64
 *
 * @return  0
 */
static inline int cc2538_get_eui64_primary(uint8_t index, eui64_t *addr)
{
    (void)index;

    /*
     * The primary EUI-64 seems to be written to memory in a non-sequential
     * byte order, with both 4-byte halves of the address flipped.
     */
    addr->uint8[7] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[4];
    addr->uint8[6] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[5];
    addr->uint8[5] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[6];
    addr->uint8[4] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[7];
    addr->uint8[3] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[0];
    addr->uint8[2] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[1];
    addr->uint8[1] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[2];
    addr->uint8[0] = ((uint8_t*)CC2538_EUI64_LOCATION_PRI)[3];

    return 0;
}

/**
 * @name    CC2538 comes with an EUI-64
 * @{
 */
#ifndef CC2538_EUI64_CUSTOM
#define EUI64_PROVIDER_FUNC   cc2538_get_eui64_primary
#define EUI64_PROVIDER_TYPE   NETDEV_CC2538
#define EUI64_PROVIDER_INDEX  0
#endif  /* !CC2538_EUI64_CUSTOM */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
