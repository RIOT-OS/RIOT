/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Implementation of the kernel cpu functions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_cpu_common.h"
#include "periph/init.h"

void cpu_init(void)
{
    uint32_t ahb_gpio_clocks;
    uint32_t tmpreg;

    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize system clocks */
    stmclk_init_sysclk();

    /* determine available ports */
    uint32_t cpu_ports_number = 0;
    for (uint32_t i = GPIOA_BASE; i<=GPIOG_BASE; i += (GPIOB_BASE - GPIOA_BASE)) {
        if (cpu_check_address((char *)i)) {
            cpu_ports_number++;
        }
        else {
            break;
        }
    }

    /* save GPIO clock configuration */
    ahb_gpio_clocks = RCC->AHBENR & 0xFF;
    /* enable all GPIO clocks */
    periph_clk_en(AHB, 0xFF);

    /* switch all GPIOs to AIN mode to minimize power consumption */
    GPIO_TypeDef *port;
    for (uint8_t i = 0; i < cpu_ports_number; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));
        port->MODER = 0xffffffff;
    }

    /* restore GPIO clocks */
    tmpreg = RCC->AHBENR;
    tmpreg &= ~((uint32_t)0xFF);
    tmpreg |= ahb_gpio_clocks;
    periph_clk_en(AHB, tmpreg);

    /* trigger static peripheral initialization */
    periph_init();
}
