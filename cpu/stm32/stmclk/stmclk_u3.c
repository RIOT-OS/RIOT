/*
 
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for U3 family
 *
 * 
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

#if !defined(CPU_FAM_STM32U3)
#error "stmclk_u3.c compiled for non-STM32U3 CPU"
#endif

#define RCC_CFGR1_SW_MSI            (0x00000000U)
#define RCC_CFGR1_SW_HSI            (RCC_CFGR1_SW_0)
#define RCC_CFGR1_SW_HSE            (RCC_CFGR1_SW_1)

#define RCC_CFGR1_SWS_MSI           (0x00000000U)
#define RCC_CFGR1_SWS_HSI           (RCC_CFGR1_SWS_0)
#define RCC_CFGR1_SWS_HSE           (RCC_CFGR1_SWS_1)




/* Define MSI range bitfields for STM32U3 */
/* We assume MSISDIV (bits 29:30) and MSIRANGE (bits 16:18) */

#if CONFIG_CLOCK_MSI == MHZ(48)
#define CLOCK_MSIRANGE              (0) /* MSISRANGE 0, MSISDIV 0 */
#elif CONFIG_CLOCK_MSI == MHZ(24)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISDIV_0) /* Range 0, Div 2 */
#elif CONFIG_CLOCK_MSI == MHZ(16)
/* Note: You need to check if your header defines RCC_ICSCR1_MSISRANGE_0 */
#define CLOCK_MSIRANGE              (0x1UL << 16) 
#elif CONFIG_CLOCK_MSI == MHZ(12)
#define CLOCK_MSIRANGE              (RCC_ICSCR1_MSISDIV_1) /* Range 0, Div 4 */
#elif CONFIG_CLOCK_MSI == MHZ(4)
#define CLOCK_MSIRANGE              (0x2UL << 16) /* Range 2, Div 1 */
#else
#error "Invalid MSI clock for STM32U3 - check manual for supported LDO frequencies"
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

/* Only periph_hwrng and periph_usbdev require HSI RC with 48MHz for the moment */
#if IS_USED(MODULE_PERIPH_HWRNG) || IS_USED(MODULE_PERIPH_USBDEV_CLK)
#define CLOCK_ENABLE_HSI48          1
#else
#define CLOCK_ENABLE_HSI48          0
#endif

/* Check if PLL is required
  - When PLLQ is used as 48MHz clock source
*/


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

/* Check if HSI RC with 16 MHz is required:
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
#define FLASH_WAITSTATES FLASH_ACR_LATENCY_2

void stmclk_init_sysclk(void)
{
    /* Bring-up safe clock init for STM32U3:
     * - SYSCLK = HSI16
     * - AHB = SYSCLK
     * - APB1/APB2 prescalers from CONFIG_CLOCK_APB{1,2}_DIV
     * - Enable PWR clock (needed by other code paths)
     * - Conservative Flash latency
     */
    unsigned is = irq_disable();

    /* Disable RCC clock interrupts */
    RCC->CIER = 0;

    /* 1) Enable HSI16 and wait until ready */
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}

    /* 2) Enable PWR interface clock (STM32U3: RCC_AHB1ENR2.PWREN) */
    RCC->AHB1ENR2 |= RCC_AHB1ENR2_PWREN;
    (void)RCC->AHB1ENR2; /* read-back */

    /* 3) Set Flash wait states BEFORE any clock increase
     * For HSI16, 1 wait state is conservative and safe.
     * (You can tune later once you run higher frequencies.)
     */
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;

    /* 4) Configure AHB/APB prescalers
     * Clear fields then apply configured divisors.
     * Note: CLOCK_APB1_DIV / CLOCK_APB2_DIV are bitfields you already define
     *       from CONFIG_CLOCK_APB{1,2}_DIV.
     */
    RCC->CFGR2 &= ~(RCC_CFGR2_HPRE_Msk |
                    RCC_CFGR2_PPRE1_Msk |
                    RCC_CFGR2_PPRE2_Msk);
    RCC->CFGR2 |= (CLOCK_APB1_DIV | CLOCK_APB2_DIV);

    /* 5) Switch SYSCLK to HSI16 and wait until switch is confirmed */
    RCC->CFGR1 = (RCC->CFGR1 & ~RCC_CFGR1_SW_Msk) | RCC_CFGR1_SW_HSI;
    while ((RCC->CFGR1 & RCC_CFGR1_SWS_Msk) != RCC_CFGR1_SWS_HSI) {}

    /* 6) Enable I-Cache (optional, but fine) */
#ifdef ICACHE
    ICACHE->CR |= ICACHE_CR_EN;
#endif

    irq_restore(is);
}
