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
 * @brief       Interface for the ESP-IDF DAC HAL API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef ESP_IDF_API_DAC_H
#define ESP_IDF_API_DAC_H

#include "esp_err.h"
#include "hal/dac_types.h"

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ESP-IDF interface wrapper functions
 * @{
 */
esp_err_t esp_idf_dac_output_voltage(dac_channel_t channel, uint8_t dac_value);
esp_err_t esp_idf_dac_output_enable(dac_channel_t channel);
esp_err_t esp_idf_dac_output_disable(dac_channel_t channel);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ESP_IDF_API_DAC_H */
