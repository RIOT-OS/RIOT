/*
 * SPDX-FileCopyrightText: 2025 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stm32c0116-dk
 * @{
 *
 * @file        board.c
 * @brief       Board specific code for the STM32C0116-DK
 *
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"

void board_init(void)
{
    /* The STM32C011x6 does not actually have the PA9 and PA10 pins bonded to
     * a dedicated pad. Instead they are shared with PA11 and PA12, so we
     * have to remap them to use USART1. This is the USART that is connected
     * to the on-board ST-Link V2.
     */

    /* enable the SYSCFG register */
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;

    /* Remap PA10 and PA9 to PA12 and PA11 which are actually bonded
     * to the hardware pins. */
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA12_RMP | SYSCFG_CFGR1_PA11_RMP;
}
