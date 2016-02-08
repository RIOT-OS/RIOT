/*
 * Copyright (C) 2014 Freie Universität Berlin
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
#include "periph/adc.h"
#include "periph_conf.h"

/* guard in case that no ADC device is defined */
#if ADC_NUMOF

typedef struct {
    int max_value;
    int bits;
} adc_config_t;

adc_config_t adc_config[ADC_NUMOF];

/**
 * @brief Perform ADC calibration routine.
 *
 * This is a recipe taken straight from the Kinetis K60 reference manual. It has
 * been tested on MK60DN512VLL10.
 *
 * @param[in]  ADC_ptr     Pointer to ADC device to operate on.
 *
 * @return 0 on success
 * @return <0 on errors
 */
int kinetis_adc_calibrate(ADC_Type *ADC_ptr)
{
    uint16_t cal;

    ADC_ptr->SC3 |= ADC_SC3_CAL_MASK;

    while (ADC_ptr->SC3 & ADC_SC3_CAL_MASK); /* wait for calibration to finish */

    while (!(ADC_ptr->SC1[0] & ADC_SC1_COCO_MASK));

    if (ADC_ptr->SC3 & ADC_SC3_CALF_MASK) {
        /* calibration failed for some reason, possibly SC2[ADTRG] is 1 ? */
        return -1;
    }

    /*
     * Following the steps in the reference manual:
     */
    /* 1. Initialize or clear a 16-bit variable in RAM. */
    /* 2. Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, and
     * CLPS to the variable. */
    cal = ADC_ptr->CLP0 + ADC_ptr->CLP1 + ADC_ptr->CLP2 + ADC_ptr->CLP3 +
          ADC_ptr->CLP4 + ADC_ptr->CLPS;
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
    ADC_ptr->PG = cal;

    /* 7. Repeat the procedure for the minus-side gain calibration value. */
    cal = ADC_ptr->CLM0 + ADC_ptr->CLM1 + ADC_ptr->CLM2 + ADC_ptr->CLM3 +
          ADC_ptr->CLM4 + ADC_ptr->CLMS;
    cal /= 2;
    cal |= (1 << 15);
    ADC_ptr->MG = cal;

    return 0;
}


int adc_init(adc_t dev, adc_precision_t precision)
{
    ADC_Type *adc = 0;
    PORT_Type *port[ADC_MAX_CHANNELS];
    uint8_t pins[ADC_MAX_CHANNELS];
    uint8_t af[ADC_MAX_CHANNELS];
    int channels = 0;
    uint32_t mode = 0;
    uint32_t div = 0;
    int i = 0;
    uint32_t clock = 0;

    adc_poweron(dev);

    switch (dev) {
#if ADC_0_EN

        case ADC_0:
            adc = ADC_0_DEV;
            port[0] = ADC_0_CH0_PORT;
            port[1] = ADC_0_CH1_PORT;
            port[2] = ADC_0_CH2_PORT;
            port[3] = ADC_0_CH3_PORT;
            port[4] = ADC_0_CH4_PORT;
            port[5] = ADC_0_CH5_PORT;
            pins[0] = ADC_0_CH0_PIN;
            pins[1] = ADC_0_CH1_PIN;
            pins[2] = ADC_0_CH2_PIN;
            pins[3] = ADC_0_CH3_PIN;
            pins[4] = ADC_0_CH4_PIN;
            pins[5] = ADC_0_CH5_PIN;
            af[0] = ADC_0_CH0_PIN_AF;
            af[1] = ADC_0_CH1_PIN_AF;
            af[2] = ADC_0_CH2_PIN_AF;
            af[3] = ADC_0_CH3_PIN_AF;
            af[4] = ADC_0_CH4_PIN_AF;
            af[5] = ADC_0_CH5_PIN_AF;
            channels = ADC_0_CHANNELS;
            clock = ADC_0_MODULE_CLOCK;
            ADC_0_PORT_CLKEN();
            break;
#endif
#if ADC_1_EN

        case ADC_1:
            adc = ADC_1_DEV;
            port[0] = ADC_1_CH0_PORT;
            port[1] = ADC_1_CH1_PORT;
            port[2] = ADC_1_CH2_PORT;
            port[3] = ADC_1_CH3_PORT;
            port[4] = ADC_1_CH4_PORT;
            port[5] = ADC_1_CH5_PORT;
            pins[0] = ADC_1_CH0_PIN;
            pins[1] = ADC_1_CH1_PIN;
            pins[2] = ADC_1_CH2_PIN;
            pins[3] = ADC_1_CH3_PIN;
            pins[4] = ADC_1_CH4_PIN;
            pins[5] = ADC_1_CH5_PIN;
            af[0] = ADC_1_CH0_PIN_AF;
            af[1] = ADC_1_CH1_PIN_AF;
            af[2] = ADC_1_CH2_PIN_AF;
            af[3] = ADC_1_CH3_PIN_AF;
            af[4] = ADC_1_CH4_PIN_AF;
            af[5] = ADC_1_CH5_PIN_AF;
            channels = ADC_1_CHANNELS;
            clock = ADC_1_MODULE_CLOCK;
            ADC_1_PORT_CLKEN();
            break;
#endif

        default:
            return -1;
    }

    if (channels > ADC_MAX_CHANNELS) {
        return -1;
    }

    /* set precision, these numbers are valid for the K60 */
    switch (precision) {
        case ADC_RES_6BIT:
            /* Not supported by hardware, use 8 bit mode. */
            mode = ADC_CFG1_MODE(0);
            adc_config[dev].bits = 8;
            break;

        case ADC_RES_8BIT:
            mode = ADC_CFG1_MODE(0);
            adc_config[dev].bits = 8;
            break;

        case ADC_RES_10BIT:
            mode = ADC_CFG1_MODE(2);
            adc_config[dev].bits = 10;
            break;

        case ADC_RES_12BIT:
            mode = ADC_CFG1_MODE(1);
            adc_config[dev].bits = 12;
            break;

        case ADC_RES_14BIT:
            /* Not supported by hardware, use 16 bit mode. */
            mode = ADC_CFG1_MODE(3);
            adc_config[dev].bits = 16;
            break;

        case ADC_RES_16BIT:
            mode = ADC_CFG1_MODE(3);
            adc_config[dev].bits = 16;
            break;
    }

    adc_config[dev].max_value = (1 << adc_config[dev].bits) - 1;

    for (i = 0; i < channels; i++) {
        /* Check whether we need to set the pin mux for this channel. */
        if (port[i] != NULL) {
            port[i]->PCR[pins[i]] = PORT_PCR_MUX(af[i]);
        }
    }

    /* The ADC requires at least 2 MHz module clock for full accuracy, and less
     * than 12 MHz */
    /* For the calibration it is important that the ADC clock is <= 4 MHz */
    if (clock > 4000000 * 8) {
        /* Need to divide by 16, we set the clock input to BusClock/2 and
         * divide clock by 8 (the maximum divider) */
        div = ADC_CFG1_ADIV(3) | ADC_CFG1_ADICLK(1);
    }
    else if (clock > 4000000 * 4) {
        /* divide clock by 8 */
        div = ADC_CFG1_ADIV(3);
    }
    else if (clock > 4000000 * 2) {
        /* divide clock by 4 */
        div = ADC_CFG1_ADIV(2);
    }
    else if (clock > 4000000 * 1) {
        /* divide clock by 2 */
        div = ADC_CFG1_ADIV(1);
    }
    else {
        /* no clock divider */
        div = ADC_CFG1_ADIV(0);
    }

    /* set configuration register 1: clocking and precision */
    /* Set long sample time */
    adc->CFG1 = ADC_CFG1_ADLSMP_MASK | mode | div;

    /* select ADxxb channels, longest sample time (20 extra ADC cycles) */
    adc->CFG2 = ADC_CFG2_MUXSEL_MASK | ADC_CFG2_ADLSTS(0);

    /* select software trigger, external ref pins */
    adc->SC2 = ADC_SC2_REFSEL(0);

    /* select hardware average over 32 samples */
    adc->SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3);

    /* set an (arbitrary) input channel, single-ended mode */
    adc->SC1[0] = ADC_SC1_ADCH(ADC_0_CH0);
    adc->SC1[1] = ADC_SC1_ADCH(ADC_0_CH0);

    /* perform calibration routine */
    if (kinetis_adc_calibrate(adc) != 0) {
        return -1;
    }

    return 0;
}

int adc_sample(adc_t dev, int channel)
{
    ADC_Type *adc = 0;

    switch (dev) {
#if ADC_0_EN

        case ADC_0:
            adc = ADC_0_DEV;

            /* start single conversion on corresponding channel */
            switch (channel) {
                case 0:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_0_CH0);
                    break;

                case 1:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_0_CH1);
                    break;

                case 2:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_0_CH2);
                    break;

                case 3:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_0_CH3);
                    break;

                case 4:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_0_CH4);
                    break;

                case 5:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_0_CH5);
                    break;

                default:
                    return -1;
            }

            break;
#endif
#if ADC_1_EN

        case ADC_1:
            adc = ADC_1_DEV;

            /* start single conversion on corresponding channel */
            switch (channel) {
                case 0:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_1_CH0);
                    break;

                case 1:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_1_CH1);
                    break;

                case 2:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_1_CH2);
                    break;

                case 3:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_1_CH3);
                    break;

                case 4:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_1_CH4);
                    break;

                case 5:
                    adc->SC1[0] = ADC_SC1_ADCH(ADC_1_CH5);
                    break;

                default:
                    return -1;
            }

            break;
#endif
    }

    /* wait until conversion is complete */
    /* TODO: Use interrupts and yield the thread */
    while (!(adc->SC1[0] & ADC_SC1_COCO_MASK));

    /* read and return result */
    return (int)adc->R[0];
}

void adc_poweron(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN

        case ADC_0:
            ADC_0_CLKEN();
            break;
#endif
#if ADC_1_EN

        case ADC_1:
            ADC_1_CLKEN();
            break;
#endif
    }
}

void adc_poweroff(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN

        case ADC_0:
            ADC_0_CLKDIS();
            break;
#endif
#if ADC_1_EN

        case ADC_1:
            ADC_1_CLKDIS();
            break;
#endif
    }
}

int adc_map(adc_t dev, int value, int min, int max)
{
    int scale = max - min;

    /* NB: There is additional room for the multiplication result if using the
     * actual precision setting of the ADC as the limit in this if statement: */
    if (scale < (1 << 16)) {
        /* The ADCs on these CPUs are limited to 16 bits, the result of
         * value x scale will be 31 bits long or less. We use the upper part
         * of a 32 bit word when scaling */
        int32_t tmp = value * scale;
        /* Divide by ADC range to get the scaled result */
        return (tmp / (1 << adc_config[dev].bits));
    }
    else {
        /* Target scale is too large to use int32_t as an in between result */
        int64_t tmp = scale;
        /* Make sure the compiler does not generate code which will truncate the
         * result. */
        tmp *= value;
        /* Divide by ADC range to get the scaled result */
        tmp /= (1 << adc_config[dev].bits);
        return tmp;
    }
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    return ((max - min) / ((float)adc_config[dev].max_value)) * value;
}

#endif /* ADC_NUMOF */
