/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_rfduino-nrf51822
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the RFduino NRF51822 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de> 
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
}
