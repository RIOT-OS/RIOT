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

#endif
