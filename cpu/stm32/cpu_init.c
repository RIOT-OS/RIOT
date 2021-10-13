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
 * @ingroup     cpu_stm32
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
 * @author      Francisco Molina <francisco.molina@inria.cl>
 *
 * @}
 */

#include "cpu.h"
#include "stdio_base.h"
#include "stmclk.h"
#include "periph_cpu.h"
#include "periph/init.h"
#include "periph/gpio.h"
#include "board.h"

#if defined (CPU_FAM_STM32L4) || defined (CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5)
#define BIT_APB_PWREN       RCC_APB1ENR1_PWREN
#elif defined (CPU_FAM_STM32G0)
#define BIT_APB_PWREN       RCC_APBENR1_PWREN
#elif !defined(CPU_FAM_STM32MP1)
#define BIT_APB_PWREN       RCC_APB1ENR_PWREN
#endif

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32L1)

#define STM32_CPU_MAX_GPIOS    (12U)

#if defined(CPU_FAM_STM32L1)
#define GPIO_CLK              (AHB)
#define GPIO_CLK_ENR          (RCC->AHBENR)
#define GPIO_CLK_ENR_MASK     (0x0000FFFF)
#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
#define GPIO_CLK              (AHB)
#define GPIO_CLK_ENR          (RCC->AHBENR)
#define GPIO_CLK_ENR_MASK     (0xFFFF0000)
#elif defined(CPU_FAM_STM32WL)
#define GPIO_CLK              (AHB2)
#define GPIO_CLK_ENR          (RCC->AHB2ENR)
#define GPIO_CLK_ENR_MASK     (0x00000087)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7)
#define GPIO_CLK              (AHB1)
#define GPIO_CLK_ENR          (RCC->AHB1ENR)
#define GPIO_CLK_ENR_MASK     (0x0000FFFF)
#elif defined(CPU_FAM_STM32F1)
#define GPIO_CLK              (APB2)
#define GPIO_CLK_ENR          (RCC->APB2ENR)
#define GPIO_CLK_ENR_MASK     (0x000001FC)
#endif

#ifndef DISABLE_JTAG
#define DISABLE_JTAG 0
#endif

/**
 * @brief   Initialize gpio to AIN
 *
 * stm32f need to have all there pins initialized to AIN so the consumption
 * of the input Schmitt trigger is saved when running in STOP mode.
 *
 * @see https://comm.eefocus.com/media/download/index/id-1013834
 */
static void _gpio_init_ain(void)
{
    uint32_t ahb_gpio_clocks;

    /* enable GPIO clock and save GPIO clock configuration */
    ahb_gpio_clocks = GPIO_CLK_ENR & GPIO_CLK_ENR_MASK;
    periph_clk_en(GPIO_CLK, GPIO_CLK_ENR_MASK);

    /* switch all GPIOs to AIN mode to minimize power consumption */
    for (uint8_t i = 0; i < STM32_CPU_MAX_GPIOS; i++) {
        GPIO_TypeDef *port;
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));
        if (IS_GPIO_ALL_INSTANCE(port)) {
            if (!DISABLE_JTAG) {
#if defined(CPU_FAM_STM32F1)
                switch (i) {
                    /* preserve JTAG pins on PORTA and PORTB */
                    case 0:
                        port->CRL = GPIO_CRL_CNF;
                        port->CRH = GPIO_CRH_CNF & 0x000FFFFF;
                        break;
                    case 1:
                        port->CRL = GPIO_CRL_CNF & 0xFFF00FFF;
                        port->CRH = GPIO_CRH_CNF;
                        break;
                    default:
                        port->CRL = GPIO_CRL_CNF;
                        port->CRH = GPIO_CRH_CNF;
                        break;
                }
#else /* ! defined(CPU_FAM_STM32F1) */
                switch (i) {
                    /* preserve JTAG pins on PORTA and PORTB */
                    case 0:
                        port->MODER = 0xABFFFFFF;
                        break;
                    case 1:
                        port->MODER = 0xFFFFFEBF;
                        break;
                    default:
                        port->MODER = 0xFFFFFFFF;
                        break;
                }
#endif /* defined(CPU_FAM_STM32F1) */
            }
            else {
#if defined(CPU_FAM_STM32F1)
                port->CRL = GPIO_CRL_CNF;
                port->CRH = GPIO_CRH_CNF;
#else
                port->MODER = 0xFFFFFFFF;
#endif
            }
        }
    }

    /* restore GPIO clocks */
    periph_clk_en(GPIO_CLK, ahb_gpio_clocks);
}
#endif

/**
 * @brief   Initialize HW debug pins for Sub-GHz Radio
 */
void _wlx5xx_init_subghz_debug_pins(void)
{
#if IS_ACTIVE(CONFIG_STM32_WLX5XX_SUBGHZ_DEBUG)
    /* SUBGHZSPI Debug */
    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT_AF);

    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT_AF);

    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT_AF);

    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT_AF);

    /* Sub-GHz Radio Debug */
    gpio_init(CPU_STM32WL_SUBGHZ_RF_BUSY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_RF_BUSY,
                 CPU_STM32WL_SUBGHZ_RF_BUSY_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY_AF);
#endif
}

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* enable PWR module */
#if !defined(CPU_FAM_STM32WB) && !defined(CPU_FAM_STM32MP1) &&  \
    !defined(CPU_FAM_STM32WL)
    periph_clk_en(APB1, BIT_APB_PWREN);
#endif
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32L1)
    _gpio_init_ain();
#endif
#if !defined(CPU_FAM_STM32MP1) || IS_USED(MODULE_STM32MP1_ENG_MODE)
    /* initialize the system clock as configured in the periph_conf.h */
    stmclk_init_sysclk();
#endif
#ifdef MODULE_PERIPH_DMA
    /*  initialize DMA streams */
    dma_init();
#endif
    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

#ifdef STM32F1_DISABLE_JTAG
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
#endif

    /* trigger static peripheral initialization */
    periph_init();

    if (IS_ACTIVE(CONFIG_STM32_WLX5XX_SUBGHZ_DEBUG)) {
        _wlx5xx_init_subghz_debug_pins();
    }
}
