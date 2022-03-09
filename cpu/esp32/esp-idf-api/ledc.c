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

#include <stdbool.h>

#include "driver/ledc.h"

int esp_ledc_channel_config(const ledc_channel_config_t* ledc_conf)
{
    return ledc_channel_config(ledc_conf);
}

int esp_ledc_timer_config(const ledc_timer_config_t* timer_conf)
{
    return ledc_timer_config(timer_conf);
}

int esp_ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    return ledc_update_duty(speed_mode, channel);
}

int esp_ledc_set_duty_with_hpoint(ledc_mode_t speed_mode,
                                  ledc_channel_t channel,
                                  uint32_t duty, uint32_t hpoint)
{
    return ledc_set_duty_with_hpoint(speed_mode, channel, duty, hpoint);
}
