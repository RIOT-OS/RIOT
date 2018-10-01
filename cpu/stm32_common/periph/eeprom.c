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
#include "periph/eeprom.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

extern void _lock(void);
extern void _unlock(void);

#ifndef EEPROM_START_ADDR
#error "periph/eeprom: EEPROM_START_ADDR is not defined"
#endif

size_t eeprom_read(uint32_t pos, uint8_t *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = data;

    DEBUG("Reading data from EEPROM at pos %" PRIu32 ": ", pos);
    for (size_t i = 0; i < len; i++) {
        *p++ = *(uint8_t *)(EEPROM_START_ADDR + pos++);
        DEBUG("0x%02X ", *p);
    }
    DEBUG("\n");

    return len;
}

size_t eeprom_write(uint32_t pos, const uint8_t *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = (uint8_t *)data;

    _unlock();

    for (size_t i = 0; i < len; i++) {
        *(uint8_t *)(EEPROM_START_ADDR + pos++) = *p++;
    }

    _lock();

    return len;
}
