/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_dac
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
#include "periph/dac.h"
#include "periph_conf.h"

static const dac_conf_t dac_config[] = DAC_CONFIG;

int8_t dac_init(dac_t line)
{
    DAC_Type *dac;
    if ((unsigned int)line >= DAC_NUMOF) {
        return -1;
    }

    dac = dac_config[line].dev;

    /* Enable module clock */
    *(dac_config[line].clk_gate) = 1;

    /* Select VDDA as voltage reference */
    dac->C0 = (DAC_C0_DACRFS_MASK);

    /* Disable DMA and buffering */
    dac->C1 = 0;
    dac->C2 = 0;

    /* Power on */
    dac_poweron(line);

    /* Set output value to zero */
    dac_set(line, 0);
    return 0;
}

void dac_set(dac_t line, uint16_t value)
{
    DAC_Type *dac;

    if ((unsigned int)line >= DAC_NUMOF) {
        return;
    }

    dac = dac_config[line].dev;

    /* Scale to 12 bit */
    value = value >> ((sizeof(value) * 8) - 12);

    dac->DAT[0].DATH = ((value >> 8) & 0xff);
    dac->DAT[0].DATL = (value & 0xff);
}

static inline void _dac_set_power(dac_t line, uint8_t value)
{
    DAC_Type *dac;

    if ((unsigned int)line >= DAC_NUMOF) {
        return;
    }

    dac = dac_config[line].dev;

    BITBAND_REG8(dac->C0, DAC_C0_DACEN_SHIFT) = value;
}

void dac_poweron(dac_t line)
{
    _dac_set_power(line, 1);
}

void dac_poweroff(dac_t line)
{
    _dac_set_power(line, 0);
}
