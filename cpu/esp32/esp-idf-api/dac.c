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

#ifdef MODULE_PERIPH_DAC

#include <stdbool.h>

#include "driver/dac_common.h"

esp_err_t esp_idf_dac_output_voltage(dac_channel_t channel, uint8_t dac_value)
{
    return dac_output_voltage(channel, dac_value);
}

esp_err_t esp_idf_dac_output_enable(dac_channel_t channel)
{
    return dac_output_enable(channel);
}

esp_err_t esp_idf_dac_output_disable(dac_channel_t channel)
{
    return dac_output_disable(channel);
}

#endif
