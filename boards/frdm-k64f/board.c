/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-k64f
 * @{
 *
 * @file
 * @brief       Board specific implementations for the FRDM-K64F
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include "board.h"
#include "mcg.h"
#include "periph/gpio.h"

#define SIM_CLKDIV1_60MHZ      (SIM_CLKDIV1_OUTDIV1(0) | \
                                SIM_CLKDIV1_OUTDIV2(0) | \
                                SIM_CLKDIV1_OUTDIV3(1) | \
                                SIM_CLKDIV1_OUTDIV4(2))

#define SIM_CLKDIV1_SAFE       (SIM_CLKDIV1_OUTDIV1(0) | \
                                SIM_CLKDIV1_OUTDIV2(1) | \
                                SIM_CLKDIV1_OUTDIV3(1) | \
                                SIM_CLKDIV1_OUTDIV4(3))

static void cpu_clock_init(void);

void board_init(void)
{
    /* initialize the clock system */
    cpu_clock_init();
    /* initialize the CPU core */
    cpu_init();

    /* initialize and turn off the on-board RGB-LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
    gpio_set(LED1_PIN);
    gpio_set(LED2_PIN);
}

/**
 * @brief Configure the controllers clock system
 *
 * | Clock name | Run mode frequency (max) | VLPR mode frequency (max) |
 *
 * | Core       | 120 MHz                  |   4 MHz                   |
 * | System     | 120 MHz                  |   4 MHz                   |
 * | Bus        |  60 MHz                  |   4 MHz                   |
 * | FlexBus    |  50 MHz                  | 800 kHz                   |
 * | Flash      |  25 MHz                  |   4 MHz                   |
 */
static void cpu_clock_init(void)
{
    /* setup system prescalers */
#ifdef MCUBOOT
    SIM->CLKDIV1 = (uint32_t)SIM_CLKDIV1_SAFE;
#else
    SIM->CLKDIV1 = (uint32_t)SIM_CLKDIV1_60MHZ;
#endif

    /* RMII RXCLK */
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[18] &= ~(PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07));

    kinetis_mcg_set_mode(KINETIS_MCG_PEE);
}
