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

#include <stdbool.h>
#include <stdint.h>

#include "driver/uart.h"
#include "hal/uart_hal.h"

#include "esp_idf_api/uart.h"

static uart_hal_context_t _uart_hal_ctx[] = {
#if UART_NUM_MAX >= 1
    {
        .dev = UART_LL_GET_HW(0),
    },
#endif
#if UART_NUM_MAX >= 2
    {
        .dev = UART_LL_GET_HW(1),
    },
#endif
#if UART_NUM_MAX >= 3
    {
        .dev = UART_LL_GET_HW(2),
    },
#endif
};

void esp_idf_uart_set_wakeup_threshold(unsigned uart_num, uint32_t threshold)
{
    assert(uart_num < ARRAY_SIZE(_uart_hal_ctx));
    uart_hal_set_wakeup_thrd(&_uart_hal_ctx[uart_num], threshold);
}
