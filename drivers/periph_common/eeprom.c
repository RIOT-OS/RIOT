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

/* guard this file, must be done before including periph/eeprom.h */
#if defined(EEPROM_SIZE)

#include "periph/eeprom.h"

size_t eeprom_read(uint32_t pos, uint8_t *data, size_t len)
{
    assert(pos + len < EEPROM_SIZE);

    for (size_t i = 0; i < len; i++) {
        data[i] = eeprom_read_byte(pos++);
    }

    return len;
}

size_t eeprom_write(uint32_t pos, const uint8_t *data, size_t len)
{
    assert(pos + len < EEPROM_SIZE);

    for (size_t i = 0; i < len; i++) {
        eeprom_write_byte(pos++, data[i]);
    }

    return len;
}

#endif
