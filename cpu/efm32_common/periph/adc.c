/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"

#include "periph_conf.h"
#include "periph/adc.h"

#include "em_cmu.h"
#include "em_adc.h"
#include "em_common_utils.h"

/* guard file in case no ADC device is defined */
#if ADC_NUMOF

static mutex_t adc_lock[ADC_NUMOF] = {
#if ADC_0_EN
    [ADC_0] = MUTEX_INIT,
#endif
#if ADC_1_EN
    [ADC_1] = MUTEX_INIT,
#endif
};

int adc_init(adc_t line)
{
    /* check if line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    uint8_t dev = adc_channel_config[line].dev;

    /* enable clock */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(adc_config[dev].cmu, true);

    /* reset and initialize peripheral */
    EFM32_CREATE_INIT(init, ADC_Init_TypeDef, ADC_INIT_DEFAULT,
        .conf.timebase = ADC_TimebaseCalc(0),
        .conf.prescale = ADC_PrescaleCalc(400000, 0)
    );

    ADC_Reset(adc_config[dev].dev);
    ADC_Init(adc_config[dev].dev, &init.conf);

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    uint8_t dev = adc_channel_config[line].dev;

    /* lock device */
    mutex_lock(&adc_lock[dev]);

    /* setup channel */
    EFM32_CREATE_INIT(init, ADC_InitSingle_TypeDef, ADC_INITSINGLE_DEFAULT,
        .conf.acqTime = adc_channel_config[line].acq_time,
        .conf.reference = adc_channel_config[line].reference,
        .conf.resolution = (ADC_Res_TypeDef) res,
#ifdef _SILICON_LABS_32B_PLATFORM_1
        .conf.input = adc_channel_config[line].input,
#else
        .conf.posSel = adc_channel_config[line].input,
#endif
    );

    ADC_InitSingle(adc_config[dev].dev, &init.conf);

    /* start conversion and block until it completes */
    ADC_Start(adc_config[dev].dev, adcStartSingle);

    while (adc_config[dev].dev->STATUS & ADC_STATUS_SINGLEACT);

    int result = ADC_DataSingleGet(adc_config[dev].dev);

    /* EFM32 has no 6, 8 or 12-bit resolution, therefore use 12-bit sample
        and shift it by two to yield a 10-bit sample */
    if (res == ADC_RES_10BIT) {
        result = result >> 2;
    }

    /* unlock device */
    mutex_unlock(&adc_lock[dev]);

    return result;
}

#endif /* ADC_NUMOF */
