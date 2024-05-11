/*
 * Copyright (C) 2016 Engineering-Spirit
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "compiler_hints.h"
#include "cpu.h"
#include "macros/units.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

/**
 * @brief   Maximum allowed ADC clock speed
 */
#ifndef ADC_CLK_MAX
#define ADC_CLK_MAX             MHZ(14)
#endif

/**
 * @brief   Allocate locks for all three available ADC devices
 */
static mutex_t locks[] = {
#if ADC_DEVS > 1
    MUTEX_INIT,
#endif
    MUTEX_INIT
};

static inline ADC0_Type *dev(adc_t line)
{
    switch (adc_config[line].dev) {
    case 0:
        return (ADC0_Type *)ADC0_BASE;
#if ADC_DEVS > 1
    case 1:
        return (ADC0_Type *)ADC1_BASE;
#endif
    default:
        assert(0);
        return NULL;
    }
}

static inline void prep(adc_t line, adc_res_t res)
{
    mutex_lock(&locks[adc_config[line].dev]);
    periph_clk_en(APB2, (RCU_APB2EN_ADC0EN_Msk << adc_config[line].dev));

    /* enable the ADC module */
    dev(line)->CTL1 |= ADC0_CTL1_ADCON_Msk;

    /* configure the resolution */
    dev(line)->OVSAMPCTL &= ~ADC0_OVSAMPCTL_DRES_Msk;
    switch (res) {
        case ADC_RES_12BIT:
            dev(line)->OVSAMPCTL |= 0 << ADC0_OVSAMPCTL_DRES_Pos;
            break;
        case ADC_RES_10BIT:
            dev(line)->OVSAMPCTL |= 1 << ADC0_OVSAMPCTL_DRES_Pos;
            break;
        case ADC_RES_8BIT:
            dev(line)->OVSAMPCTL |= 2 << ADC0_OVSAMPCTL_DRES_Pos;
            break;
        case ADC_RES_6BIT:
            dev(line)->OVSAMPCTL |= 3 << ADC0_OVSAMPCTL_DRES_Pos;
            break;
        default:
            break;
    }

    /* check if this channel is an internal ADC channel, if so
     * enable the internal temperature and Vref */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        dev(line)->CTL1 |= ADC0_CTL1_TSVREN_Msk;
    }
}

static inline void done(adc_t line)
{
    /* disable the internal temperature and Vref */
    dev(line)->CTL1 &= ~ADC0_CTL1_TSVREN_Msk;

    /* disable the ADC module */
    dev(line)->CTL1 &= ~ADC0_CTL1_ADCON_Msk;

    periph_clk_dis(APB2, (RCU_APB2EN_ADC0EN_Msk << adc_config[line].dev));
    mutex_unlock(&locks[adc_config[line].dev]);
}

int adc_init(adc_t line)
{
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
        if ((periph_apb_clk(APB2) / clk_div) <= ADC_CLK_MAX) {
            break;
        }
    }
    assume((periph_apb_clk(APB2) / clk_div) <= ADC_CLK_MAX);
    RCU->CFG0 &= ~(RCU_CFG0_ADCPSC_2_Msk);
    RCU->CFG0 |= ((clk_div / 2) - 1) << RCU_CFG0_ADCPSC_2_Pos;

    /* resets the selected ADC calibration registers */
    dev(line)->CTL1 |= ADC0_CTL1_RSTCLB_Msk;
    /* check the status of RSTCAL bit */
    while (dev(line)->CTL1 & ADC0_CTL1_RSTCLB_Msk) {}

    /* enable the selected ADC calibration process */
    dev(line)->CTL1 |= ADC0_CTL1_CLB_Msk;
    /* wait for the calibration to have finished */
    while (dev(line)->CTL1 & ADC0_CTL1_CLB_Msk) {}

    /* set all channels to maximum (239.5) cycles for best accuracy */
    dev(line)->SAMPT0 |= 0x00ffffff;
    dev(line)->SAMPT1 |= 0x3fffffff;
    /* we want to sample one channel */
    dev(line)->RSQ0 = 1 << ADC0_RSQ0_RL_Pos;
    /* start sampling from software */
    dev(line)->CTL1 |= ADC0_CTL1_ETERC_Msk | ADC0_CTL1_ETSRC_Msk;

    /* check if the internal channels are configured to use ADC0 */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        assert (dev(line) == ADC0);
    }

    /* free the device again */
    done(line);
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
    dev(line)->OVSAMPCTL &= ~ADC0_OVSAMPCTL_DRES_Msk;
    switch (res) {
        case ADC_RES_12BIT:
        case ADC_RES_10BIT:
        case ADC_RES_8BIT:
        case ADC_RES_6BIT:
            break;
        default:
            return -1;
    }

    /* lock and power on the ADC device  */
    prep(line, res);

    /* set conversion channel */
    dev(line)->RSQ2 = adc_config[line].chan;
    /* start conversion and wait for results */
    dev(line)->CTL1 |= ADC0_CTL1_SWRCST_Msk;
    while (!(dev(line)->STAT & ADC0_STAT_EOC_Msk)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)dev(line)->RDATA;

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
    done(line);

    return sample;
}
