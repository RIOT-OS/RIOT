/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART functions required by ESP32x
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "periph/uart.h"

#include "esp_err.h"
#include "esp_private/uart_share_hw_ctrl.h"
#include "hal/uart_ll.h"

void uart_set_wakeup_threshold(uart_t uart, uint32_t threshold)
{
    assert(uart < SOC_UART_NUM);

    HP_UART_PAD_CLK_ATOMIC() {
        uart_ll_set_wakeup_thrd(UART_LL_GET_HW(uart), threshold);
        uart_ll_enable_pad_sleep_clock(UART_LL_GET_HW(uart), true);
    }
}

esp_err_t uart_wait_tx_idle_polling(uart_t uart)
{
    if (uart_ll_is_enabled(uart)) {
        while (!uart_ll_is_tx_idle(UART_LL_GET_HW(uart))) { }
    }
    return ESP_OK;
}
