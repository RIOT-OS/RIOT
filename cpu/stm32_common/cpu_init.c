/*
 * Copyright (C) 2013 INRIA
 *               2014 Freie Universität Berlin
 *               2016 TriaGnoSys GmbH
 *               2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 OTA keys S.A.
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel cpu functions
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Víctor Ariño <victor.arino@zii.aero>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Oleg Artamonov <oleg@unwds.com>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_cpu.h"
#include "periph/init.h"

#if defined (CPU_FAM_STM32L4)
#define BIT_APB_PWREN       RCC_APB1ENR1_PWREN
#else
#define BIT_APB_PWREN       RCC_APB1ENR_PWREN
#endif

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* enable PWR module */
    periph_clk_en(APB1, BIT_APB_PWREN);
    /* initialize the system clock as configured in the periph_conf.h */
    stmclk_init_sysclk();

#if defined(CPU_FAM_STM32L1)
    uint32_t ahb_gpio_clocks;
    GPIO_TypeDef *port;

    /* enable GPIO clock and save GPIO clock configuration */
    ahb_gpio_clocks = RCC->AHBENR & 0xFF;
    periph_clk_en(AHB, 0xFF);

    /* switch all GPIOs to AIN mode to minimize power consumption */
    /* can't be more than 12 ports on STM32L1 */
    for (int i = 0; i < 12; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));
        if (IS_GPIO_ALL_INSTANCE(port))
            port->MODER = 0xffffffff;
        else
            break;
        }
    /* restore GPIO clock */
    periph_clk_en(AHB, ((RCC->AHBENR & ~((uint32_t)0xFF)) | ahb_gpio_clocks));
    }
#endif
#ifdef MODULE_PERIPH_DMA
    /*  initialize DMA streams */
    dma_init();
#endif
    /* trigger static peripheral initialization */
    periph_init();
}
