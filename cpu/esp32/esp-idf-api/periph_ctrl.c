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
 * @brief       Interface for ESP-IDF peripherals control API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "driver/periph_ctrl.h"

void esp_idf_periph_module_enable(periph_module_t periph)
{
    periph_module_enable(periph);
}

void esp_idf_periph_module_disable(periph_module_t periph)
{
    periph_module_disable(periph);
}
