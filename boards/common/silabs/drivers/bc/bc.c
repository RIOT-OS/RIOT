/*
 * SPDX-FileCopyrightText: 2018 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_common_silabs_drivers_bc
 * @{
 *
 * @file
 * @brief       Implementations of the board controller driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "bc.h"

#include "stdio_uart.h"

#include "periph/gpio.h"
#include "periph/uart.h"

/**
 * @brief   Ensure that the BC_PIN is defined
 */
#ifndef BC_PIN
#error "BC_PIN is not defined by the board."
#endif

/**
 * @brief   Ensure that the correct UART is used.
 */
#if ((STDIO_UART_DEV) != (UART_DEV(0)))
#error "The BC requires UART_DEV(0)."
#endif

/**
 * @brief   Ensure that the correct baud rate is used.
 */
#if ((STDIO_UART_BAUDRATE) != 115200)
#error "The BC requires a baud rate of 115200."
#endif

void bc_init(void)
{
    gpio_init(BC_PIN, GPIO_OUT);
    gpio_set(BC_PIN);
}
