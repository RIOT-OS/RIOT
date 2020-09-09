/*
 * Copyright (C) 2020 Inria
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
 * @brief       Implementation of STM32 clock configuration for the G0 and G4 families
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

#if defined(CPU_FAM_STM32G0)
#define PLL_M_MIN                   (1)
#define PLL_M_MAX                   (8)
#define PLL_N_MIN                   (8)
#define PLL_N_MAX                   (86)
#define PLL_R_MIN                   (2)
#define PLL_R_MAX                   (8)
#else /* CPu_FAM_STM32G4 */
#define PLL_M_MIN                   (1)
#define PLL_M_MAX                   (16)
#define PLL_N_MIN                   (8)
#define PLL_N_MAX                   (127)
#define PLL_R_MIN                   (1)
#define PLL_R_MAX                   (8)
#endif

#if CONFIG_USE_CLOCK_PLL
#if (CONFIG_CLOCK_PLL_M < PLL_M_MIN || CONFIG_CLOCK_PLL_M > PLL_M_MAX)
#error "PLL configuration: PLL M value is out of range"
#endif
#define PLL_M                       ((CONFIG_CLOCK_PLL_M - 1) << RCC_PLLCFGR_PLLM_Pos)

#if (CONFIG_CLOCK_PLL_N < PLL_N_MIN || CONFIG_CLOCK_PLL_N > PLL_N_MAX)
#error "PLL configuration: PLL N value is out of range"
#endif
#define PLL_N                       (CONFIG_CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)

#if (CONFIG_CLOCK_PLL_R < PLL_R_MIN || CONFIG_CLOCK_PLL_R > PLL_R_MAX)
#error "PLL configuration: PLL R value is out of range"
#endif

#if defined(CPU_FAM_STM32G0)
#define PLL_R                       ((CONFIG_CLOCK_PLL_R - 1) << RCC_PLLCFGR_PLLR_Pos)
#else /* CPU_FAM_STM32G4 */
#define PLL_R                       (((CONFIG_CLOCK_PLL_R >> 1) - 1) << RCC_PLLCFGR_PLLR_Pos)
#endif

#if CONFIG_BOARD_HAS_HSE
#define PLL_IN                      CLOCK_HSE
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_IN                      CLOCK_HSI
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif

#endif /* CONFIG_USE_CLOCK_PLL */

#if defined(CPU_FAM_STM32G0)
#define RCC_CFGR_SW_HSI             (0)
#define RCC_CFGR_SW_HSE             (RCC_CFGR_SW_0)
#define RCC_CFGR_SW_PLL             (RCC_CFGR_SW_1)

#if CONFIG_USE_CLOCK_HSI
#if CONFIG_CLOCK_HSISYS_DIV == 1
#define CLOCK_HSI_DIV               (0)
#elif CONFIG_CLOCK_HSISYS_DIV == 2
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_0)
#elif CONFIG_CLOCK_HSISYS_DIV == 4
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_1)
#elif CONFIG_CLOCK_HSISYS_DIV == 8
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_1 | RCC_CR_HSIDIV_0)
#elif CONFIG_CLOCK_HSISYS_DIV == 16
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2)
#elif CONFIG_CLOCK_HSISYS_DIV == 32
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2 | RCC_CR_HSIDIV_0)
#elif CONFIG_CLOCK_HSISYS_DIV == 64
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2 | RCC_CR_HSIDIV_1)
#elif CONFIG_CLOCK_HSISYS_DIV == 128
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2 | RCC_CR_HSIDIV_1 | RCC_CR_HSIDIV_0)
#endif
#endif /* CONFIG_USE_CLOCK_HSI */

#define CLOCK_AHB_DIV               (0)

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (0)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_0)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1 | RCC_CFGR_PPRE_0)
#endif
#endif /* CPU_FAM_STM32G0 */

#if defined(CPU_FAM_STM32G4)
#define CLOCK_AHB_DIV               (RCC_CFGR_HPRE_DIV1)

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV1)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV4)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV8)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV16)
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV1)
#elif CONFIG_CLOCK_APB2_DIV == 2
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV2)
#elif CONFIG_CLOCK_APB2_DIV == 4
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV4)
#elif CONFIG_CLOCK_APB2_DIV == 8
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV8)
#elif CONFIG_CLOCK_APB2_DIV == 16
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV16)
#endif
#endif /* CPU_FAM_STM32G4 */

/** Determine the required flash wait states from the core clock frequency */
#if defined(CPU_FAM_STM32G0)
#if CLOCK_CORECLOCK >= 48000000
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_1)   /* 2 wait states */
#elif CLOCK_CORECLOCK >= 24000000
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_0)   /* 1 wait states */
#else
#define FLASH_WAITSTATES            (0)                     /* 0 wait states */
#endif
#else /* CPU_FAM_STM32G4 */
#if CLOCK_AHB >= 136
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_4WS) /* 4 ws */
#elif CLOCK_AHB >= 102
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_3WS) /* 3 ws */
#elif CLOCK_AHB >= 68
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_2WS) /* 2 ws */
#elif CLOCK_AHB >= 34
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_1WS) /* 1 ws */
#else
#define FLASH_WAITSTATES            (0)                     /* 0 ws */
#endif
#endif /* CPU_FAM_STM32G4 */

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    RCC->CIER = 0;

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configured by the board */
#if defined(CPU_FAM_STM32G0)
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV);
#elif defined(CPU_FAM_STM32G4)
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
#endif
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

#if defined(CPU_FAM_STM32G0)
    /* we enable instruction cache, pre-fetch, and we set the required flash wait states */
    FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES);
#elif defined(CPU_FAM_STM32G4)
    /* we enable I+D caches, pre-fetch, and we set the actual number of
     * needed flash wait states */
    FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN |
                   FLASH_WAITSTATES);
#endif

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = RCC_CR_HSION;

#if CLOCK_LSE
    stmclk_enable_lfclk();
#endif

#if CONFIG_USE_CLOCK_HSI && defined(CPU_FAM_STM32G0)
    /* configure HSISYS divider, only available on G0 */
    RCC->CR |= CLOCK_HSI_DIV;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}

#elif CONFIG_USE_CLOCK_HSE
    /* if configured, we need to enable the HSE clock now */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}

#if defined(CPU_FAM_STM32G0)
    RCC->CFGR = (RCC_CFGR_SW_HSE | CLOCK_AHB_DIV | CLOCK_APB1_DIV);
#elif defined(CPU_FAM_STM32G4)
    RCC->CFGR = (RCC_CFGR_SW_HSE | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
#endif
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}

#elif CONFIG_USE_CLOCK_PLL
#if CONFIG_BOARD_HAS_HSE
    /* if configured, we need to enable the HSE clock now */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif
    /* now we can safely configure and start the PLL */
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | RCC_PLLCFGR_PLLREN);
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

#if CLOCK_AHB > MHZ(80)
    /* Divide HCLK by before enabling the PLL */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV2;
#endif

    /* now that the PLL is running, we use it as system clock */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

#if CLOCK_AHB > MHZ(80)
    /* Wait 1us before switching back to full speed */
    /* Use volatile to prevent the compiler from optimizing the loop */
    volatile uint8_t count = CLOCK_CORECLOCK / MHZ(1);
    while (count--) {}
    RCC->CFGR &= ~RCC_CFGR_HPRE_DIV2;
#endif

#endif

    stmclk_disable_hsi();
    irq_restore(is);

#ifdef MODULE_PERIPH_HWRNG
    /* HWRNG is clocked by HSI48 so enable this clock when the peripheral is used */
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;
    while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY)) {}
#endif

#ifdef MODULE_PERIPH_RTT
    /* Ensure LPTIM1 clock source (LSI or LSE) is correctly reset when initializing
       the clock, this is particularly useful after waking up from deep sleep */
#if CLOCK_LSE
    RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1;
#else
    RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0;
#endif /* CLOCK_LSE */
#endif /* MODULE_PERIPH_RTT */
}
