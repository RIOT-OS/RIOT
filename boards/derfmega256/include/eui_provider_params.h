/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_deRFmega256
 * @{
 *
 * @file
 * @brief       EUI providers found on the board
 *
 * @author      Alexander Chudov <chudov@gmail.com>
 */
#ifndef EUI_PROVIDER_PARAMS_H
#define EUI_PROVIDER_PARAMS_H

#include "net/eui64.h"
#include "periph/eeprom.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    MAC configuration
 *          Offset of the MAC address in the EEPROM
 */
#define EEPROM_MAC_ADDR (0x1fe4)

/**
 * @brief    Constant in EEPROM provides a EUI-64, this is also printed on the board
 */
static inline int _eeprom_mac_get_eui64(uint8_t index, eui64_t *addr)
{
    (void)index;

    if (eeprom_read(EEPROM_MAC_ADDR, addr, sizeof(eui64_t)) != sizeof(eui64_t)) {
        return -1;
    }

    addr->uint64.u64 = byteorder_htonll(addr->uint64.u64).u64;

    return 0;
}

/**
 * @name    EUI-64 sources on the board
 * @{
 */
#define EUI64_PROVIDER_FUNC   _eeprom_mac_get_eui64
#define EUI64_PROVIDER_TYPE   NETDEV_AT86RF2XX
#define EUI64_PROVIDER_INDEX  0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* EUI_PROVIDER_PARAMS_H */
/** @} */
