/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @notice      based on TI peripheral drivers library
 *
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
  */

#include "vendor/hw_memmap.h"
#include "vendor/hw_soc_adc.h"

#include "board.h"
#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/adc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static cc2538_soc_adc_t *soc_adc = (cc2538_soc_adc_t *)SOC_ADC_BASE;

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        DEBUG("adc_init: invalid ADC line (%d)!\n", line);
        return -1;
    }

    /* stop random number generator, and set STSEL = 1 */
    soc_adc->ADCCON1 = (SOC_ADC_ADCCON1_STSEL_M | SOC_ADC_ADCCON1_RCTRL_M);
    /* disable any DMA, continuous ADC settings */
    soc_adc->ADCCON2 = 0x0;
    /* configure ADC GPIO as analog input */
    gpio_init(adc_config[line], GPIO_IN_ANALOG);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    /* check if adc line valid */
    if (line >= ADC_NUMOF) {
        DEBUG("adc_sample: invalid ADC line!\n");
        return -1;
    }

    uint8_t rshift;
    /* check if given resolution valid, and set right shift */
    switch(res) {
        case ADC_RES_7BIT:
            rshift = SOCADC_7_BIT_RSHIFT;
            break;
        case ADC_RES_9BIT:
            rshift = SOCADC_9_BIT_RSHIFT;
            break;
        case ADC_RES_10BIT:
            rshift = SOCADC_10_BIT_RSHIFT;
            break;
        case ADC_RES_12BIT:
            rshift = SOCADC_12_BIT_RSHIFT;
            break;
        default:
            DEBUG("adc_sample: invalid resolution!\n");
            return -1;
    }
    /**
     * @attention CC2538 ADC supports differential comparison of two analog
     * GPIO inputs, hence negative values are possible. RIOT currently allows
     * positive ADC output only. Thus, reduce shift by one to compensate and
     * get full value range according to ADC resolution. E.g. 10 Bit resolution
     * with diff ADC would have [-512,511] range but RIOT expects [0,1023].
     */
    rshift--;

    /* configure adc line with parameters and trigger a single conversion*/
    uint32_t reg = (soc_adc->ADCCON3) & ~(SOC_ADC_ADCCON3_EREF_M |
                                          SOC_ADC_ADCCON3_EDIV_M |
                                          SOC_ADC_ADCCON3_ECH_M);
    soc_adc->ADCCON3 = reg | res | SOC_ADC_ADCCON3_EREF |
                       (adc_config[line] & GPIO_PIN_MASK);

    DEBUG("ADCCON1: %"PRIu32" ADCCON2: %"PRIu32" ADCCON3: %"PRIu32"\n",
          soc_adc->ADCCON1, soc_adc->ADCCON2, soc_adc->ADCCON3);

    /* Poll/wait until end of conversion */
    while ((soc_adc->ADCCON1 & SOC_ADC_ADCCON1_EOC_M) == 0) {}

    /* Read result after conversion completed,
     * reading SOC_ADC_ADCH last will clear SOC_ADC_ADCCON1.EOC */
    int16_t sample = soc_adc->ADCL & SOC_ADC_ADCL_ADC_M;
    sample |= (soc_adc->ADCH & SOC_ADC_ADCH_ADC_M) << 8;
    /* sample right shifted depending on resolution */
    sample = sample >> rshift;
    DEBUG("adc_sample: raw value %"PRIi16"\n", sample);
    /* FIXME: currently RIOT ADC allows values >0 only */
    if (sample < 0) {
        sample = 0;
    }

    return (int)sample;
}
