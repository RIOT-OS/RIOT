/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f103
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo-f103 board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();

    /* pin remapping: in order to use the MCU peripherals with the Arduino
    * compatible connectors. Some peripherals need to be remapped here. */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The green LED is connected to pin PA5
 */
static void leds_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CR[0] &= ~(0x0f << 20);
    GPIOA->CR[0] |= (0x03 << 20);
    GPIOA->BRR = (1 << 5);
}
