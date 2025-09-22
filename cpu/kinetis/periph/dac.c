/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_dac
 *
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * This driver uses the 12-bit in-CPU DAC module. The 6-bit digital-to-analog
 * converter inside the CMP module is _not_ used by this implementation.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "bit.h"
#include "assert.h"
#include "periph/dac.h"
#include "periph_conf.h"

static inline DAC_Type *dev(dac_t line)
{
    return dac_config[line].dev;
}

int8_t dac_init(dac_t line)
{
    if (line >= DAC_NUMOF) {
        return DAC_NOLINE;
    }

    /* Enable module clock */
    bit_set32(dac_config[line].scgc_addr, dac_config[line].scgc_bit);
    /* Select VDDA as voltage reference */
    dev(line)->C0 = (DAC_C0_DACRFS_MASK);
    /* Disable DMA and buffering */
    dev(line)->C1 = 0;
    dev(line)->C2 = 0;
    /* Enable the device */
    bit_set8(&dac_config[line].dev->C0, DAC_C0_DACEN_SHIFT);
    /* Set output value to zero */
    dac_set(line, 0);

    return DAC_OK;
}

void dac_set(dac_t line, uint16_t value)
{
    assert(line < DAC_NUMOF);

    /* Scale to 12 bit */
    value = (value >> 4);

    dev(line)->DAT[0].DATH = ((value >> 8) & 0xff);
    dev(line)->DAT[0].DATL = (value & 0xff);
}

void dac_poweron(dac_t line)
{
    assert(line < DAC_NUMOF);

    bit_set8(&dac_config[line].dev->C0, DAC_C0_DACEN_SHIFT);
}

void dac_poweroff(dac_t line)
{
    assert(line < DAC_NUMOF);

    bit_clear8(&dac_config[line].dev->C0, DAC_C0_DACEN_SHIFT);
}
