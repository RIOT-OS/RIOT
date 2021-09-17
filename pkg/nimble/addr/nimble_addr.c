/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_addr
 * @{
 *
 * @file
 * @brief       NimBLE specific BLE address helper functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "nimble_addr.h"
#include "net/bluetil/addr.h"

void nimble_addr_print(const ble_addr_t *addr)
{
    char tmp[NIMBLE_ADDR_STRLEN];
    nimble_addr_sprint(tmp, addr);
    printf("%s", tmp);
}

void nimble_addr_sprint(char *buf, const ble_addr_t *addr)
{
    uint8_t addrn[BLE_ADDR_LEN];
    bluetil_addr_swapped_cp(addr->val, addrn);
    bluetil_addr_sprint(buf, addrn);
    const char *type;
    switch (addr->type) {
        case BLE_ADDR_PUBLIC:       type = " (public) "; break;
        case BLE_ADDR_RANDOM:       type = " (random) "; break;
        case BLE_ADDR_PUBLIC_ID:    type = " (id_pub) "; break;
        case BLE_ADDR_RANDOM_ID:    type = " (id_rand)"; break;
        default:                    type = " (unknown)"; break;
    }
    memcpy(&buf[BLUETIL_ADDR_STRLEN - 1], type, 10);
    buf[NIMBLE_ADDR_STRLEN - 1] = '\0';
}
