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

int onewire_rom_from_str(onewire_rom_t *rom, const char *str)
{
    assert(rom);
    assert(str);

    if (strlen(str) != (ONEWIRE_ROM_STR_LEN - 1)) {
        return ONEWIRE_ERR_ROMSTR;
    }
    fmt_hex_bytes(rom->u8, str);

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
