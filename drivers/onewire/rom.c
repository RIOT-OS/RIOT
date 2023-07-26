/*
 * Copyright (C) 2016-2020 Freie Universität Berlin
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_onewire
 * @{
 *
 * @file
 * @brief       1-Wire address (ROM) string helper functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "fmt.h"
#include "onewire.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static_assert(sizeof(onewire_rom_t) == ONEWIRE_ROM_LEN);

int onewire_rom_from_str(onewire_rom_t *rom, const char *str)
{
    assert(rom);
    assert(str);

    if (strlen(str) != (ONEWIRE_ROM_STR_LEN - 1)) {
        return -EINVAL;
    }
    fmt_hex_bytes(rom->u8, str);

    return 0;
}

void onewire_rom_to_str(char *str, const onewire_rom_t *rom)
{
    assert(rom);
    assert(str);

    size_t pos = fmt_bytes_hex(str, rom->u8, sizeof(rom->u8));
    str[pos] = '\0';
}

void onewire_rom_print(const onewire_rom_t *rom)
{
    assert(rom);

    char str[ONEWIRE_ROM_STR_LEN];
    onewire_rom_to_str(str, rom);
    print(str, (ONEWIRE_ROM_STR_LEN - 1));
}

bool onewire_rom_is_valid(const onewire_rom_t *rom)
{
    assert(rom);

    /* Some TI tmp1827 parts have broken ROM ID's. Their CRC was calculated as
       if their family code was 0x26 rather than the 0x27 actually used. Account
       for this, allowing those part IDs to pass.

       see here for more info:
       https://e2e.ti.com/support/sensors-group/sensors/f/sensors-forum/1342717/tmp1827-64-bit-uid-crc8-not-correct
    */
    if (onewire_rom_family_code(rom) == 0x27) {
        const uint_fast8_t crc =
            onewire_crc8(0xfe, &rom->u8[1], sizeof(rom->u8) - 1);
        if (crc == 0) {
            DEBUG("%s: crc result: %x\n", DEBUG_FUNC, crc);
            return true;
        }
    }

    const uint_fast8_t crc = onewire_crc8(0, rom->u8, sizeof(rom->u8));
    DEBUG("%s: crc result: %x\n", DEBUG_FUNC, crc);
    return crc == 0;
}

uint_fast8_t onewire_rom_family_code(const onewire_rom_t *rom)
{
    assert(rom);

    return rom->u8[0];
}

int onewire_rom_compare(const onewire_rom_t *left, const onewire_rom_t *right)
{
    assert(right);
    assert(left);

    return memcmp(left->u8, right->u8, sizeof(left->u8));
}
