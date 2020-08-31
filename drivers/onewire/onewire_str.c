/*
 * Copyright (C) 2016-2020 Freie Universität Berlin
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
 *
 * @}
 */

#include <string.h>

#include "fmt.h"
#include "assert.h"
#include "onewire.h"

static int _hex2int(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }
    else {
        return -1;
    }
}

int onewire_rom_from_str(onewire_rom_t *rom, const char *str)
{
    assert(rom);
    assert(str);

    if (strlen(str) != (ONEWIRE_ROM_STR_LEN - 1)) {
        return ONEWIRE_ERR_ROMSTR;
    }

    memset(rom->u8, 0, sizeof(onewire_rom_t));
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 2; j++) {
            int digit = _hex2int(str[(i * 2) + j]);
            if (digit < 0) {
                return ONEWIRE_ERR_ROMSTR;
            }
            rom->u8[i] |= (digit << ((1 - j) * 4));
        }
    }

    return ONEWIRE_OK;
}

void onewire_rom_to_str(char *str, const onewire_rom_t *rom)
{
    assert(rom);
    assert(str);

    size_t pos = fmt_bytes_hex(str, rom->u8, sizeof(onewire_rom_t));
    str[pos] = '\0';
}

void onewire_rom_print(const onewire_rom_t *rom)
{
    assert(rom);

    char str[ONEWIRE_ROM_STR_LEN];
    onewire_rom_to_str(str, rom);
    print(str, (ONEWIRE_ROM_STR_LEN - 1));
}
