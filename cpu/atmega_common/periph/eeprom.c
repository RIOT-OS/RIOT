/*
 * Copyright (C) 2018 Inria
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_eeprom
 * @{
 *
 * @file
 * @brief       Low-level EEPROM driver implementation for ATmega family
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 * @}
 */

#include <stdint.h>
#include <assert.h>

#include "cpu.h"
#include "periph/eeprom.h"

#define ENABLE_DEBUG 0
#include "debug.h"

size_t eeprom_read(uint32_t pos, void *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = data;

    DEBUG("Reading data from EEPROM at pos %" PRIu32 ": ", pos);
    for (size_t i = 0; i < len; i++) {
#ifdef EEPE
        while (EECR & (1 << EEPE)) {}
#elif defined(EEWE)
        while (EECR & (1 << EEWE)) {}
#endif

        /* Set up address register */
        EEAR = pos++;

        /* Start eeprom read by writing EERE */
        EECR |= (1 << EERE);
        *p++ = (uint8_t)EEDR;
        DEBUG("0x%02X ", EEDR);
    }
    DEBUG("\n");

    return len;
}

size_t eeprom_write(uint32_t pos, const void *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = (uint8_t *)data;

    for (size_t i = 0; i < len; i++) {
        /* Wait for completion of previous operation */
#ifdef EEPE
        while (EECR & (1 << EEPE)) {}
#elif defined(EEWE)
        while (EECR & (1 << EEWE)) {}
#endif

        /* Set up address and Data Registers */
        EEAR = pos++;
        EEDR = *p++;

#ifdef EEMPE
        /* Write logical one to EEMPE */
        EECR |= (1 << EEMPE);
#elif defined(EEMWE)
        /* Write logical one to EEMWE */
        EECR |= (1 << EEMWE);
#endif
#ifdef EEPE
        /* Start eeprom write by setting EEPE */
        EECR |= (1 << EEPE);
#elif defined(EEWE)
        /* Start eeprom write by setting EEWE */
        EECR |= (1 << EEWE);
#endif
    }

    return len;
}
