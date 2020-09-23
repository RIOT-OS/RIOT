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
#include "periph/gpio.h"

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

#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_IN                      CLOCK_HSE
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_IN                      CLOCK_HSI
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif

#if defined(CPU_FAM_STM32G0)
#define RCC_CFGR_SW_HSI             (0)
#define RCC_CFGR_SW_HSE             (RCC_CFGR_SW_0)
#define RCC_CFGR_SW_PLL             (RCC_CFGR_SW_1)

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

/* Configure MCO */
#ifndef CONFIG_CLOCK_ENABLE_MCO
#define CONFIG_CLOCK_ENABLE_MCO     0   /* Don't enable MCO by default */
#endif

/* Configure the MCO clock source: options are PLLCLK (default), HSE, HSI, LSE, LSI or SYSCLK*/
#ifndef CONFIG_CLOCK_MCO_USE_PLLCLK
#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK)
#define CONFIG_CLOCK_MCO_USE_PLLCLK 0
#else
#define CONFIG_CLOCK_MCO_USE_PLLCLK 1   /* Use PLLCLK by default */
#endif
#endif /* CONFIG_CLOCK_MCO_USE_PLLCLK */

#ifndef CONFIG_CLOCK_MCO_USE_HSE
#define CONFIG_CLOCK_MCO_USE_HSE    0
#endif /* CONFIG_CLOCK_MCO_USE_HSE */

#ifndef CONFIG_CLOCK_MCO_USE_HSI
#define CONFIG_CLOCK_MCO_USE_HSI    0
#endif /* CONFIG_CLOCK_MCO_USE_HSI */

#ifndef CONFIG_CLOCK_MCO_USE_LSE
#define CONFIG_CLOCK_MCO_USE_LSE    0
#endif /* CONFIG_CLOCK_MCO_USE_LSE */

#ifndef CONFIG_CLOCK_MCO_USE_LSI
#define CONFIG_CLOCK_MCO_USE_LSI    0
#endif /* CONFIG_CLOCK_MCO_USE_LSI */

#ifndef CONFIG_CLOCK_MCO_USE_SYSCLK
#define CONFIG_CLOCK_MCO_USE_SYSCLK 0
#endif /* CONFIG_CLOCK_MCO_USE_SYSCLK */

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use PLLCLK as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSE as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use LSE as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use LSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK))
#error "Cannot use SYSCLK as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_1 | RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_1 | RCC_CFGR_MCOSEL_0)
#else
#error "Invalid MCO clock source selection"
#endif

/* Configure the MCO prescaler: valid values are 1, 2, 4, 8, 16 on G4
   and 1, 2, 4, 8, 16, 32, 64, 128 on G0 */
#ifndef CONFIG_CLOCK_MCO_PRE
#define CONFIG_CLOCK_MCO_PRE                    (1)
#endif

/* Define MCO prescalers for G0 for compatibility with G4 */
#if defined(CPU_FAM_STM32G0)
#if CONFIG_CLOCK_MCO_PRE == 1
#define CLOCK_MCO_PRE                           (0)
#elif CONFIG_CLOCK_MCO_PRE == 2
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_0)
#elif CONFIG_CLOCK_MCO_PRE == 4
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_1)
#elif CONFIG_CLOCK_MCO_PRE == 8
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_1 | RCC_CFGR_MCOPRE_0)
#elif CONFIG_CLOCK_MCO_PRE == 16
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_2)
#elif CONFIG_CLOCK_MCO_PRE == 32
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_0)
#elif CONFIG_CLOCK_MCO_PRE == 64
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_1)
#elif CONFIG_CLOCK_MCO_PRE == 128
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_1 | RCC_CFGR_MCOPRE_0)
#else
#error "Invalid MCO prescaler"
#endif
#else /* CPU_FAM_STM32G4 */
#if CONFIG_CLOCK_MCO_PRE == 1
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV1)
#elif CONFIG_CLOCK_MCO_PRE == 2
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV2)
#elif CONFIG_CLOCK_MCO_PRE == 4
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV4)
#elif CONFIG_CLOCK_MCO_PRE == 8
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV8)
#elif CONFIG_CLOCK_MCO_PRE == 16
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV16)
#else
#error "Invalid MCO prescaler"
#endif
#endif

/* Check whether PLL must be enabled:
  - When PLLCLK is used as SYSCLK
  - When PLLCLK is used as MCO clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK))
#define CLOCK_ENABLE_PLL            1
#else
#define CLOCK_ENABLE_PLL            0
#endif

/* Check whether HSE is required:
  - When HSE is used as SYSCLK
  - When PLL is used as SYSCLK and the board provides HSE (since HSE will be
    used as PLL input clock)
  - When HSE is used as MCO clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE))
#define CLOCK_ENABLE_HSE            1
#else
#define CLOCK_ENABLE_HSE            0
#endif

/* Check whether HSI is required:
  - When HSI is used as SYSCLK
  - When PLL is used as SYSCLK and the board doesn't provide HSE (since HSI will be
    used as PLL input clock)
  - When HSI is used as MCO clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    (!IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI))
#define CLOCK_ENABLE_HSI            1
#else
#define CLOCK_ENABLE_HSI            0
#endif

/* Check whether LSE must be enabled */
#if IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE)
#define CLOCK_ENABLE_LSE            1
#else
#define CLOCK_ENABLE_LSE            0
#endif

/* Check whether LSI must be enabled */
#if IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI)
#define CLOCK_ENABLE_LSI            1
#else
#define CLOCK_ENABLE_LSI            0
#endif

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

#if defined(CPU_FAM_STM32G0)
    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSI) && CONFIG_CLOCK_HSISYS_DIV != 1) {
        /* configure HSISYS divider, only available on G0 */
        RCC->CR |= CLOCK_HSI_DIV;
        while (!(RCC->CR & RCC_CR_HSIRDY)) {}
    }
#endif

    /* Enable HSE if required */
    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) {
        RCC->CR |= RCC_CR_HSEON;
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    /* Enable PLL if required */
    if (IS_ACTIVE(CLOCK_ENABLE_PLL)) {
        RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | RCC_PLLCFGR_PLLREN);
        RCC->CR |= RCC_CR_PLLON;
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    }

    /* Configure SYSCLK */
    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
#if defined(CPU_FAM_STM32G0)
        RCC->CFGR = (RCC_CFGR_SW_HSE | CLOCK_AHB_DIV | CLOCK_APB1_DIV);
#elif defined(CPU_FAM_STM32G4)
        RCC->CFGR = (RCC_CFGR_SW_HSE | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
#endif
        RCC->CFGR |= RCC_CFGR_SW_HSE;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
#if defined(CPU_FAM_STM32G4)
        if (CLOCK_AHB > MHZ(80)) {
            /* Divide HCLK by 2 before enabling the PLL */
            RCC->CFGR |= RCC_CFGR_HPRE_DIV2;
        }
#endif

        /* now that the PLL is running, we use it as system clock */
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

#if defined(CPU_FAM_STM32G4)
        if (CLOCK_AHB > MHZ(80)) {
            /* Wait 1us before switching back to full speed */
            /* Use volatile to prevent the compiler from optimizing the loop */
            volatile uint8_t count = CLOCK_CORECLOCK / MHZ(1);
            while (count--) {}
            RCC->CFGR &= ~RCC_CFGR_HPRE_DIV2;
        }
#endif
    }

    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

    /* Enable the LSE if required for MCO
     * If available on the board, LSE might also be initialized for RTT/RTC
     * peripherals. For the monent, this initialization is done in the
     * corresponding peripheral drivers. */
    if (IS_ACTIVE(CLOCK_ENABLE_LSE)) {
        stmclk_dbp_unlock();
        RCC->BDCR |= RCC_BDCR_LSEON;
        while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
        stmclk_dbp_lock();
    }

    /* Enable the LSI if required for MCO
     * If no LSE is available on the board, LSI might also be initialized for
     * RTT/RTC peripherals. For the monent, this initialization is done in the
     * corresponding peripheral drivers. */
    if (IS_ACTIVE(CLOCK_ENABLE_LSI)) {
        RCC->CSR |= RCC_CSR_LSION;
        while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
    }

    /* Configure MCO */
    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO)) {
        /* As stated in the manual, it is highly recommended to change the MCO
           prescaler before enabling the MCO */
        RCC->CFGR |= CLOCK_MCO_PRE;
        RCC->CFGR |= CLOCK_MCO_SRC;

        /* Configure MCO pin (PA8 with AF0) */
        gpio_init(GPIO_PIN(PORT_A, 8), GPIO_OUT);
        gpio_init_af(GPIO_PIN(PORT_A, 8), GPIO_AF0);
    }

#if IS_USED(MODULE_PERIPH_HWRNG)
    /* HWRNG is clocked by HSI48 so enable this clock when the peripheral is used */
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;
    while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY)) {}
#endif

    irq_restore(is);
}
