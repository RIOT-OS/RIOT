/*
 * Copyright (C) 2016 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "cpu.h"
#include "periph/dac.h"
#include "periph_conf.h"

/* guard in case that no DAC device is defined */
#if DAC_NUMOF

/**
 * @brief   Get the DAC configuration from the board (if configured)
 * @{
 */
#ifdef DAC_CONFIG
static const dac_conf_t dac_config[] = DAC_CONFIG;
#else
static const dac_conf_t dac_config[] = {};
#endif
/** @} */

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

#endif /* DAC_NUMOF */
