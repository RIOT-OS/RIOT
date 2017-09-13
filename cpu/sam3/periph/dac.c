/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/dac.h"

#define WP_KEY              (0x444143)
#define PMC_BIT             (1 << (ID_DACC - 32))

#ifndef DAC_STARTUP
#define DAC_STARTUP         (DACC_MR_STARTUP_1984)
#endif
#ifndef DAC_REFRESH
#define DAC_REFRESH         (DACC_MR_REFRESH(128))
#endif

int8_t dac_init(dac_t line)
{
    assert(line < DAC_NUMOF);

    /* power on DACC peripheral */
    PMC->PMC_PCER1 = PMC_BIT;

    /* unlock DACC registers */
    DACC->DACC_WPMR = DACC_WPMR_WPKEY(WP_KEY);
    /* configure mode register */
    DACC->DACC_MR = (DAC_STARTUP | DACC_MR_TAG_EN | DAC_REFRESH);
    /* enable the selected channel/line */
    DACC->DACC_CHER = (1 << line);
    /* set line initially to 0 volts -> this will startup the channel and the
     * channel is ready after the defined number of startup cycles
     * (DAC_STARTUP) have passed */
    dac_set(line, 0);

    return 0;
}

void dac_set(dac_t line, uint16_t value)
{
    assert(line < DAC_NUMOF);

    DACC->DACC_CDR = ((value >> 4) | (line << 12));
}

void dac_poweron(dac_t line)
{
    assert(line < DAC_NUMOF);

    PMC->PMC_PCER1 = (1 << (ID_DACC - 32));
    DACC->DACC_CHER = PMC_BIT;
}

void dac_poweroff(dac_t line)
{
    assert(line < DAC_NUMOF);

    DACC->DACC_CHDR = (1 << line);
    if (!(DACC->DACC_CHSR)) {
        PMC->PMC_PCDR1 = PMC_BIT;
    }
}
