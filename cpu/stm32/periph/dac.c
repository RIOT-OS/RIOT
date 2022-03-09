/*
 * Copyright (C) 2014 Simon Brummer
 *               2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_dac
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
#include "assert.h"
#include "periph/dac.h"

/* DAC channel enable bits */
#ifdef DAC_CR_EN2
#define EN_MASK             (DAC_CR_EN1 | DAC_CR_EN2)
#else
#define EN_MASK             (DAC_CR_EN1)
#endif

/* get RCC bit */
#ifdef RCC_APB1ENR_DAC1EN
#define RCC_BIT             (RCC_APB1ENR_DAC1EN)
#else
#define RCC_BIT             (RCC_APB1ENR_DACEN)
#endif

/* deduct DAC device from given line channel */
static inline DAC_TypeDef *dev(dac_t line)
{
#if defined(DAC2)
    return (dac_config[line].chan > 1) ? DAC2 : DAC1;
#elif defined (DAC1)
    (void) line;
    return DAC1;
#else
    (void) line;
    return DAC;
#endif
}

int8_t dac_init(dac_t line)
{
    if (line >= DAC_NUMOF) {
        return DAC_NOLINE;
    }

    /* configure pin */
    gpio_init_analog(dac_config[line].pin);

    /* reset output and enable the line's channel */
    dac_poweron(line);
    dac_set(line, 0);

    return DAC_OK;
}

void dac_set(dac_t line, uint16_t value)
{
    assert(line < DAC_NUMOF);

    /* scale set value to 12-bit */
    value = (value >> 4);

#ifdef DAC_DHR12R2_DACC2DHR
    if (dac_config[line].chan & 0x01) {
        dev(line)->DHR12R2 = value;
    }
    else {
        dev(line)->DHR12R1 = value;
    }
#else
    dev(line)->DHR12R1 = value;
#endif
}

void dac_poweron(dac_t line)
{
    assert(line < DAC_NUMOF);

    /* enable the DAC's clock */
#if defined(DAC2)
    periph_clk_en(APB1, (dac_config[line].chan > 1) ?
                  RCC_APB1ENR_DAC2EN : RCC_APB1ENR_DAC1EN);
#else
    periph_clk_en(APB1, RCC_BIT);
#endif

    /* enable corresponding DAC channel */
    dev(line)->CR |= (1 << (16 * (dac_config[line].chan & 0x01)));
}

void dac_poweroff(dac_t line)
{
    assert(line < DAC_NUMOF);

    /* disable corresponding channel */
    dev(line)->CR &= ~(1 << (16 * (dac_config[line].chan & 0x01)));

    /* disable the DAC's clock in case no channel is active anymore */
    if (!(dev(line)->CR & EN_MASK)) {
#if defined(DAC2)
        periph_clk_dis(APB1, (dac_config[line].chan > 1) ?
                      RCC_APB1ENR_DAC2EN : RCC_APB1ENR_DAC1EN);
#else
        periph_clk_dis(APB1, RCC_BIT);
#endif
    }
}
