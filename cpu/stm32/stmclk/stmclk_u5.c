/*
 * Copyright (C) 2021 Inria
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
 * @brief       Implementation of STM32 clock configuration for U5 family
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"


#define RCC_CFGR1_SW_MSI            (0x00000000U)
#define RCC_CFGR1_SW_HSI            (RCC_CFGR1_SW_0)
#define RCC_CFGR1_SW_HSE            (RCC_CFGR1_SW_1)
#define RCC_CFGR1_SW_PLL            (RCC_CFGR1_SW_1 | RCC_CFGR1_SW_0)
#define RCC_CFGR1_SWS_MSI           (0x00000000U)
#define RCC_CFGR1_SWS_HSI           (RCC_CFGR1_SWS_0)
#define RCC_CFGR1_SWS_HSE           (RCC_CFGR1_SWS_1)
#define RCC_CFGR1_SWS_PLL           (RCC_CFGR1_SWS_1 | RCC_CFGR1_SWS_0)

/* PLL configuration */
/* figure out which input to use */
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)
#define PLL_SRC                     (RCC_PLL1CFGR_PLL1SRC_0)
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) && IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_SRC                     (RCC_PLL1CFGR_PLL1SRC_1 | RCC_PLL1CFGR_PLL1SRC_0)
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI)
#define PLL_SRC                     (RCC_PLL1CFGR_PLL1SRC_1)
#else
#define PLL_SRC                     0
#endif

/* check configuration and get the corresponding bitfields */
#if (CONFIG_CLOCK_PLL_M < 1 || CONFIG_CLOCK_PLL_M > 16)
#error "PLL configuration: PLL M value is out of range"
#endif
#define PLL_M                       ((CONFIG_CLOCK_PLL_M - 1) << RCC_PLL1CFGR_PLL1M_Pos)

#if (CONFIG_CLOCK_PLL_N < 4 || CONFIG_CLOCK_PLL_N > 512)
#error "PLL configuration: PLL N value is out of range"
#endif
#define PLL_N                       ((CONFIG_CLOCK_PLL_N - 1) << RCC_PLL1DIVR_PLL1N_Pos)

#if (CONFIG_CLOCK_PLL_R < 1 || CONFIG_CLOCK_PLL_R > 128)
#error "PLL configuration: PLL R value is out of range"
#endif
#define PLL_R                       ((CONFIG_CLOCK_PLL_R - 1) << RCC_PLL1DIVR_PLL1R_Pos)

#if (CONFIG_CLOCK_PLL_Q < 1 || CONFIG_CLOCK_PLL_Q > 128)
#error "PLL configuration: PLL Q value is out of range"
#endif
#define PLL_Q                       ((CONFIG_CLOCK_PLL_Q - 1) << RCC_PLL1DIVR_PLL1Q_Pos)

/* Define MSI range bitfields */
#if CONFIG_CLOCK_MSI == KHZ(100)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3 | RCC_ICSCR1_MSISRANGE_2 | RCC_ICSCR1_MSISRANGE_1 | RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == KHZ(133)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3 | RCC_ICSCR1_MSISRANGE_2 | RCC_ICSCR1_MSISRANGE_1)
#elif CONFIG_CLOCK_MSI == KHZ(200)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3 | RCC_ICSCR1_MSISRANGE_2 | RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == KHZ(400)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3 | RCC_ICSCR1_MSISRANGE_2)
#elif CONFIG_CLOCK_MSI == KHZ(768)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3 | RCC_ICSCR1_MSISRANGE_1 | RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(1)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_2 | RCC_ICSCR1_MSISRANGE_1 | RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == KHZ(1024)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3 | RCC_ICSCR1_MSISRANGE_1)
#elif CONFIG_CLOCK_MSI == KHZ(1330)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_2 | RCC_ICSCR1_MSISRANGE_1)
#elif CONFIG_CLOCK_MSI == KHZ(1536)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3 | RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(2)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_2 | RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == KHZ(3072)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_3)
#elif CONFIG_CLOCK_MSI == MHZ(4)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_2)
#elif CONFIG_CLOCK_MSI == MHZ(12)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_1 | RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(16)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_1)
#elif CONFIG_CLOCK_MSI == MHZ(24)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(48)
#define CLOCK_MSIRANGE              (0)
#else
#error "Invalid MSI clock"
#endif

/* Configure 48MHz clock source */
#define CLOCK_PLLQ                  ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_Q

#if CLOCK_PLLQ == MHZ(48)
#define CLOCK48MHZ_USE_PLLQ         1
#elif CONFIG_CLOCK_MSI == MHZ(48)
#define CLOCK48MHZ_USE_MSI          1
#else
#define CLOCK48MHZ_USE_PLLQ         0
#define CLOCK48MHZ_USE_MSI          0
#endif

#if IS_ACTIVE(CLOCK48MHZ_USE_PLLQ)
#define CLOCK48MHZ_SELECT           (RCC_CCIPR1_CLK48MSEL_1)
#elif IS_ACTIVE(CLOCK48MHZ_USE_MSI)
#define CLOCK48MHZ_SELECT           (RCC_CCIPR1_CLK48MSEL_1 | RCC_CCIPR1_CLK48MSEL_0)
#else
#define CLOCK48MHZ_SELECT           (0)
#endif

/* Configure the AHB and APB buses prescalers */
#define CLOCK_AHB_DIV               (0)

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (0)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_CFGR2_PPRE1_2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_0)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_1)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_1 | RCC_CFGR2_PPRE1_0)
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#define CLOCK_APB2_DIV              (0)
#elif CONFIG_CLOCK_APB2_DIV == 2
#define CLOCK_APB2_DIV              (RCC_CFGR2_PPRE2_2)
#elif CONFIG_CLOCK_APB2_DIV == 4
#define CLOCK_APB2_DIV              (RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_0)
#elif CONFIG_CLOCK_APB2_DIV == 8
#define CLOCK_APB2_DIV              (RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_1)
#elif CONFIG_CLOCK_APB2_DIV == 16
#define CLOCK_APB2_DIV              (RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_1 | RCC_CFGR2_PPRE2_0)
#endif

/* Only periph_hwrng requires 48MHz for the moment */
#if IS_USED(MODULE_PERIPH_HWRNG)
#if !IS_ACTIVE(CLOCK48MHZ_USE_PLLQ) && !IS_ACTIVE(CLOCK48MHZ_USE_MSI)
#error "No 48MHz clock source available, HWRNG cannot work"
#endif
#define CLOCK_ENABLE_48MHZ          1
#else
#define CLOCK_ENABLE_48MHZ          0
#endif

/* Check if PLL is required
  - When used as system clock
  - When PLLQ is used as 48MHz clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || \
    (IS_ACTIVE(CLOCK_ENABLE_48MHZ) && IS_ACTIVE(CLOCK48MHZ_USE_PLLQ))
#define CLOCK_ENABLE_PLL            1
#else
#define CLOCK_ENABLE_PLL            0
#endif

/* Check if HSE is required:
  - When used as system clock
  - When used as PLL input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE))
#define CLOCK_ENABLE_HSE            1
#else
#define CLOCK_ENABLE_HSE            0
#endif

/* HSE cannot be enabled if not provided by the board */
#if IS_ACTIVE(CLOCK_ENABLE_HSE) && !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "HSE is required by the clock configuration but is not provided by the board."
#endif

/* Check if HSI is required:
  - When used as system clock
  - When used as PLL input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI))
#define CLOCK_ENABLE_HSI            1
#else
#define CLOCK_ENABLE_HSI            0
#endif

/* Check if MSI is required
  - When used as system clock
  - When used as PLL input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_MSI) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)) || \
    (IS_ACTIVE(CLOCK_ENABLE_48MHZ) && IS_ACTIVE(CLOCK48MHZ_USE_MSI))
#define CLOCK_ENABLE_MSI            1
#else
#define CLOCK_ENABLE_MSI            0
#endif

/* Deduct the needed flash wait states from the core clock frequency */
#if CLOCK_AHB <= MHZ(32)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_0WS
#elif CLOCK_AHB <= MHZ(64)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_1WS
#elif CLOCK_AHB <= MHZ(96)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_2WS
#elif CLOCK_AHB <= MHZ(128)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_3WS
#elif CLOCK_AHB <= MHZ(160)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_4WS
#endif

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIER = 0;

    /* select 1-way cache */
    ICACHE->CR &= ~ICACHE_CR_WAYSEL;
    ICACHE->CR |= ICACHE_CR_EN;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configured by the board */
    RCC->CFGR1 = RCC_CFGR1_SW_HSI;
    while ((RCC->CFGR1 & RCC_CFGR1_SWS) != RCC_CFGR1_SWS_HSI) {}
    RCC->CFGR2 = (CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);

    /* Select the Voltage Range 1 */
    PWR->VOSR = (PWR_VOSR_VOS_1 | PWR_VOSR_VOS_0);
    /* Wait Until the Voltage Regulator is ready */
    while (!(PWR->VOSR & PWR_VOSR_VOSRDY)) {}

    /* Switch to SMPS regulator instead of LDO */
    PWR->CR3 |= PWR_CR3_REGSEL;
    while (!(PWR->SVMSR & PWR_SVMSR_REGS)) {}

    /* configure flash wait states */
    FLASH->ACR = FLASH_WAITSTATES;

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = RCC_CR_HSION;

    /* Enable the HSE clock only when it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    /* Enable the MSI clock only when it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_MSI)) {
        RCC->ICSCR1 = RCC_ICSCR1_MSIRGSEL;
        RCC->ICSCR1 |= CLOCK_MSIRANGE;
        RCC->CR |= (RCC_CR_MSISON | RCC_CR_MSIPLLSEL | RCC_CR_MSIPLLFAST);
        while (!(RCC->CR & RCC_CR_MSISRDY)) {}
    }

    /* Enable the PLL clock only when it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_PLL)) {
        if (IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI) && IS_ACTIVE(CONFIG_BOARD_HAS_LSE)) {
            /* configure the low speed clock domain */
            stmclk_enable_lfclk();
            /* now we can enable the MSI PLL mode to enhance accuracy of the MSI */
            RCC->CR |= RCC_CR_MSIPLLEN;
            while (!(RCC->CR & RCC_CR_MSISRDY)) {}
        }

        /* configure and start the PLL */
        RCC->PLL1CFGR = (PLL_SRC | PLL_M | RCC_PLL1CFGR_PLL1RGE_1 | RCC_PLL1CFGR_PLL1RGE_0);

        if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
            /* Enable PLLCLK if PLL is used as system clock */
            RCC->PLL1CFGR |= RCC_PLL1CFGR_PLL1REN;
        }

        if (IS_ACTIVE(CLOCK48MHZ_USE_PLLQ)) {
            /* Enable PLLQ if PLL is used as 48MHz source clock */
            RCC->PLL1CFGR |= RCC_PLL1CFGR_PLL1QEN;
        }

        RCC->PLL1DIVR = (PLL_N | PLL_R | PLL_Q);
        RCC->PLL1FRACR = 0;
        RCC->CR |= RCC_CR_PLL1ON;
        while (!(RCC->CR & RCC_CR_PLL1RDY)) {}
    }

    /* Configure SYSCLK */
    if (!IS_ACTIVE(CONFIG_USE_CLOCK_HSI)) {
        RCC->CFGR1 &= ~RCC_CFGR1_SW;
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        /* Select HSE as system clock */
        RCC->CFGR1 |= RCC_CFGR1_SW_HSE;
        while ((RCC->CFGR1 & RCC_CFGR1_SWS) != RCC_CFGR1_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_MSI)) {
        /* Select MSI as system clock */
        RCC->CFGR1 |= RCC_CFGR1_SW_MSI;
        while ((RCC->CFGR1 & RCC_CFGR1_SWS) != RCC_CFGR1_SWS_MSI) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
        /* Select main PLL as system clock */
        RCC->CFGR1 |= RCC_CFGR1_SW_PLL;
        while ((RCC->CFGR1 & RCC_CFGR1_SWS) != RCC_CFGR1_SWS_PLL) {}
    }

    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

    if (IS_ACTIVE(CLOCK_ENABLE_48MHZ)) {
        /* configure the clock used for the 48MHz clock tree (USB, RNG) */
        RCC->CCIPR1 = CLOCK48MHZ_SELECT;
    }

    irq_restore(is);
}
