/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */
#include <stdio.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"

/**
 * @brief   Maximum clock speed
 *
 * The ADC clock speed must be configured to be >2MHz and <12MHz in order for
 * the ADC to work. For ADC calibration to work as intended, the ADC clock speed
 * must further no be >4MHz, so we target a ADC clock speed > 2 and <4MHz.
 */
#define ADC_MAX_CLK         (4000000U)

static mutex_t locks[] = {
    MUTEX_INIT,
#ifdef ADC1
    MUTEX_INIT,
#endif
};

static inline ADC_Type *dev(adc_t line)
{
    return adc_config[line].dev;
}

static inline int dev_num(adc_t line)
{
    if (dev(line) == ADC0) {
        return 0;
    }
#ifdef ADC1
    else if (dev(line) == ADC1) {
        return 1;
    }
#endif
    return -1;
}

static inline void prep(adc_t line)
{
    if (dev(line) == ADC0) {
        BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_ADC0_SHIFT) = 1;
    }
#ifdef ADC1
    else {
        BITBAND_REG32(SIM->SCGC3, SIM_SCGC3_ADC1_SHIFT) = 1;
    }
#endif
    mutex_lock(&locks[dev_num(line)]);
}

static inline void done(adc_t line)
{
    if (dev(line) == ADC0) {
        BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_ADC0_SHIFT) = 0;
    }
#ifdef ADC1
    else {
        BITBAND_REG32(SIM->SCGC3, SIM_SCGC3_ADC1_SHIFT) = 0;
    }
#endif
    mutex_unlock(&locks[dev_num(line)]);
}

/**
 * @brief Perform ADC calibration routine.
 *
 * This is a recipe taken straight from the Kinetis K60 reference manual. It has
 * been tested on MK60DN512VLL10.
 *
 * @param[in]  dev          Pointer to ADC device to operate on.
 *
 * @return 0 on success
 * @return <0 on errors
 */
int kinetis_adc_calibrate(ADC_Type *dev)
{
    uint16_t cal;

    dev->SC3 |= ADC_SC3_CAL_MASK;

    while (dev->SC3 & ADC_SC3_CAL_MASK) {} /* wait for calibration to finish */

    while (!(dev->SC1[0] & ADC_SC1_COCO_MASK)) {}

    if (dev->SC3 & ADC_SC3_CALF_MASK) {
        /* calibration failed for some reason, possibly SC2[ADTRG] is 1 ? */
        return -1;
    }

    /* From the reference manual */
    /* 1. Initialize or clear a 16-bit variable in RAM. */
    /* 2. Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4,
     * and CLPS to the variable. */
    cal = dev->CLP0 + dev->CLP1 + dev->CLP2 + dev->CLP3 + dev->CLP4 + dev->CLPS;
    /* 3. Divide the variable by two. */
    cal /= 2;
    /* 4. Set the MSB of the variable. */
    cal |= (1 << 15);
    /* (5. The previous two steps can be achieved by setting the carry bit,
     * rotating to the right through the carry bit on the high byte and again on
     * the low byte.)
     * We ignore the above optimization suggestion, we most likely only perform
     * this calibration on startup and it will only save nanoseconds. */
    /* 6. Store the value in the plus-side gain calibration register PG. */
    dev->PG = cal;
    /* 7. Repeat the procedure for the minus-side gain calibration value. */
    cal = dev->CLM0 + dev->CLM1 + dev->CLM2 + dev->CLM3 + dev->CLM4 + dev->CLMS;
    cal /= 2;
    cal |= (1 << 15);
    dev->MG = cal;

    return 0;
}

int adc_init(adc_t line)
{
    /* make sure the given line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* prepare the device: lock and power on */
    prep(line);

    /* configure the connected pin mux */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_port(adc_config[line].pin, GPIO_AF_ANALOG);
    }

    /* The ADC requires at least 2 MHz module clock for full accuracy, and less
     * than 12 MHz */
    /* For the calibration it is important that the ADC clock is <= 4 MHz */
    uint32_t adiv;
    int i = 4;
    if (CLOCK_BUSCLOCK > (ADC_MAX_CLK * 8)) {
        adiv = ADC_CFG1_ADIV(3) | ADC_CFG1_ADICLK(1);
    }
    else {
        while ((i > 0) && (CLOCK_BUSCLOCK < (ADC_MAX_CLK * i))) {
            i--;
        }
        adiv = ADC_CFG1_ADIV(i - 1);
    }

    /* set configuration register 1: clocking and precision */
    /* Set long sample time */
    dev(line)->CFG1 = ADC_CFG1_ADLSMP_MASK | adiv;
    /* select ADxxb channels, longest sample time (20 extra ADC cycles) */
    dev(line)->CFG2 = ADC_CFG2_MUXSEL_MASK | ADC_CFG2_ADLSTS(0);
    /* select software trigger, external ref pins */
    dev(line)->SC2 = ADC_SC2_REFSEL(0);
    /* select hardware average over 32 samples */
    dev(line)->SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3);
    /* set an (arbitrary) input channel, single-ended mode */
    dev(line)->SC1[0] = ADC_SC1_ADCH(0);

    /* perform calibration routine */
    int res = kinetis_adc_calibrate(dev(line));
    done(line);
    return res;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if (res > 0xf0) {
        return -1;
    }

    /* lock and power on the device */
    prep(line);

    /* set resolution */
    dev(line)->CFG1 &= ~(ADC_CFG1_MODE_MASK);
    dev(line)->CFG1 |=  (res);
    /* select the channel that is sampled */
    dev(line)->SC1[0] = adc_config[line].chan;
    /* wait until conversion is complete */
    while (!(dev(line)->SC1[0] & ADC_SC1_COCO_MASK)) {}
    /* read and return result */
    sample = (int)dev(line)->R[0];

    /* power off and unlock the device */
    done(line);

    return sample;
}
