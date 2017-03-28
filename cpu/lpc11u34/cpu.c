/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"


#define SYSOSCCTRL_Val    0x00000000 /* Reset: 0x000 */
#define SYSPLLCTRL_Val    0x00000023 /* Reset: 0x000 */
#define SYSPLLCLKSEL_Val  0x00000001 /* Reset: 0x000 */
#define MAINCLKSEL_Val    0x00000003 /* Reset: 0x000 */
#define SYSAHBCLKDIV_Val  0x00000001 /* Reset: 0x001 */
#define USBPLLCTRL_Val    0x00000023 /* Reset: 0x000 */
#define USBPLLCLKSEL_Val  0x00000001 /* Reset: 0x000 */
#define USBCLKSEL_Val     0x00000000 /* Reset: 0x000 */
#define USBCLKDIV_Val     0x00000001 /* Reset: 0x001 */

/**
 * @brief Initialize the CPU clock
 *
 * This configuration use an external XTAL
 */
void clk_init(void)
{
    volatile uint32_t i;

    /* Power-up System Osc */
    LPC_SYSCON->PDRUNCFG &= ~(1 << 5);
    LPC_SYSCON->SYSOSCCTRL = SYSOSCCTRL_Val;
    for (i = 0; i < 200; i++) __NOP();

    /* Select PLL Input*/
    LPC_SYSCON->SYSPLLCLKSEL = SYSPLLCLKSEL_Val;
    /* Update clock source */
    LPC_SYSCON->SYSPLLCLKUEN = 0x01;
    /* Toggle update register */
    LPC_SYSCON->SYSPLLCLKUEN = 0x00;
    LPC_SYSCON->SYSPLLCLKUEN = 0x01;
    /* Wait until updated */
    while (!(LPC_SYSCON->SYSPLLCLKUEN & 0x01));
    /* Main Clock is PLL Out */
    LPC_SYSCON->SYSPLLCTRL = SYSPLLCTRL_Val;
    /* Power-up SYSPLL */
    LPC_SYSCON->PDRUNCFG &= ~(1 << 7);
    /* Wait Until PLL Locked */
    while (!(LPC_SYSCON->SYSPLLSTAT & 0x01));

    /* Select PLL Clock Output  */
    LPC_SYSCON->MAINCLKSEL = MAINCLKSEL_Val;
    /* Update MCLK Clock Source */
    LPC_SYSCON->MAINCLKUEN = 0x01;
    /* Toggle Update Register   */
    LPC_SYSCON->MAINCLKUEN = 0x00;
    LPC_SYSCON->MAINCLKUEN = 0x01;
    /* Wait Until Updated       */
    while (!(LPC_SYSCON->MAINCLKUEN & 0x01));

    LPC_SYSCON->SYSAHBCLKDIV = SYSAHBCLKDIV_Val;

    /* Power-up USB PHY         */
    LPC_SYSCON->PDRUNCFG &= ~(1 << 10);
    /* Power-up USB PLL         */
    LPC_SYSCON->PDRUNCFG &= ~(1 <<  8);
    /* Select PLL Input         */
    LPC_SYSCON->USBPLLCLKSEL = USBPLLCLKSEL_Val;
    /* Update Clock Source      */
    LPC_SYSCON->USBPLLCLKUEN = 0x01;
    /* Toggle Update Register   */
    LPC_SYSCON->USBPLLCLKUEN = 0x00;
    LPC_SYSCON->USBPLLCLKUEN = 0x01;
    /* Wait Until Updated */
    while (!(LPC_SYSCON->USBPLLCLKUEN & 0x01));
    LPC_SYSCON->USBPLLCTRL = USBPLLCTRL_Val;
    /* Wait Until PLL Locked */
    while (!(LPC_SYSCON->USBPLLSTAT & 0x01));

    /* Select USB Clock */
    LPC_SYSCON->USBCLKSEL = USBCLKSEL_Val;
    /* Toggle Update Register */
    LPC_SYSCON->USBCLKUEN = 0x00;
    LPC_SYSCON->USBCLKUEN = 0x01;
    /* Set USB clock divider */
    LPC_SYSCON->USBCLKDIV = USBCLKDIV_Val;

    /* System clock to the IOCON needs to be enabled or
       most of the I/O related peripherals won't work. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);
}

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize the clock */
    clk_init();
    /* trigger static peripheral initialization */
    periph_init();
}
