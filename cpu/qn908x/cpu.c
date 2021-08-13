/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @{
 *
 * @file
 * @brief       QN908x CPU initialization
 *
 * @author      iosabi <iosabi@protonmail.com>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"

#include "stdio_base.h"

#include "vendor/drivers/fsl_clock.h"

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
#ifndef MODULE_PERIPH_WDT
    /* If the `periph_wdt` is *not* being used (because the user does not care
     * about that feature) we need to disable the Watchdog and continue running
     * without it. Otherwise the CPU will reboot after about 10 seconds.
     */
    CLOCK_DisableClock(kCLOCK_Wdt);
#endif /* ndef MODULE_PERIPH_WDT */

    /* TODO: It would be good to move the VTOR to SRAM to allow execution from
     * RAM with the FLASH memory off to allow for ultra low power operation on
     * sleep mode. This needs to be done after cortexm_init() since it sets the
     * VTOR to _isr_vectors which is the address on FLASH.
     */

    /* initialize the clocks */
    cpu_clock_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();
    /* trigger static peripheral initialization */
    periph_init();
}

/* Set up clock speed configuration. See cpu_conf.h for details about the
 * different clock options. */
void cpu_clock_init(void)
{
    /* Set up external clock frequency. */
#if CONFIG_BOARD_HAS_XTAL
#if CONFIG_BOARD_HAS_XTAL_32M
    CLOCK_AttachClk(k32M_to_XTAL_CLK);    /* Switch XTAL_CLK to 32M */
#elif CONFIG_BOARD_HAS_XTAL_16M
    CLOCK_AttachClk(k16M_to_XTAL_CLK);    /* Switch XTAL_CLK to 16M */
#else
#error "One of the CONFIG_BOARD_XTAL_* must be set."
#endif
#endif /* CONFIG_BOARD_HAS_XTAL */

    /* Set up 32K clock source. */
#if CONFIG_CPU_CLK_32K_XTAL
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK); /* Switch 32K_CLK to XTAL32K */
#elif CONFIG_CPU_CLK_32K_RCO
    CLOCK_AttachClk(kRCO32K_to_32K_CLK);  /* Switch 32K_CLK to RCO32K */
#else
#error "One of the CONFIG_CPU_CLK_32K_* must be set."
#endif

    /* Set up System clock source. */
#if CONFIG_CPU_CLK_SYS_XTAL
    CLOCK_AttachClk(kXTAL_to_SYS_CLK);    /* Switch SYS_CLK to XTAL */
#elif CONFIG_CPU_CLK_SYS_OSC32M
    CLOCK_AttachClk(kOSC32M_to_SYS_CLK);  /* Switch SYS_CLK to OSM32M */
#elif CONFIG_CPU_CLK_SYS_32K
    CLOCK_AttachClk(k32K_to_SYS_CLK);     /* Switch SYS_CLK to 32K source */
#else
#error "One of the CONFIG_CPU_CLK_SYS_* must be set."
#endif

    /* Run the WDT from the APB always. */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);

    /* Set up dividers */

    /* Set OSC32M_DIV divider */
#if CONFIG_CPU_CLK_OSC32M_DIV != 0 && CONFIG_CPU_CLK_OSC32M_DIV != 1
#error "Invalid CONFIG_CPU_CLK_OSC32M_DIV value"
#endif
    /* Note: The denominator is set to (CONFIG_CPU_CLK_OSC32M_DIV + 1), so /2
     * when the macro is enabled. */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, CONFIG_CPU_CLK_OSC32M_DIV);

    /* Set XTAL_DIV divider */
#if CONFIG_CPU_CLK_XTAL_DIV != 0 && CONFIG_CPU_CLK_XTAL_DIV != 1
#error "Invalid CONFIG_CPU_CLK_XTAL_DIV value"
#endif
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, CONFIG_CPU_CLK_XTAL_DIV);

    /* Set AHB_DIV divider. */
#if CONFIG_CPU_CLK_AHB_DIV < 1 || CONFIG_CPU_CLK_AHB_DIV > 8192
#error "Invalid CONFIG_CPU_CLK_AHB_DIV"
#endif
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, CONFIG_CPU_CLK_AHB_DIV - 1u);

    /* Set APB_DIV divider. */
#if CONFIG_CPU_CLK_APB_DIV < 1 || CONFIG_CPU_CLK_APB_DIV > 16
#error "Invalid CONFIG_CPU_CLK_APB_DIV"
#endif
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, CONFIG_CPU_CLK_APB_DIV - 1u);
}
