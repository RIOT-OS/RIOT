/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph/vbat.h"
#include "cpu_common.h"
// #include "gd32e23x_adc.h"

/**
 * @brief   Default VBAT undefined value
 */
#ifndef VBAT_ADC
#define VBAT_ADC    ADC_UNDEF
#endif

/**
 * @brief   Allocate lock for the ADC device
 *
 * All STM32F0 & STM32G0 CPUs we support so far only come with a single ADC device.
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
#ifdef RCU_APB2EN_ADCEN_Msk
    periph_clk_en(APB2, RCU_APB2EN_ADCEN_Msk);
#endif
}

static inline void done(void)
{
#ifdef RCU_APB2EN_ADCEN_Msk
    periph_clk_dis(APB2, RCU_APB2EN_ADCEN_Msk);
#endif
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    /* make sure the given line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power on the device */
    prep();
    /* configure the pin */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }
    /* reset configuration */
    ADC->CTL0 = 0;
    ADC->CTL1 = 0;
    /* enable device */
    ADC->CTL1 = ADC_CTL1_ADCON_Msk;
    /* configure sampling time to save value */
// // set sample rate only of selected channel not of base channel in each register
//     ADC->SAMPT0 = ADC_SAMPLETIME_28POINT5;       /* 28.5 ADC clock cycles */
//     ADC->SAMPT1 = ADC_SAMPLETIME_28POINT5;       /* 28.5 ADC clock cycles */
    /* power off an release device for now */
    done();

    return 0;
}

int32_t adc_sample(adc_t line,  adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    //see below ?
    if ((res == ADC_RES_14BIT) || (res == ADC_RES_16BIT)) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();
    // 1. Make sure the DISRC, SM bits in the ADC_CTL0 register and CTN bit in the ADC_CTL1 register are reset;
    /* set resolution and channel */
    ADC->CTL0_b.DRES = (uint8_t) res;
    ADC->CTL0_b.DISRC = 0;
    ADC->CTL0_b.SM = 0;
    ADC->CTL1_b.CTN = 0;

    // 2. Configure RSQ0 with the analog channel number. (RSQ0 of ADC_RSQ2, not ADC_RSQ0)
    ADC->RSQ2_b.RSQ0 = (1 << adc_config[line].chan);

    // 3. Configure the ADC_SAMPT0 for ch 16, 17,  SAMPT1 for other channels 0-9    11 0x3 for 28.5 cycles.
    switch(adc_config[line].chan){
        case 0:
            ADC->SAMPT1_b.SPT0 = 3;
            break;
        case 1:
            ADC->SAMPT1_b.SPT1 = 3;
            break;
        case 2:
            ADC->SAMPT1_b.SPT2 = 3;
            break;
        case 3:
            ADC->SAMPT1_b.SPT3 = 3;
            break;
        case 4:
            ADC->SAMPT1_b.SPT4 = 3;
            break;
        case 5:
            ADC->SAMPT1_b.SPT5 = 3;
            break;
        case 6:
            ADC->SAMPT1_b.SPT6 = 3;
            break;
        case 7:
            ADC->SAMPT1_b.SPT7 = 3;
            break;
        case 8:
            ADC->SAMPT1_b.SPT8 = 3;
            break;
        case 9:
            ADC->SAMPT1_b.SPT9 = 3;
            break;
        case 16:
            ADC->SAMPT0_b.SPT16 = 3;
            break;
        case 17:
            ADC->SAMPT0_b.SPT17 = 3;
            break;
    }

    // 4. Configure the ETERC 0 and ETSRC bits 111 in the ADC_CTL1 register if in need.
    ADC->CTL1_b.ETERC = 0;
    ADC->CTL1_b.ETSRC = 7;

    // 5. Set the SWRCST bit 1, or generate an external trigger for the routine sequence.
    /* start conversion and wait for results */
    ADC->CTL1_b.SWRCST = 1;
    ADC->CTL1_b.ADCON = 1;

    // 6. Wait for the ADC_STAT EOC flag to be set.
    while (ADC->STAT_b.EOC == 0) {}

    // 7. Read the converted data result from the ADC_RDATA register.
    /* read result */
    sample = (int)ADC->RDATA;

    // 8. Clear the ADC_STAT EOC flag by writing 0.
    /* unlock and power off device again */
    ADC->STAT_b.EOC = 0;
    done();

    return sample;
}
