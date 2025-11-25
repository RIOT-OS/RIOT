/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_dac
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation for GD32VF103
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "assert.h"
#include "cpu.h"
#include "periph/dac.h"

#define DAC_CTL_DENX_MASK  (DAC_CTL_DEN0_Msk | DAC_CTL_DEN1_Msk)

int8_t dac_init(dac_t line)
{
    assert(line < DAC_NUMOF);

    gpio_init_analog(dac_config[line].pin);

    dac_poweron(line);
    dac_set(line, 0);

    return DAC_OK;
}

void dac_set(dac_t line, uint16_t value)
{
    assert(line < DAC_NUMOF);
    assert(dac_config[line].chan < DAC_CHANNEL_NUMOF);

    /* set the upper 12 bit of the left aligned DAC data holding register */
    if (dac_config[line].chan) {
        DAC->DAC1_L12DH = value & 0xfff0;
    }
    else {
        DAC->DAC0_L12DH = value & 0xfff0;
    }
}

void dac_poweron(dac_t line)
{
    assert(line < DAC_NUMOF);
    assert(dac_config[line].chan < DAC_CHANNEL_NUMOF);

    /* enable the DAC clock */
    periph_clk_en(APB1, RCU_APB1EN_DACEN_Msk);

    /* enable the DAC channel */
    DAC->CTL |= (dac_config[line].chan) ? DAC_CTL_DEN1_Msk : DAC_CTL_DEN0_Msk;
}

void dac_poweroff(dac_t line)
{
    assert(line < DAC_NUMOF);
    assert(dac_config[line].chan < DAC_CHANNEL_NUMOF);

    /* disable the DAC channel */
    DAC->CTL &= ~((dac_config[line].chan) ? DAC_CTL_DEN1_Msk : DAC_CTL_DEN0_Msk);

    if ((DAC->CTL & DAC_CTL_DENX_MASK) == 0) {
        /* disable the DAC clock only if both channels are disabled */
        periph_clk_dis(APB1, RCU_APB1EN_DACEN_Msk);
    }
}
