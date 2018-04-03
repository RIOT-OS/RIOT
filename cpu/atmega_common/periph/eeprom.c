/*
 * Copyright (C) 2018 Inria
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
 * @}
 */

#include <stdint.h>
#include <assert.h>

#include "cpu.h"
#include "periph/eeprom.h"

uint8_t eeprom_read_byte(uint32_t pos)
{
    assert(pos < EEPROM_SIZE);

    /* Wait for completion of previous operation */
    while (EECR & (1 << EEPE)) {}

    /* Set up address register */
    EEAR = pos;

    /* Start eeprom read by writing EERE */
    EECR |= (1 << EERE);

    /* Return data from Data Register */
    return EEDR;
}

void eeprom_write_byte(uint32_t pos, uint8_t data)
{
    assert(pos < EEPROM_SIZE);

    /* Wait for completion of previous operation */
    while (EECR & (1 << EEPE)) {}

    /* Set up address and Data Registers */
    EEAR = pos;
    EEDR = data;

    /* Write logical one to EEMPE */
    EECR |= (1 << EEMPE);

    /* Start eeprom write by setting EEPE */
    EECR |= (1 << EEPE);
}
