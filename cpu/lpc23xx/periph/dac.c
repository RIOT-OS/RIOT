/*
 * Copyright (C) 2019 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc23xx
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/dac.h"

int8_t dac_init(dac_t line)
{
    (void) line;

    /* P0.26 is the only valid DAC pin */
    PINSEL1 |=  BIT21;
    PINSEL1 &= ~BIT20;

    return 0;
}

void dac_set(dac_t line, uint16_t value)
{
    (void) line;

    /* Bits 5:0 are reserved for future, higher-resolution D/A converters. */
    DACR = value & 0xFFE0;
}

void dac_poweron(dac_t line)
{
    /* The DAC is always on. */
    (void) line;
}

void dac_poweroff(dac_t line)
{
    /* The DAC is always on. */
    (void) line;
}
