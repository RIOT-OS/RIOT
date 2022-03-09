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
 * @brief       Interface for the ESP-IDF LEDC HAL API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef ESP_IDF_API_LEDC_H
#define ESP_IDF_API_LEDC_H

#include "hal/ledc_types.h"

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ESP-IDF interface wrapper functions
 * @{
 */
int esp_ledc_channel_config(const ledc_channel_config_t* ledc_conf);
int esp_ledc_timer_config(const ledc_timer_config_t* timer_conf);
int esp_ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);
int esp_ledc_set_duty_with_hpoint(ledc_mode_t speed_mode,
                                  ledc_channel_t channel,
                                  uint32_t duty, uint32_t hpoint);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ESP_IDF_API_LEDC_H */
