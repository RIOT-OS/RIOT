/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_iot-lab_M3
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the iot-lab_M3 board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de
 *
 * @}
 */

#include "cmsis_system.h"
#include "board.h"
#include "cpu.h"


static void leds_init(void);


void board_init(void)
{
    /* initialize core clocks via CMSIS function provided by ST */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LEDs initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 * The LEDs are connected to the following pin:
 * - Green:     PB5
 * - Orange:    PC10
 * - Red:       PD2
 */
static void leds_init(void)
{
    /* green pin */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRL = (0x3 << (5*4));

    /* orange pin */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH = (0x3 << ((10-8)*4));

    /* red pin */
    RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    GPIOD->CRL = (0x3 << (2*4));
}
