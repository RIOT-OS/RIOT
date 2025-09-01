/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_pba-d-01-kw2x
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Phytec PBA-D-01
 *              evaluation board for PWA-A-002 Module
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "board.h"
#include "bit.h"
#include "periph/gpio.h"

static inline void modem_clock_init(void)
{
    /* Use the CLK_OUT of the modem as the clock source. */

    /* Enable GPIO clock gates */
    KW2XDRF_PORT_CLKEN();
    KW2XDRF_CLK_CTRL_CLKEN();

    /* Modem RST_B is connected to PTB19 and can be used to reset the modem. */
    KW2XDRF_PORT_DEV->PCR[KW2XDRF_RST_PIN] = PORT_PCR_MUX(1);
    bit_set32(&KW2XDRF_GPIO->PDDR, KW2XDRF_RST_PIN);
    KW2XDRF_GPIO->PCOR = (1 << KW2XDRF_RST_PIN);

    /* Modem GPIO5 is connected to PTC0 and can be used to select CLK_OUT frequency, */
    /* set PTC0 high for CLK_OUT=32.787kHz and low for CLK_OUT=4MHz. */
    KW2XDRF_CLK_CTRL_PORT_DEV->PCR[KW2XDRF_CLK_CTRL_PIN] = PORT_PCR_MUX(1);
    bit_set32(&KW2XDRF_CLK_CTRL_GPIO->PDDR, KW2XDRF_CLK_CTRL_PIN);
    KW2XDRF_CLK_CTRL_GPIO->PCOR = (1 << KW2XDRF_CLK_CTRL_PIN);

    /* Modem IRQ_B is connected to PTB3, modem interrupt request to the MCU. */
    KW2XDRF_PORT_DEV->PCR[KW2XDRF_IRQ_PIN] = PORT_PCR_MUX(1);
    bit_clear32(&KW2XDRF_GPIO->PDDR, KW2XDRF_IRQ_PIN);

    /* release the reset */
    KW2XDRF_GPIO->PSOR = (1 << KW2XDRF_RST_PIN);

    /* wait for modem IRQ_B interrupt request */
    while (KW2XDRF_GPIO->PDIR & (1 << KW2XDRF_IRQ_PIN));
}

void post_startup(void)
{
    modem_clock_init();
}
