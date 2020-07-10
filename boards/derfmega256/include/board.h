/*
 * Copyright (C) 2019 Alexander Chudov <chudov@gmail.com>
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
 * @brief       Board specific definitions for the deRFmega256 modules
 *
 * @author      Alexander Chudov <chudov@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#include "periph/eeprom.h"
#include "net/eui_provider.h"

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
static inline int _eeprom_mac_get_eui64(const void *arg, eui64_t *addr)
{
    (void) arg;

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

/**
 * @name xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (CLOCK_CORECLOCK / 64)
#define XTIMER_BACKOFF      (40)

/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
