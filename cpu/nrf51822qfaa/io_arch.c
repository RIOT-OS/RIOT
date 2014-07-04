/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f051r8
 * @{
 *
 * @file        io_arch.c
 * @brief       Implementation of the kernel's architecture dependent IO interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "arch/io_arch.h"
#include "periph/uart.h"

int io_arch_puts(char *data, int size)
{
    int i = 0;
    for (; i < size; i++) {
        uart_write_blocking(UART_0, data[i]);
    }
    return i;
}
