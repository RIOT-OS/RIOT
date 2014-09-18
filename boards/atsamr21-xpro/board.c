/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_atsamr21-xpro
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Atem SAM R21 Xplained Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"


void led_init(void);
void clk_init(void);


void board_init(void)
{
    /* initialize core clocks via CMSIS function provided by Atmel */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* initialize the GCLK */
    clk_init();

    /* initialize the boards LEDs */
    led_init();
}


/**
 * @brief Initialize the boards on-board LED
 *
 * The LED initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 * The LED is connected to the following pin:
 * - LED: PA19
 */
void led_init(void)
{
    LED_PORT.DIRSET.reg = LED_PIN;
    LED_PORT.OUTSET.reg = LED_PIN;
}

void clk_init(void)
{
    PM->APBAMASK.reg |= PM_APBAMASK_SYSCTRL;
    SYSCTRL->OSC8M.bit.PRESC = 0;
    SYSCTRL->OSC8M.bit.ONDEMAND = 1;
    SYSCTRL->OSC8M.bit.RUNSTDBY = 0;
    SYSCTRL->OSC8M.bit.ENABLE = 1;

    PM->APBAMASK.reg |= PM_APBAMASK_GCLK;
    /* Software reset the module to ensure it is re-initialized correctly */
    GCLK->CTRL.reg = GCLK_CTRL_SWRST;
    while (GCLK->CTRL.reg & GCLK_CTRL_SWRST);

    PM->CPUSEL.reg = (uint32_t)0x0;
    PM->APBASEL.reg = (uint32_t)0x0;
    PM->APBBSEL.reg = (uint32_t)0x0;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    /* Select the correct generator */
    *((uint8_t*)&GCLK->GENDIV.reg) = 0;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    GCLK->GENDIV.reg = 0x00000100;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    *((uint8_t*)&GCLK->GENCTRL.reg) = GCLK_CLKCTRL_GEN_GCLK0;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M);
    // while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    // GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN;
}
