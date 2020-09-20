/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for STM32 F0
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* PLL configuration */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_SRC                 (RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1)
#else
#define PLL_SRC                 (RCC_CFGR_PLLSRC_HSI_DIV2)
#endif

#define PLL_MUL                 ((CONFIG_CLOCK_PLL_MUL - 2) << RCC_CFGR_PLLMUL_Pos)
#define PLL_PREDIV              (CONFIG_CLOCK_PLL_PREDIV - 1)

#define CLOCK_AHB_DIV           (RCC_CFGR_HPRE_DIV1)

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV1)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV4)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV8)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV16)
#else
#error "Invalid APB prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif


/* Deduct the needed flash wait states from the core clock frequency */
#define FLASH_WAITSTATES        ((CLOCK_CORECLOCK - 1) / MHZ(24))
/* we enable I+D cashes, pre-fetch, and we set the actual number of
 * needed flash wait states */
#define FLASH_ACR_CONFIG        (FLASH_ACR_PRFTBE | FLASH_WAITSTATES)
/** @} */

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIR = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* Flash config */
    FLASH->ACR = FLASH_ACR_CONFIG;

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = (RCC_CR_HSION | RCC_CR_HSITRIM_4);

    /* HSE is only used if provided by board and core clock input is using HSE
       or PLL */
    if (IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && !IS_ACTIVE(CONFIG_USE_CLOCK_HSI)) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        RCC->CFGR |= RCC_CFGR_SW_HSE;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
        /* now the PLL can safely be configured and started */
        /* reset PLL configuration bits */
        RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
        /* set PLL configuration */
        RCC->CFGR |= (PLL_SRC | PLL_MUL);
        if (CONFIG_CLOCK_PLL_PREDIV == 2) {
            RCC->CFGR |= RCC_CFGR_PLLXTPRE; /* PREDIV == 2 */
        }
        else if (CONFIG_CLOCK_PLL_PREDIV > 2) {
            RCC->CFGR2 = PLL_PREDIV;        /* PREDIV > 2 */
        }
        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}

        /* now that the PLL is running, use it as system clock */
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
    }

    if (!IS_ACTIVE(CONFIG_USE_CLOCK_HSI) ||
        (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && IS_ACTIVE(CONFIG_BOARD_HAS_HSE))) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

    irq_restore(is);
}
