/*
 * Copyright (C) 2014 Simon Brummer
 *               2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
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

/* only compile this, if the CPU has a DAC */
#if defined(DAC) || defined(DAC1)

#ifdef DAC2
#define _DAC(line)          (dac_config[line].dac ? DAC2 : DAC1)
#else
#define _DAC(line)          DAC
#endif

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
#if defined(DAC2)
    RCC->APB1ENR |= dac_config[line].dac
            ? RCC_APB1ENR_DAC2EN
            : RCC_APB1ENR_DAC1EN;
#elif defined(DAC1)
    RCC->APB1ENR |= RCC_APB1ENR_DAC1EN;
#else
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
#endif

    /* reset output and enable the line's channel */
    dac_set(line, 0);
    dac_poweron(line);
    return 0;
}

void dac_set(dac_t line, uint16_t value)
{
    value = (value >> 4);       /* scale to 12-bit */
#ifdef DAC_DHR12R2_DACC2DHR
    if (dac_config[line].chan) {
        _DAC(line)->DHR12R2 = value;
    }
    else {
        _DAC(line)->DHR12R1 = value;
    }
#else
    (void) line;

    _DAC(line)->DHR12R1 = value;
#endif
}

void dac_poweron(dac_t line)
{
    DAC->CR |= (1 << (16 * dac_config[line].chan));
}

void dac_poweroff(dac_t line)
{
    DAC->CR &= ~(1 << (16 * dac_config[line].chan));
}

#endif /* DAC */
