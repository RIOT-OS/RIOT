/*
 * Copyright (C) 2014 Simon Brummer
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/dac.h"
#include "periph_conf.h"


int8_t dac_init(dac_t line)
{
    if (line >= DAC_NUMOF) {
        return -1;
    }

    /* configure pin */
    gpio_init_analog(dac_config[line].pin);
    /* enable the DAC's clock */
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    /* reset output and enable the line's channel */
    dac_set(line, 0);
    dac_poweron(line);
    return 0;
}

void dac_set(dac_t line, uint16_t value)
{
    value = (value >> 4);       /* scale to 12-bit */
    if (dac_config[line].chan) {
        DAC->DHR12R2 = value;
    }
    else {
        DAC->DHR12R1 = value;
    }
}

void dac_poweron(dac_t line)
{
    DAC->CR |= (1 << (16 * dac_config[line].chan));
}

void dac_poweroff(dac_t line)
{
    DAC->CR &= ~(1 << (16 * dac_config[line].chan));
}
