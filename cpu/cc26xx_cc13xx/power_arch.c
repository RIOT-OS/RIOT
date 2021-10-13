/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 *
 * @{
 *
 * @file
 * @brief       Power management abstractions
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "cc26xx_cc13xx_power.h"

#define DOMAIN_ON (1)

/* Save changes of the PRCM */
static void prcm_commit(void)
{
    /* Write CLKLOADCTL in the non-buffered register bank to avoid buffered
     * writes */
    PRCM_NONBUF->CLKLOADCTL = CLKLOADCTL_LOAD;

    /* Wait while load is done */
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}
}

bool power_is_domain_enabled(const power_domain_t domain)
{
    switch (domain) {
        case POWER_DOMAIN_PERIPHERALS:
            return PRCM->PDSTAT0PERIPH == DOMAIN_ON;

        case POWER_DOMAIN_SERIAL:
            return PRCM->PDSTAT0SERIAL == DOMAIN_ON;

        case POWER_DOMAIN_RFC:
            /* At least one of the registers need to indicate that the power
             * domain is on */
            return (PRCM->PDSTAT1RFC == DOMAIN_ON) ||
                   (PRCM->PDSTAT0RFC == DOMAIN_ON);

        case POWER_DOMAIN_VIMS:
            return PRCM->PDSTAT1VIMS == DOMAIN_ON;

        case POWER_DOMAIN_CPU:
            return PRCM->PDSTAT1CPU == DOMAIN_ON;

        default:
            return false;
    }

    return false;
}

void power_enable_domain(const power_domain_t domain)
{
    switch (domain) {
        case POWER_DOMAIN_PERIPHERALS:
            PRCM->PDCTL0PERIPH = DOMAIN_ON;
            break;

        case POWER_DOMAIN_SERIAL:
            PRCM->PDCTL0SERIAL = DOMAIN_ON;
            break;

        case POWER_DOMAIN_RFC:
            /* On CC26x0 MCUs PDCTL1RFC needs to be written too in order to
             * enable the RF Core power domain. On `cc13x2_cc26x2` it's not
             * necessary and domain is powered normally. */
            PRCM->PDCTL0RFC = DOMAIN_ON;
            PRCM->PDCTL1RFC = DOMAIN_ON;
            break;

        case POWER_DOMAIN_CPU:
            PRCM->PDCTL1CPU = DOMAIN_ON;
            break;

        case POWER_DOMAIN_VIMS:
            PRCM->PDCTL1VIMS = DOMAIN_ON;
            break;
    }

    while (!power_is_domain_enabled(domain)) {}
}

void power_clock_enable_gpio(void)
{
    /* enable clock gates for GPIO peripheral, for run mode
     * and sleep mode */
    PRCM->GPIOCLKGR |= GPIOCLKGR_CLK_EN;
    PRCM->GPIOCLKGS |= GPIOCLKGS_CLK_EN;

    prcm_commit();
}

void power_clock_enable_gpt(uint32_t tim)
{
    /* enable clock gates for GPT peripheral, for run mode and sleep mode */
    PRCM->GPTCLKGR |= (1 << tim);
    PRCM->GPTCLKGS |= (1 << tim);

    prcm_commit();
}
void power_clock_enable_i2c(void) {
    /* I2C peripheral is only enabled for run mode as it isn't necessary to
     * keep it running at sleep or deep sleep, as the I2C interrupt is mainly
     * for the slave mode ;-) */
    PRCM->I2CCLKGR |= I2CCLKGR_CLK_EN;

    prcm_commit();
}

void power_clock_enable_uart(uart_t uart)
{
    /* enable clock gates for UART peripheral, for run mode and sleep mode. */
    if (uart == 0) {
        PRCM->UARTCLKGR |= UARTCLKGR_CLK_EN_UART0;
        PRCM->UARTCLKGS |= UARTCLKGS_CLK_EN_UART0;
    }
#ifdef UARTCLKGR_CLK_EN_UART1
    else if (uart == 1) {
        PRCM->UARTCLKGR |= UARTCLKGR_CLK_EN_UART1;
        PRCM->UARTCLKGS |= UARTCLKGS_CLK_EN_UART1;
    }
#endif

    prcm_commit();
}

void power_clock_disable_uart(uart_t uart)
{
    /* disable clock gates for UART peripheral, for run and sleep mode */
    if (uart == 0) {
        PRCM->UARTCLKGR &= ~UARTCLKGR_CLK_EN_UART0;
        PRCM->UARTCLKGS &= ~UARTCLKGS_CLK_EN_UART0;
    }
#ifdef UARTCLKGR_CLK_EN_UART1
    else if (uart == 1) {
        PRCM->UARTCLKGR &= ~UARTCLKGR_CLK_EN_UART1;
        PRCM->UARTCLKGS &= ~UARTCLKGS_CLK_EN_UART1;
    }
#endif

    prcm_commit();
}
