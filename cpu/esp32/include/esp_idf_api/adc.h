/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_idf_api
 * @{
 *
 * @file
 * @brief       Interface for the ESP-IDF ADC HAL API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef ESP_IDF_API_ADC_H
#define ESP_IDF_API_ADC_H

#include "esp_err.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ESP-IDF interface mapper functions
 * @{
 */
/** @} */
void esp_idf_adc_power_acquire(void);
void esp_idf_adc_power_release(void);

esp_err_t esp_idf_adc1_config_width(adc_bits_width_t width_bit);
esp_err_t esp_idf_adc1_config_channel_atten(adc_channel_t channel,
                                            adc_atten_t atten);
int esp_idf_adc1_get_raw(adc_channel_t channel);

esp_err_t esp_idf_adc2_config_channel_atten(adc_channel_t channel,
                                            adc_atten_t atten);
esp_err_t esp_idf_adc2_get_raw(adc_channel_t channel,
                               adc_bits_width_t width_bit, int *raw_out);
esp_err_t esp_idf_adc_vref_to_gpio(adc_unit_t adc_unit, gpio_num_t gpio);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ESP_IDF_API_ADC_H */
