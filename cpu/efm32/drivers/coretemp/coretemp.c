/*
 * SPDX-FileCopyrightText: 2018-2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32_drivers_coretemp
 * @{
 *
 * @file
 * @brief       Implementation of EFM32 internal temperature sensor
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <errno.h>

#include "board.h"
#include "coretemp.h"

#include "periph/adc.h"

#include "em_device.h"

int16_t coretemp_read(void)
{
    /* initialize factory calibration values */
    int32_t cal_temp = ((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >>
                         _DEVINFO_CAL_TEMP_SHIFT);
#if defined(_SILICON_LABS_32B_SERIES_0)
    int32_t cal_value = ((DEVINFO->ADC0CAL2 & _DEVINFO_ADC0CAL2_TEMP1V25_MASK) >>
                          _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT);
#else
    int32_t cal_value = ((DEVINFO->ADC0CAL3 & _DEVINFO_ADC0CAL3_TEMPREAD1V25_MASK) >>
                          _DEVINFO_ADC0CAL3_TEMPREAD1V25_SHIFT);
#endif

    /* no factory calibration values */
    if ((cal_temp == 0xFF) || (cal_value == 0x0FFF)) {
        return -10000;
    }

    /* convert temperature channel */
    int32_t value = adc_sample(CORETEMP_ADC, ADC_RES_12BIT);

    /* t_grad is the inverse of 1.25 Vref / (4096 * mV/C) times 1000, so that
       we can divide by an integer below with sufficient resolution (values
       are from the datasheets) */
#if defined(_SILICON_LABS_32B_SERIES_0)
    int32_t t_grad = -6291; /* -1.92 mV/C */
#else
    int32_t t_grad = -6029; /* -1.84 mv/C */
#endif

    /* convert to degrees centi-degrees Celsius (times 100) */
    return (int16_t) ((cal_temp * 100) - ((cal_value - value) * 100000 / t_grad));
}

int coretemp_init(void)
{
    /* sanity check to ensure the internal temperature sensor is selected */
#if defined(_SILICON_LABS_32B_SERIES_0)
    assert(adc_channel_config[CORETEMP_ADC].input == adcSingleInputTemp);
#else
    assert(adc_channel_config[CORETEMP_ADC].input == adcPosSelTEMP);
#endif

    /* initialize ADC */
    if (adc_init(CORETEMP_ADC) != 0) {
        return -EIO;
    }

    return 0;
}
