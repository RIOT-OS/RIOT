/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture specific ADC definitions and functions for ESP32
 *
 * All ESP32x SoCs have two SAR ADC units each. However, these have
 * functionalities as well as specific properties that vary between the
 * ESP32x SoC and therefore require different handling for each ESP32x SoC.
 * This is already taken into account in the high-level API of the ESP-IDF.
 * To avoid having to reimplement these specifics and the different handling,
 * the high-level API of the ESP-IDF is used directly for the ADC peripherals.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "periph/adc.h"
#include "periph/gpio.h"

#include "driver/adc.h"
#include "hal/adc_types.h"

/**
 * @brief  Attenuations that can be set for ADC lines
 *
 * Event though ESP-IDF type `adc_atten_t` and `ADC_ATTEN_DB_*` are used
 * now, the `adc_attenuation_t` type with constants `ADC_ATTENUATION_*_DB` are
 * kept for compatibility.
 */
typedef enum {
    ADC_ATTENUATION_0_DB = ADC_ATTEN_DB_0,    /**< full-range is about 1.1 V (Vref) */
    ADC_ATTENUATION_3_DB = ADC_ATTEN_DB_2_5,  /**< full-range is about 1.5 V */
    ADC_ATTENUATION_6_DB = ADC_ATTEN_DB_6,    /**< full-range is about 2.2 V */
    ADC_ATTENUATION_11_DB = ADC_ATTEN_DB_11,  /**< full-range is about 3.3 V */
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
 *  0 dB           | 0 ... 1.1V (Vref) | ADC_ATTEN_DB_0
 *  2.5 dB         | 0 ... 1.5V        | ADC_ATTEN_DB_2_5
 *  6 dB           | 0 ... 2.2V        | ADC_ATTEN_DB_6
 * 11 dB (default) | 0 ... 3.3V        | ADC_ATTEN_DB_11
 *
 * </center>
 *
 * @note: The reference voltage Vref can vary from ADC unit to ADC unit in
 * the range of 1.0V and 1.2V. The Vref of a unit can be routed with
 * function *adc_vref_to_gpio* to a GPIO pin.
 *
 * @param   line    ADC line for which the attenuation is set
 * @param   atten   Attenuation, see type definition of *adc_attenuation_t
 * @return  0       on success
 * @return  -1      on error
 */
int adc_set_attenuation(adc_t line, adc_atten_t atten);

/**
 * @brief   Output reference voltage of a ADC line to GPIO n
 *
 * The Vref of the ADC unit of the given ADC line is routed to a GPIO pin n.
 * This allows to measure the Vref used by the ADC unit to adjusted the
 * results of the conversions accordingly.
 *
 * @note
 * - The given GPIO must be a valid ADC channel of ADC2 unit.
 * - For ESP32 and ESP32C3, the given ADC line has to be a channel of ADC2 unit.
 *
 * @param   line    ADC line for which Vref of its ADC unit is routed to the GPIO
 * @param   gpio    GPIO to which Vref is routed (ADC2 channel GPIOs only)
 *
 * @return  0       on success
 * @return  -1      on error
 */
int adc_line_vref_to_gpio(adc_t line, gpio_t gpio);

#if defined(CPU_FAM_ESP32)
/**
 * @brief   Output ADC reference voltage to GPIO25
 *
 * This function is deprecated and will be removed in future versions.
 *
 * @return  0 on success
 * @return  -1 on invalid ADC line
 */
static inline int adc_vref_to_gpio25(void)
{
    return adc_vref_to_gpio(ADC_UNIT_2, GPIO25);
}
#endif

#ifdef __cplusplus
}
#endif
