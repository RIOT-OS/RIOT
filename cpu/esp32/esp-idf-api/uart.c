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

#include "esp_private/uart_share_hw_ctrl.h"
#include "esp_rom_uart.h"
#include "hal/uart_ll.h"

#include "esp_idf_api/uart.h"

void esp_idf_uart_set_wakeup_threshold(unsigned uart_num, uint32_t threshold)
{
    assert(uart_num < SOC_UART_NUM);

    HP_UART_PAD_CLK_ATOMIC() {
        uart_ll_set_wakeup_thrd(UART_LL_GET_HW(uart_num), threshold);
        uart_ll_enable_pad_sleep_clock(UART_LL_GET_HW(uart_num), true);
    }
}

void esp_idf_esp_rom_output_tx_wait_idle(unsigned uart_num)
{
    esp_rom_output_tx_wait_idle(uart_num);
}
