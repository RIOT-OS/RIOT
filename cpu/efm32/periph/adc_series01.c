/*
 * SPDX-FileCopyrightText: 2016-2017 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"

#include "periph_conf.h"
#include "periph/adc.h"

#include "em_cmu.h"
#include "em_adc.h"

static mutex_t adc_lock[ADC_DEV_NUMOF];

int adc_init(adc_t line)
{
    /* check if line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    uint8_t dev = adc_channel_config[line].dev;
    assert(dev < ADC_DEV_NUMOF);

    /* initialize lock */
    mutex_init(&adc_lock[dev]);

    /* enable clock */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(adc_config[dev].cmu, true);

    /* reset and initialize peripheral */
    ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

    init.timebase = ADC_TimebaseCalc(0);
    init.prescale = ADC_PrescaleCalc(400000, 0);

    ADC_Reset(adc_config[dev].dev);
    ADC_Init(adc_config[dev].dev, &init);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    /* resolutions larger than 12 bits are not supported */
    if (res >= ADC_MODE_UNDEF(0)) {
        return -1;
    }

    uint8_t dev = adc_channel_config[line].dev;

    /* lock device */
    mutex_lock(&adc_lock[dev]);

    /* setup channel */
    ADC_InitSingle_TypeDef init = ADC_INITSINGLE_DEFAULT;

    init.acqTime = adc_channel_config[line].acq_time;
    init.reference = adc_channel_config[line].reference;
    init.resolution = (ADC_Res_TypeDef) (res & 0x0F);
#if defined(_SILICON_LABS_32B_SERIES_0)
    init.input = adc_channel_config[line].input;
#elif defined(_SILICON_LABS_32B_SERIES_1)
    init.posSel = adc_channel_config[line].input;
#endif

    ADC_InitSingle(adc_config[dev].dev, &init);

    /* start conversion and block until it completes */
    ADC_Start(adc_config[dev].dev, adcStartSingle);

    while ((adc_config[dev].dev->STATUS & ADC_STATUS_SINGLEDV) == 0);

    int result = ADC_DataSingleGet(adc_config[dev].dev);

    /* for resolutions that are not really supported, shift the result (for
       instance, 10 bit resolution is achieved by shifting a 12 bit sample). */
    result = result >> (res >> 4);

    /* unlock device */
    mutex_unlock(&adc_lock[dev]);

    return result;
}
