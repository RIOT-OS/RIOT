/*
 * SPDX-FileCopyrightText: 2014 Simon Brummer
 * SPDX-FileCopyrightText: 2015-2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#if defined(RCC_APB1ENR_DAC1EN)
#define RCC_BIT             (RCC_APB1ENR_DAC1EN)
#elif defined(RCC_APB1ENR1_DAC1EN)
#define RCC_BIT             (RCC_APB1ENR1_DAC1EN)
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

#ifdef DAC_DHR12L2_DACC2DHR
    if (dac_config[line].chan & 0x01) {
        dev(line)->DHR12L2 = value;
    }
    else {
        dev(line)->DHR12L1 = value;
    }
#else
    dev(line)->DHR12L1 = value;
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

#if VREFBUF_ENABLE && defined(VREFBUF_CSR_ENVR)
    /* enable VREFBUF if needed and available (for example if the board doesn't
     * have an external reference voltage connected to V_REF+), wait until
     * it is ready */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    VREFBUF->CSR &= ~VREFBUF_CSR_HIZ;
    VREFBUF->CSR |= VREFBUF_CSR_ENVR;
    while (!(VREFBUF->CSR & VREFBUF_CSR_VRR)) { }
#endif

#ifdef DAC_MCR_MODE1
    /* Normal mode with Buffer enabled and connected to external pin and on-chip
     * peripherals */
    dev(line)->MCR |= (DAC_MCR_MODE1_0 << (16 * (dac_config[line].chan & 0x01)));
#endif

    /* enable corresponding DAC channel */
    dev(line)->CR |= (DAC_CR_EN1 << (16 * (dac_config[line].chan & 0x01)));
}

void dac_poweroff(dac_t line)
{
    assert(line < DAC_NUMOF);

    /* disable corresponding channel */
    dev(line)->CR &= ~(DAC_CR_EN1 << (16 * (dac_config[line].chan & 0x01)));

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
