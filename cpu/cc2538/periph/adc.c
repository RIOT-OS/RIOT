/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "board.h"
#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/adc.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        DEBUG("adc_init: invalid ADC line (%d)!\n", line);
        return -1;
    }

    cc2538_soc_adc_t *adca = SOC_ADC;
    /* stop random number generator, and set STSEL = 1 */
    adca->cc2538_adc_adccon1.ADCCON1 = 0x3c;
    /* disable any DMA, continous ADC settings */
    adca->ADCCON2 = 0x0;
    /* configure ADC GPIO as analog input */
    gpio_init(adc_config[line], GPIO_IN_ANALOG);

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
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
            DEBUG("adc_sample: invalid resultion!\n");
            return -1;
    }
    /**
     * @attention CC2538 ADC supports differential comparision of two analog
     * GPIO inputs, hence negative values are possible. RIOT currently allows
     * positive ADC output only. Thus, reduce shift by one to compensate and
     * get full value range according to ADC resolution. E.g. 10 Bit resultion
     * with diff ADC would have [-512,511] range but RIOT expects [0,1023].
     */
    rshift--;

    cc2538_soc_adc_t *adca = SOC_ADC;
    /* configure adc line with parameters and trigger a single conversion*/
    uint32_t reg = (adca->ADCCON3) & ~(SOC_ADC_ADCCON3_EREF |
                                       SOC_ADC_ADCCON3_EDIV |
                                       SOC_ADC_ADCCON3_ECH);
    adca->ADCCON3 = reg | res | SOC_ADC_ADCCON_REF |
                    (adc_config[line] & GPIO_PIN_MASK);

    DEBUG("ADCCON1: %"PRIu32" ADCCON2: %"PRIu32" ADCCON3: %"PRIu32"\n",
          adca->cc2538_adc_adccon1.ADCCON1, adca->ADCCON2, adca->ADCCON3);

    /* Poll/wait until end of conversion */
    while ((adca->cc2538_adc_adccon1.ADCCON1 &
            SOC_ADC_ADCCON1_EOC_MASK) == 0) {}

    /* Read result after conversion completed,
     * reading SOC_ADC_ADCH last will clear SOC_ADC_ADCCON1.EOC */
    int16_t sample = adca->ADCL & SOC_ADC_ADCL_MASK;
    sample |= (adca->ADCH & SOC_ADC_ADCH_MASK) << 8;
    /* sample right shifted depending on resolution */
    sample = sample >> rshift;
    DEBUG("adc_sample: raw value %"PRIi16"\n", sample);
    /* FIXME: currently RIOT ADC allows values >0 only */
    if (sample < 0) {
        sample = 0;
    }

    return (int)sample;
}
