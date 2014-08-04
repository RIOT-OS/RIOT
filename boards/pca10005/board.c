/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_pca10005
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the nRF51822 evaluation board pca10005
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "cpu.h"
#include "nrf51.h"
#include "periph/uart.h"
#include "periph/gpio.h"

extern void SystemInit(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
}
