/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "periph/adc.h"
#include "periph_conf.h"

/* guard in case that no ADC device is defined */
#if ADC_NUMOF

static inline uint32_t *_adc_channel_to_pin(int channel)
{
    return (channel < 6) ? ((uint32_t*)(LPC_IOCON) + (11 + channel))
                         : ((uint32_t*)(LPC_IOCON) + (16 + channel));
}

int adc_init(adc_t dev, adc_precision_t precision)
{
    adc_poweron(dev);

    /* Set resolution */
    switch (precision) {
        case ADC_RES_6BIT:
            LPC_ADC->CR |= (0x04 << 17);
            break;
        case ADC_RES_8BIT:
            LPC_ADC->CR |= (0x02 << 17);
            break;
        case ADC_RES_10BIT:
            LPC_ADC->CR |= (0x0 << 17);
            break;
        case ADC_RES_12BIT:
        case ADC_RES_14BIT:
        case ADC_RES_16BIT:
            /* Resolution > 10 bits unsupported */
            adc_poweroff(dev);
            return -1;
            break;
    }

    /* ADC frequency : 3MHz */
    LPC_ADC->CR |= (15 << 8);

    return 0;
}

int adc_sample(adc_t dev, int channel)
{
    if (channel > ADC_MAX_CHANNELS) {
        return -1;
    }

    /* Compute the IOCON register for the channel */
    uint32_t *cfg = _adc_channel_to_pin(channel);
    /* Put the pin in its ADC alternate function */
    if (channel < 5) {
        *cfg |= 2;
    }
    else {
        *cfg |= 1;
    }
    /* Configure ADMODE in analog input */
    *cfg &= ~(1 << 7);
    /* Start a conversion */
    LPC_ADC->CR |= (1 << channel) | (1 << 24);
    /* Wait for the end of the conversion */
    while (!(LPC_ADC->DR[channel] & (1 << 31)));
    /* Read and return result */
    return (LPC_ADC->DR[channel] >> 6);
}

void adc_poweron(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
            LPC_SYSCON->PDRUNCFG &= ~(1 << 4);
            LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
            break;
#endif
    }
}

void adc_poweroff(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
            LPC_SYSCON->PDRUNCFG |= (1 << 4);
            LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 13);
            break;
#endif
    }
}

int adc_map(adc_t dev, int value, int min, int max)
{
    return 0;
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    return 0.0;
}

#endif /* ADC_NUMOF */
