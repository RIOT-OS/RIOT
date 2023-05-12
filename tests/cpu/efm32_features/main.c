/*
 * Copyright (C) 2018-2019 Bas Stottelaar <basstottelaar@gmail.com>
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
