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
 * @brief       Interface for the ESP-IDF UART HAL API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef ESP_IDF_API_UART_H
#define ESP_IDF_API_UART_H

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#ifdef __cplusplus
extern "C" {
#endif

void esp_idf_uart_set_wakeup_threshold(unsigned uart_num, uint32_t threshold);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ESP_IDF_API_UART_H */
