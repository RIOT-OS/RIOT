/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for EFM32 specific features.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "periph/uart.h"

int main(void)
{
    /* test if uart_config[i].mode is set to a know value */
    for (unsigned i = 0; i < UART_NUMOF; i++) {
        printf("UART %u mode: ", i);

        switch (uart_config[i].mode) {
            case UART_MODE_8N1:
                puts("8N1");
                break;
            case UART_MODE_8E1:
                puts("8E1");
                break;
            default:
                puts("unknown");
        }
    }

    return 0;
}
