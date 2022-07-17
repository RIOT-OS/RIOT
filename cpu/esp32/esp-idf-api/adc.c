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

#include <stdbool.h>

#include "driver/adc.h"

void esp_idf_adc_power_acquire(void)
{
    adc_power_acquire();
}

void esp_idf_adc_power_release(void)
{
    adc_power_release();
}

esp_err_t esp_idf_adc1_config_width(adc_bits_width_t width_bit)
{
    return adc1_config_width(width_bit);
}

esp_err_t esp_idf_adc1_config_channel_atten(adc_channel_t channel,
                                            adc_atten_t atten)
{
    return adc1_config_channel_atten(channel, atten);
}

int esp_idf_adc1_get_raw(adc1_channel_t channel)
{
    return adc1_get_raw(channel);
}

esp_err_t esp_idf_adc2_config_channel_atten(adc_channel_t channel,
                                            adc_atten_t atten)
{
    return adc2_config_channel_atten(channel, atten);
}

esp_err_t esp_idf_adc2_get_raw(adc2_channel_t channel,
                               adc_bits_width_t width_bit, int *raw_out)
{
    return adc2_get_raw(channel, width_bit, raw_out);
}

esp_err_t esp_idf_adc_vref_to_gpio(adc_unit_t adc_unit, gpio_num_t gpio)
{
    return adc_vref_to_gpio(adc_unit, gpio);
}
