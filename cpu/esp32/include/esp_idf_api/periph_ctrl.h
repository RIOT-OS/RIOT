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

#ifndef ESP_IDF_API_PERIPH_CTRL_H
#define ESP_IDF_API_PERIPH_CTRL_H

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

void esp_idf_periph_module_enable(periph_module_t periph);
void esp_idf_periph_module_disable(periph_module_t periph);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ESP_IDF_API_PERIPH_CTRL_H */
