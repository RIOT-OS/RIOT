/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_opencm904
 * @{
 *
 * @file
 * @brief       Board specific implementations for the OpenCM9.04 board
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "log.h"

void board_init(void)
{

    /* disable bootloader's USB */
    RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;

    /* disable bootloader's TIMER update interrupt */
    TIM2->DIER &= ~(TIM_DIER_UIE);

    /* remap USART1 to PB7 and PB6 */
    AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
}
