/*
 * Copyright (C) 2020 Savoir-faire Linux
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
 * @brief       Implementation of STM32 clock configuration for STM32MP1
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* PLL configuration */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_SRC                     RCC_RCK3SELR_PLL3SRC_1
#else
#define PLL_SRC                     RCC_RCK3SELR_PLL3SRC_0
#endif

/* Compute the bitfields for the PLL configuration */
#define PLL_P                       (((CONFIG_CLOCK_PLL_P / 2)) \
        << RCC_PLL3CFGR2_DIVP_Pos)
#define PLL_M                       ((CONFIG_CLOCK_PLL_M)       \
        << RCC_PLL3CFGR1_DIVM3_Pos)
#define PLL_N                       ((CONFIG_CLOCK_PLL_N)       \
        << RCC_PLL3CFGR1_DIVN_Pos)
#define PLL_Q                       ((CONFIG_CLOCK_PLL_Q)       \
        << RCC_PLL3CFGR2_DIVQ_Pos)
#if defined(RCC_PLL3CFGR2_DIVR) && defined(CONFIG_CLOCK_PLL_R)
#define PLL_R                       ((CONFIG_CLOCK_PLL_R)       \
        << RCC_PLL3CFGR2_DIVR_Pos)
#else
#define PLL_R                       (0)
#endif

/* Configure HLCK and PCLK prescalers */
#if CONFIG_CLOCK_MCU_DIV == 1
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_0)
#elif CONFIG_CLOCK_MCU_DIV == 2
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_1)
#elif CONFIG_CLOCK_MCU_DIV == 4
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_2)
#elif CONFIG_CLOCK_MCU_DIV == 8
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_3)
#elif CONFIG_CLOCK_MCU_DIV == 16
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_4)
#elif CONFIG_CLOCK_MCU_DIV == 32
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_5)
#elif CONFIG_CLOCK_MCU_DIV == 64
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_6)
#elif CONFIG_CLOCK_MCU_DIV == 128
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_7)
#elif CONFIG_CLOCK_MCU_DIV == 256
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_8)
#elif CONFIG_CLOCK_MCU_DIV == 512
#define CLOCK_MCU_DIV              (RCC_MCUDIVR_MCUDIV_9)
#else
#error "Invalid MCU prescaler value (only 1, 2, 4, 8, 16, 32, 64, 128, 256 \
and 512 allowed)"
#endif

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (RCC_APB1DIVR_APB1DIV_0)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_APB1DIVR_APB1DIV_1)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_APB1DIVR_APB1DIV_2)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_APB1DIVR_APB1DIV_3)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_APB1DIVR_APB1DIV_4)
#else
#error "Invalid APB1 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#define CLOCK_APB2_DIV              (RCC_APB2DIVR_APB2DIV_0)
#elif CONFIG_CLOCK_APB2_DIV == 2
#define CLOCK_APB2_DIV              (RCC_APB2DIVR_APB2DIV_1)
#elif CONFIG_CLOCK_APB2_DIV == 4
#define CLOCK_APB2_DIV              (RCC_APB2DIVR_APB2DIV_2)
#elif CONFIG_CLOCK_APB2_DIV == 8
#define CLOCK_APB2_DIV              (RCC_APB2DIVR_APB2DIV_3)
#elif CONFIG_CLOCK_APB2_DIV == 16
#define CLOCK_APB2_DIV              (RCC_APB2DIVR_APB2DIV_4)
#else
#error "Invalid APB2 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

#if CONFIG_CLOCK_APB3_DIV == 1
#define CLOCK_APB3_DIV              (RCC_APB3DIVR_APB3DIV_0)
#elif CONFIG_CLOCK_APB3_DIV == 2
#define CLOCK_APB3_DIV              (RCC_APB3DIVR_APB3DIV_1)
#elif CONFIG_CLOCK_APB3_DIV == 4
#define CLOCK_APB3_DIV              (RCC_APB3DIVR_APB3DIV_2)
#elif CONFIG_CLOCK_APB3_DIV == 8
#define CLOCK_APB3_DIV              (RCC_APB3DIVR_APB3DIV_3)
#elif CONFIG_CLOCK_APB3_DIV == 16
#define CLOCK_APB3_DIV              (RCC_APB3DIVR_APB3DIV_4)
#else
#error "Invalid APB3 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

/* Check whether PLL must be enabled:
  - When PLL is used as SYSCLK
  - When PLLQ is required
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLQ)
#define CONFIG_CLOCK_USE_PLL                1
#else
#define CONFIG_CLOCK_USE_PLL                0
#endif

/* Check whether HSE must be enabled:
  - When HSE is used as SYSCLK
  - When PLL is used as SYSCLK and the board provides HSE (since HSE will be
    used as PLL input clock)
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && IS_ACTIVE(CONFIG_CLOCK_USE_PLL))
#define CONFIG_CLOCK_USE_HSE                1
#else
#define CONFIG_CLOCK_USE_HSE                0
#endif

void stmclk_enable_hsi(void)
{
    RCC->OCENSETR |= RCC_OCENSETR_HSION;
    while (!(RCC->OCRDYR & RCC_OCRDYR_HSIRDY)) {}
}

static void stmclk_enable_hse(void)
{
    RCC->OCENSETR |= RCC_OCENSETR_HSEON;
    while (!(RCC->OCRDYR & RCC_OCRDYR_HSERDY)) {}
}

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->MC_CIFR = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configured by the board */
    /* MCUDIV */
    RCC->MCUDIVR =  CLOCK_MCU_DIV;
    while ((RCC->MCUDIVR & RCC_MCUDIVR_MCUDIVRDY)
            != RCC_MCUDIVR_MCUDIVRDY) {}
    /* APB1DIV */
    RCC->APB1DIVR =  CLOCK_APB1_DIV;
    while ((RCC->APB1DIVR & RCC_APB1DIVR_APB1DIVRDY)
            != RCC_APB1DIVR_APB1DIVRDY) {}
    /* APB2DIV */
    RCC->APB2DIVR =  CLOCK_APB2_DIV;
    while ((RCC->APB2DIVR & RCC_APB2DIVR_APB2DIVRDY)
            != RCC_APB2DIVR_APB2DIVRDY) {}
    /* APB3DIV */
    RCC->APB3DIVR =  CLOCK_APB3_DIV;
    while ((RCC->APB3DIVR & RCC_APB3DIVR_APB3DIVRDY)
            != RCC_APB3DIVR_APB3DIVRDY) {}

    /* MCU clock source */
    RCC->MSSCKSELR = RCC_MSSCKSELR_MCUSSRC_0; /* HSI */
    while ((RCC->MSSCKSELR & RCC_MSSCKSELR_MCUSSRCRDY)
            != RCC_MSSCKSELR_MCUSSRCRDY) {}

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->OCENCLRR = ~(RCC_OCENSETR_HSION);

    /* if configured, we need to enable the HSE clock now */
    if (IS_ACTIVE(CONFIG_CLOCK_USE_HSE)) {
        stmclk_enable_hse();
    }

    if (IS_ACTIVE(CONFIG_CLOCK_USE_PLL)) {
        /* now we can safely configure the PLL */
        RCC->PLL3CFGR1 = (PLL_M | PLL_N);
        RCC->PLL3CFGR2 = (PLL_P | PLL_Q | PLL_R);

        RCC->RCK3SELR |= PLL_SRC;
        while (!(RCC->RCK3SELR & RCC_RCK3SELR_PLL3SRCRDY)) {}

        /* and start the PLL */
        RCC->PLL3CR |= (RCC_PLL3CR_DIVPEN | RCC_PLL3CR_DIVQEN
                | RCC_PLL3CR_DIVREN | RCC_PLL3CR_PLLON);
        while (!(RCC->PLL3CR & RCC_PLL3CR_PLL3RDY)) {}
    }

    /* Configure SYSCLK */
    if (IS_ACTIVE(CONFIG_CLOCK_USE_PLL)) {
        RCC->MSSCKSELR = RCC_MSSCKSELR_MCUSSRC_3; /* PLL3 */
    }
    else if (IS_ACTIVE(CONFIG_CLOCK_USE_HSE)) {
        RCC->MSSCKSELR = RCC_MSSCKSELR_MCUSSRC_1; /* HSE */
    }
    else {
        RCC->MSSCKSELR = RCC_MSSCKSELR_MCUSSRC_0; /* HSI by default */
    }
    /* Wait SYSCLK to be ready */
    while (!(RCC->MSSCKSELR & RCC_MSSCKSELR_MCUSSRCRDY)) {}

    irq_restore(is);
}
