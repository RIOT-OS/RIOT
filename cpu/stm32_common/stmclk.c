/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) \
    || defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7)

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* make sure we have all needed information about the clock configuration */
#ifndef CLOCK_HSE
#error "Please provide CLOCK_HSE in your board's perhip_conf.h"
#endif
#ifndef CLOCK_LSE
#error "Please provide CLOCK_LSE in your board's periph_conf.h"
#endif
#ifndef CLOCK_CORECLOCK
#error "Please provide CLOCK_CORECLOCK in your board's periph_conf.h"
#endif

/**
 * @name    PLL configuration
 * @{
 */
#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7)
/* figure out which input to use */
#if (CLOCK_HSE)
#define PLL_SRC                  RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_SRC                  RCC_PLLCFGR_PLLSRC_HSI
#endif

#if defined(CPU_FAM_STM32F2)
#define RCC_PLLCFGR_PLLP_Pos       (16U)
#define RCC_PLLCFGR_PLLM_Pos       (0U)
#define RCC_PLLCFGR_PLLN_Pos       (6U)
#define RCC_PLLCFGR_PLLQ_Pos       (24U)

#define RCC_PLLI2SCFGR_PLLI2SN_Pos (6U)
#define RCC_PLLI2SCFGR_PLLI2SR_Pos (28U)
#endif

#if (CLOCK_ENABLE_PLL_I2S)
#ifdef RCC_PLLI2SCFGR_PLLI2SM_Pos
#define PLLI2S_M                 (CLOCK_PLL_I2S_M << RCC_PLLI2SCFGR_PLLI2SM_Pos)
#else
#define PLLI2S_M                 (0)
#endif
#define PLLI2S_N                 (CLOCK_PLL_I2S_N << RCC_PLLI2SCFGR_PLLI2SN_Pos)
#ifdef RCC_PLLI2SCFGR_PLLI2SP_Pos
#define PLLI2S_P                 (((CLOCK_PLL_I2S_P / 2) - 1) << RCC_PLLI2SCFGR_PLLI2SP_Pos)
#else
#define PLLI2S_P                 (0)
#endif
#ifdef RCC_PLLI2SCFGR_PLLI2SQ_Pos
#define PLLI2S_Q                 (CLOCK_PLL_I2S_Q << RCC_PLLI2SCFGR_PLLI2SQ_Pos)
#else
#define PLLI2S_Q                 (0)
#endif
#if defined(RCC_PLLI2SCFGR_PLLI2SR_Pos) && defined(CLOCK_PLL_I2S_R)
#define PLLI2S_R                 (CLOCK_PLL_I2S_R << RCC_PLLI2SCFGR_PLLI2SR_Pos)
#else
#define PLLI2S_R                 (0)
#endif
#endif /* CLOCK_ENABLE_PLLI_2S */

#if (CLOCK_ENABLE_PLL_SAI)
#ifdef RCC_PLLSAICFGR_PLLSAIN_Pos
#define PLLSAI_M                 (CLOCK_PLL_SAI_M << RCC_PLLSAICFGR_PLLSAIN_Pos)
#else
#define PLLSAI_M                 (0)
#endif
#define PLLSAI_N                 (CLOCK_PLL_SAI_N << RCC_PLLSAICFGR_PLLSAIN_Pos)
#ifdef RCC_PLLSAICFGR_PLLSAIP_Pos
#define PLLSAI_P                 (((CLOCK_PLL_SAI_P / 2) - 1) << RCC_PLLSAICFGR_PLLSAIP_Pos)
#else
#define PLLSAI_P                 (0)
#endif
#define PLLSAI_Q                 (CLOCK_PLL_SAI_Q << RCC_PLLSAICFGR_PLLSAIQ_Pos)
#if defined(RCC_PLLSAICFGR_PLLSAIR_Pos) && defined(CLOCK_PLL_SAI_R)
#define PLLSAI_R                 (CLOCK_PLL_SAI_R << RCC_PLLSAICFGR_PLLSAIR_Pos)
#else
#define PLLSAI_R                 (0)
#endif
#endif /* CLOCK_ENABLE_PLL_SAI */

/* now we get the actual bitfields */
#define PLL_P                   (((CLOCK_PLL_P / 2) - 1) << RCC_PLLCFGR_PLLP_Pos)
#define PLL_M                   (CLOCK_PLL_M << RCC_PLLCFGR_PLLM_Pos)
#define PLL_N                   (CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)
#define PLL_Q                   (CLOCK_PLL_Q << RCC_PLLCFGR_PLLQ_Pos)
#if defined(RCC_PLLCFGR_PLLR_Pos) && defined(CLOCK_PLL_R)
#define PLL_R                   (CLOCK_PLL_R << RCC_PLLCFGR_PLLR_Pos)
#else
#define PLL_R                   (0)
#endif

#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
#if (CLOCK_HSE)
#define PLL_SRC                 (RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1)
#else
#define PLL_SRC                 (RCC_CFGR_PLLSRC_HSI_DIV2)
#endif

#define PLL_MUL                 ((CLOCK_PLL_MUL - 2) << 18)
#define PLL_PREDIV              (CLOCK_PLL_PREDIV - 1)

#if defined(CPU_FAM_STM32F0)
#define CLOCK_APB2_DIV          (0)
#endif

#elif defined(CPU_FAM_STM32F1)
#if CLOCK_HSE
#define PLL_SRC                 (RCC_CFGR_PLLSRC) /* HSE */
#else
#define PLL_SRC                 (0) /* HSI / 2 */
#endif

#define PLL_MUL                 ((CLOCK_PLL_MUL - 2) << 18)
#define PLL_PREDIV              (CLOCK_PLL_PREDIV - 1)

#define RCC_CR_HSITRIM_4        (1 << 7)
#define RCC_CFGR_PLLMUL         RCC_CFGR_PLLMULL
#endif
/** @} */

/**
 * @name    Deduct the needed flash wait states from the core clock frequency
 * @{
 */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || defined(STM32F3)
#define FLASH_WAITSTATES        ((CLOCK_CORECLOCK - 1) / 24000000U)
#else
#define FLASH_WAITSTATES        (CLOCK_CORECLOCK / 30000000U)
#endif
/* we enable I+D cashes, pre-fetch, and we set the actual number of
 * needed flash wait states */
#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
#define FLASH_ACR_CONFIG        (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#elif defined(CPU_FAM_STM32F7)
#define FLASH_ACR_CONFIG        (FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F3)
#define FLASH_ACR_CONFIG        (FLASH_ACR_PRFTBE | FLASH_WAITSTATES)
#endif
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
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV |
                 CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* Flash config */
    FLASH->ACR = FLASH_ACR_CONFIG;

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = (RCC_CR_HSION | RCC_CR_HSITRIM_4);

#if (CLOCK_MCO1_SRC)
#ifndef RCC_CFGR_MCO1
#error "stmclk: no MCO1 on this device"
#endif
    RCC->CFGR |= CLOCK_MCO1_SRC | CLOCK_MCO1_PRE;
#endif
#if (CLOCK_MCO2_SRC)
#ifndef RCC_CFGR_MCO2
#error "stmclk: no MCO2 on this device"
#endif
    RCC->CFGR |= CLOCK_MCO2_SRC | CLOCK_MCO2_PRE;
#endif

    /* if configured, we need to enable the HSE clock now */
#if (CLOCK_HSE)
    RCC->CR |= (RCC_CR_HSEON);
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif

#if CLOCK_USE_ALT_48MHZ
    RCC->DCKCFGR2 |= RCC_DCKCFGR2_CK48MSEL;
#endif
    /* now we can safely configure and start the PLL */
#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7)
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_P | PLL_Q | PLL_R);
#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F3)
    /* reset PLL configuration bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
    /* set PLL configuration */
    RCC->CFGR |= PLL_SRC | PLL_MUL;
#if CLOCK_PLL_PREDIV == 2
    RCC->CFGR |= RCC_CFGR_PLLXTPRE; /* PREDIV == 2 */
#elif CLOCK_PLL_PREDIV > 2
    RCC->CFGR2 = PLL_PREDIV;        /* PREDIV > 2 */
#endif
#endif
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    /* now that the PLL is running, we use it as system clock */
    RCC->CFGR |= (RCC_CFGR_SW_PLL);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    stmclk_disable_hsi();

#if (CLOCK_ENABLE_PLL_I2S)
    RCC->PLLI2SCFGR = (CLOCK_PLL_I2S_SRC | PLLI2S_M | PLLI2S_N | PLLI2S_P | PLLI2S_Q | PLLI2S_R);
    RCC->CR |= (RCC_CR_PLLI2SON);
    while (!(RCC->CR & RCC_CR_PLLI2SRDY)) {}
#endif /* CLOCK_ENABLE_PLLI2S */

#if (CLOCK_ENABLE_PLL_SAI)
    RCC->PLLSAICFGR = (PLLSAI_M | PLLSAI_N | PLLSAI_P | PLLSAI_Q | PLLSAI_R);
    RCC->CR |= (RCC_CR_PLLSAION);
    while (!(RCC->CR & RCC_CR_PLLSAIRDY)) {}
#endif

    irq_restore(is);
}
#else
typedef int dont_be_pedantic;
#endif /* defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) ||
        * defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F3) ||
        * defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) */
