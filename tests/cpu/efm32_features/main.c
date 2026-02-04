/*
 * SPDX-FileCopyrightText: 2018-2019 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <stdio.h>

#include "periph/uart.h"

#if EFM32_LEUART_ENABLED
#error "Expected EFM32_LEUART_ENABLED feature to be disabled."
#endif

int main(void)
{
    puts("Board booted, with some EFM32 features enabled or disabled.");

    return 0;
}
