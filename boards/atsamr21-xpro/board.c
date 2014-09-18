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
    PM->APBAMASK.reg |= PM_APBAMASK_GCLK;


    SYSCTRL->OSC8M.bit.PRESC = 0;
    SYSCTRL->OSC8M.bit.ONDEMAND = 1;
    SYSCTRL->OSC8M.bit.RUNSTDBY = 0;
    SYSCTRL->OSC8M.bit.ENABLE = 1;

    GCLK->CTRL.reg = GCLK_CTRL_SWRST;
    while (GCLK->CTRL.reg & GCLK_CTRL_SWRST);
    /* disable the watchdog timer */
    WDT->CTRL.bit.ENABLE = 0;
    /* GCLK setup */

    /*Set up main clock generator */
    GCLK_GENDIV_Type gendiv =
    {
        .bit.ID = 0,
        .bit.DIV = 0   
    };
    GCLK->GENDIV = gendiv;
    while (GCLK->STATUS.bit.SYNCBUSY);
    
    GCLK_GENCTRL_Type genctrl = {
        .bit.ID = 0, //Generator 0, TODO: is this correct
        .bit.SRC = GCLK_SOURCE_OSC8M,
        .bit.GENEN = true,
        .bit.IDC = 0,
        .bit.OOV = 0,
        .bit.DIVSEL = 0,
        .bit.RUNSTDBY = 0
    };
    GCLK->GENCTRL = genctrl;

    while(GCLK->STATUS.bit.SYNCBUSY);
}
