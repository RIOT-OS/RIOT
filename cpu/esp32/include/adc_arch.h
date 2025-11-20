/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "driver/adc_types_legacy.h"

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
    ADC_ATTENUATION_12_DB = ADC_ATTEN_DB_12,  /**< full-range is about 3.3 V */
} adc_attenuation_t;

/**
 * @brief   Attenuation of 11 dB is depcricated and has to be mapped
 *
 * The define ensures the compatibility with older versions.
 *
 * @deprecated `ADC_ATTENUATION_11_DB` is deprecated, use
 *             `ADC_ATTENUATION_12_DB` instead.
 */
#define ADC_ATTENUATION_11_DB   ADC_ATTENUATION_12_DB

/**
 * @brief   Set the attenuation for the ADC line. Default attenuation is 12 dB.
 *
 * For each ADC line, an attenuation of the input signal can be defined
 * separately. This results in different full ranges of the measurable voltage
 * at the input. The attenuation can be set to 0 dB, 3 dB, 6 dB and 12 dB,
 * with 12 dB being the standard attenuation. Since an ADC input is measured
 * against a reference voltage Vref of 1.1 V, approximately the following
 * measurement ranges are given when using a corresponding attenuation:
 *
 * <center>
 *
 * Attenuation     | Voltage Range     | Symbol
 * ----------------|-------------------|----------------------
 *  0 dB           | 0 ... 1.1V (Vref) | `ADC_ATTEN_DB_0`
 *  2.5 dB         | 0 ... 1.5V        | `ADC_ATTEN_DB_2_5`
 *  6 dB           | 0 ... 2.2V        | `ADC_ATTEN_DB_6`
 * 12 dB (default) | 0 ... 3.3V        | `ADC_ATTEN_DB_12`
 *
 * </center>
 *
 * @pre   #adc_init must have been executed for the line before.
 * @note  The function has to be executed before @ref adc_sample if required.
 *        The configured attenuation is then used for all subsequent samples.
 *
 * @param [in] line   ADC line for which the attenuation is set
 * @param [in] atten  Attenuation, see type definition of @ref adc_attenuation_t
 * @retval  0    on success
 * @retval  -1   on error
 */
int adc_set_attenuation(adc_t line, adc_atten_t atten);

/**
 * @brief   Get the voltage for a given sample value
 *
 * The function converts the given sample value as read from the channel
 * according to the attenuation set with @ref adc_set_attenuation and the
 * resolution used to read the sample with @ref adc_sample. It uses a predefined
 * calibration scheme and the calibration parameters that have been burned
 * into the eFuses of the ESP32x SoC. If the calibration parameters have not
 * been burned into the eFuses and the initialization of the calibration
 * fails, a linear conversion according to the predefined voltage
 * ranges is used as a fallback.
 *
 * @note In the case that the initialization of the calibration fails,
 *       the function returns `-EINVAL` and the value in parameter `voltage`
 *       is expected to be inaccurate.
 * @note For ESP32, the valid voltages start at around 140 mV.
 *
 * @param [in]  line    ADC line
 * @param [in]  sample  sample sample as read by adc_read
 * @param [out] voltage voltage in mV
 * @retval 0 on success
 * @retval -EINVAL if the initialization of the calibration failed
 */
int adc_raw_to_voltage(adc_t line, int sample, int *voltage);

#if !DOXYGEN
/**
 * @brief   Output reference voltage of a ADC line to a GPIO pin
 *
 * The Vref of the ADC unit for the given ADC line is routed to a GPIO pin.
 * This allows to measure the Vref used by the ADC unit to adjusted the
 * results of the conversions accordingly.
 *
 * @warning The function is not supported any longer, use
 *          @ref adc_raw_to_voltage to get the voltage for a sample value.
 *
 * @note
 * - The given GPIO must be a valid ADC channel of ADC2 unit.
 * - For ESP32, only the internal reference voltage of ADC2 is given.
 *
 * @param [in] line    ADC line for which Vref of its ADC unit is routed to the GPIO
 * @param [in] gpio    GPIO to which Vref is routed (ADC2 channel GPIOs only)
 *
 * @retval  0       on success
 * @retval  -1      on error
 */
__attribute__((__deprecated__))
int adc_line_vref_to_gpio(adc_t line, gpio_t gpio);

#if defined(CPU_FAM_ESP32)
/**
 * @brief   Output ADC reference voltage to GPIO25
 *
 * This function is deprecated and will be removed in future versions.
 *
 * @warning The function is not supported any longer, use
 *          @ref adc_raw_to_voltage to get the voltage for a sample value.
 *
 * @retval  0 on success
 * @retval  -1 on invalid ADC line
 */
__attribute__((__deprecated__))
static inline int adc_vref_to_gpio25(void)
{
    return adc_vref_to_gpio(ADC_UNIT_2, GPIO25);
}
#endif
#endif /* !DOXYGEN */

#ifdef __cplusplus
}
#endif
