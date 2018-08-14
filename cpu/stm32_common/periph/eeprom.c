/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_eeprom
 * @{
 *
 * @file
 * @brief       Low-level eeprom driver implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#include "periph/eeprom.h"

extern void _lock(void);
extern void _unlock(void);

#ifndef EEPROM_START_ADDR
#error "periph/eeprom: EEPROM_START_ADDR is not defined"
#endif

uint8_t eeprom_read_byte(uint32_t pos)
{
    assert(pos < EEPROM_SIZE);

    DEBUG("Reading data from EEPROM at pos %" PRIu32 "\n", pos);
    return *(uint8_t *)(EEPROM_START_ADDR + pos);
}

void eeprom_write_byte(uint32_t pos, uint8_t data)
{
    assert(pos < EEPROM_SIZE);

    DEBUG("Writing data '%c' to EEPROM at pos %" PRIu32 "\n", data, pos);
    _unlock();
    *(uint8_t *)(EEPROM_START_ADDR + pos) = data;
    _lock();
}
