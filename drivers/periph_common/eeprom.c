/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       Common eeprom functions implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "periph_cpu.h"
#include "assert.h"

#include "periph/eeprom.h"

uint8_t eeprom_read_byte(uint32_t pos)
{
    uint8_t byte;
    eeprom_read(pos, &byte, 1);
    return byte;
}

void eeprom_write_byte(uint32_t pos, uint8_t byte)
{
    eeprom_write(pos, &byte, 1);
}

size_t eeprom_set(uint32_t pos, uint8_t val, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    for (size_t i = 0; i < len; i++) {
        eeprom_write_byte(pos++, val);
    }

    return len;
}

size_t eeprom_clear(uint32_t pos, size_t len)
{
    return eeprom_set(pos, EEPROM_CLEAR_BYTE, len);
}

size_t eeprom_erase(void)
{
    return eeprom_clear(0, EEPROM_SIZE);
}
