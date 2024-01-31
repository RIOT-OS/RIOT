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
#include "macros/units.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

/**
 * @brief   Maximum allowed ADC clock speed
 */
#define MAX_ADC_SPEED           MHZ(14)

/**
 * @brief   Allocate locks for all three available ADC devices
 */
// static mutex_t locks[] = {
// #if ADC_DEVS > 1
//     MUTEX_INIT,
// #endif
//     MUTEX_INIT
// };

static inline void prep(adc_t line, adc_res_t res)
{
    (void) res;
    // mutex_lock(&locks[adc_config[line]]);
    periph_clk_en(APB2, (RCU_APB2EN_ADCEN_Msk << 0));

    /* enable the ADC module */
    ADC->CTL1 |= ADC_CTL1_ADCON_Msk;

// #define ADC_OVSAMPCTL_TOVS_Msk            (0x200UL)                 /*!< TOVS (Bitfield-Mask: 0x01)                            */
// #define ADC_OVSAMPCTL_OVSS_Msk            (0x1e0UL)                 /*!< OVSS (Bitfield-Mask: 0x0f)                            */
// #define ADC_OVSAMPCTL_OVSR_Msk            (0x1cUL)                  /*!< OVSR (Bitfield-Mask: 0x07)                            */
// #define ADC_OVSAMPCTL_OVSEN_Msk           (0x1UL)                   /*!< OVSEN (Bitfield-Mask: 0x01)

    /* configure the resolution */
    // ADC->OVSAMPCTL &= ~ADC_OVSAMPCTL_DRES_Msk;
    // switch (res) {
    //     case ADC_RES_12BIT:
    //         ADC->OVSAMPCTL |= 0 << ADC_OVSAMPCTL_DRES_Pos;
    //         break;
    //     case ADC_RES_10BIT:
    //         ADC->OVSAMPCTL |= 1 << ADC_OVSAMPCTL_DRES_Pos;
    //         break;
    //     case ADC_RES_8BIT:
    //         ADC->OVSAMPCTL |= 2 << ADC_OVSAMPCTL_DRES_Pos;
    //         break;
    //     case ADC_RES_6BIT:
    //         ADC->OVSAMPCTL |= 3 << ADC_OVSAMPCTL_DRES_Pos;
    //         break;
    //     default:
    //         break;
    // }

    /* check if this channel is an internal ADC channel, if so
     * enable the internal temperature and Vref */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        ADC->CTL1 |= ADC_CTL1_TSVREN_Msk;
    }
}

static inline void done(void)
{
    /* disable the internal temperature and Vref */
    ADC->CTL1 &= ~ADC_CTL1_TSVREN_Msk;

    /* disable the ADC module */
    ADC->CTL1 &= ~ADC_CTL1_ADCON_Msk;

    periph_clk_dis(APB2, (RCU_APB2EN_ADCEN_Msk << 0));
    // mutex_unlock(&locks[adc_config[line]]);
}

int adc_init(adc_t line)
{
    (void) line;
    uint32_t clk_div = 2;

    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power-on the device */
    prep(line, ADC_RES_12BIT);

    /* configure the pin */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }
    /* set clock prescaler to get the maximal possible ADC clock value */
    for (clk_div = 2; clk_div < 8; clk_div += 2) {
        if ((CLOCK_CORECLOCK / clk_div) <= MAX_ADC_SPEED) {
            break;
        }
    }
    RCU->CFG0 &= ~(RCU_CFG0_ADCPSC_Msk);
    RCU->CFG0 |= ((clk_div / 2) - 1) << RCU_CFG0_ADCPSC_Pos;

    /* resets the selected ADC calibration registers */
    ADC->CTL1 |= ADC_CTL1_RSTCLB_Msk;
    /* check the status of RSTCAL bit */
    while (ADC->CTL1 & ADC_CTL1_RSTCLB_Msk) {}

    /* enable the selected ADC calibration process */
    ADC->CTL1 |= ADC_CTL1_CLB_Msk;
    /* wait for the calibration to have finished */
    while (ADC->CTL1 & ADC_CTL1_CLB_Msk) {}

    /* set all channels to maximum (239.5) cycles for best accuracy */
    ADC->SAMPT0 |= 0x00ffffff;
    ADC->SAMPT1 |= 0x3fffffff;
    /* we want to sample one channel */
    ADC->RSQ0 = 1 << ADC_RSQ0_RL_Pos;
    /* start sampling from software */
    ADC->CTL1 |= ADC_CTL1_ETERC_Msk | ADC_CTL1_ETSRC_Msk;

    /* check if the internal channels are configured to use ADC */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        assert (ADC == ADC);
    }

    /* free the device again */
    done();
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if the linenel is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* check valid resolution */
    // ADC->OVSAMPCTL &= ~ADC_OVSAMPCTL_DRES_Msk;
    // switch (res) {
    //     case ADC_RES_12BIT:
    //     case ADC_RES_10BIT:
    //     case ADC_RES_8BIT:
    //     case ADC_RES_6BIT:
    //         break;
    //     default:
    //         return -1;
    // }

    /* lock and power on the ADC device  */
    prep(line, res);

    /* set conversion channel */
    ADC->RSQ2 = adc_config[line].chan;
    /* start conversion and wait for results */
    ADC->CTL1 |= ADC_CTL1_SWRCST_Msk;
    while (!(ADC->STAT & ADC_STAT_EOC_Msk)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)ADC->RDATA;

    /* the sample is 12 bit even if the resolution is less than 12 bit,
     * scale down the 12 bit value to the requested resolution */
    switch (res) {
        case ADC_RES_10BIT:
            sample = sample >> 2;
            break;
        case ADC_RES_8BIT:
            sample = sample >> 4;
            break;
        case ADC_RES_6BIT:
            sample = sample >> 6;
            break;
        default:
            break;
    }

    /* power off and unlock device again */
    done();

    return sample;
}
