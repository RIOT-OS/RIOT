/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture specific ADC functions for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef ADC_ARCH_H
#define ADC_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/gpio.h"
#include "periph/adc.h"

/**
 * @brief  Attenuations that can be set for ADC lines
 */
typedef enum {
    ADC_ATTENUATION_0_DB = 0, /**< full-range is about 1.1 V (Vref) */
    ADC_ATTENUATION_3_DB,     /**< full-range is about 1.5 V */
    ADC_ATTENUATION_6_DB,     /**< full-range is about 2.2 V */
    ADC_ATTENUATION_11_DB     /**< full-range is about 3.3 V */
} adc_attenuation_t;

/**
 * @brief   Set the attenuation for the ADC line. Default attenuation is 11 dB.
 *
 * For each ADC line, an attenuation of the input signal can be defined
 * separately. This results in different full ranges of the measurable voltage
 * at the input. The attenuation can be set to 0 dB, 3 dB, 6 dB and 11 dB,
 * with 11 dB being the standard attenuation. Since an ADC input is measured
 * against a reference voltage Vref of 1.1 V, approximately the following
 * measurement ranges are given when using a corresponding attenuation:
 *
 * <center>
 *
 * Attenuation     | Voltage Range     | Symbol
 * ----------------|-------------------|----------------------
 *  0 dB           | 0 ... 1.1V (Vref) | ADC_ATTENUATION_0_DB
 *  3 dB           | 0 ... 1.5V        | ADC_ATTENUATION_3_DB
 *  6 dB           | 0 ... 2.2V        | ADC_ATTENUATION_6_DB
 * 11 dB (default) | 0 ... 3.3V        | ADC_ATTENUATION_11_DB
 *
 * </center>
 *
 * Please note: The reference voltage Vref can vary from device to device in
 * the range of 1.0V and 1.2V. The Vref of a device can be read with the
 * function *adc_vref_to_gpio25* at the pin GPIO 25. The results of the ADC
 * input can then be adjusted accordingly.
 *
 * @param   line    ADC line for which the attenuation is set
 * @param   atten   Attenuation, see type definition of *adc_attenuation_t
 * @return  0 on success
 * @return  -1 on invalid ADC line
 */
int adc_set_attenuation(adc_t line, adc_attenuation_t atten);

/**
 * @brief   Output ADC reference voltage to GPIO25
 *
 * @return  0 on success
 * @return  -1 on invalid ADC line
 */
int adc_vref_to_gpio25 (void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_ARCH_H */
