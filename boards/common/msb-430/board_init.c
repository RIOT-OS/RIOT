/*
 * SPDX-FileCopyrightText: 2014 INRIA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup boards
 * @{
 * @file
 * @brief       msb-430 common board initialization
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "board.h"
#include "stdio_base.h"
#include "periph_conf.h"
#include "msp430.h"
#include "debug.h"

static void msb_ports_init(void)
{
    /* Port 1: Free port, for energy saving all outputs are set to zero. */
    P1SEL = 0x00;   /* Port1 I/O Function */
    P1OUT = 0x00;   /* Port1 Output register: 00000000 = 0x00 */
    P1DIR = 0xFF;   /* Port1 Direction: 11111111 = 0xFF */

    P2SEL = 0x20;   /* Port2 I/O Function */
    P2OUT = 0x00;   /* Port2 Output register: 00000000 = 0x00 */
    P2DIR = 0x1C;   /* Port2 Direction: 00011010 = 0x1C */
    /*   0 - P2.0 [IN ] - */
    /*   0 - P2.1 [OUT] - */
    /*   1 - P2.2 [IN ] - */
    /*   1 - P2.3 [OUT] - */
    /*   1 - P2.4 [OUT] - */
    /*   0 - P2.5 [IN ] - */
    /*   0 - P2.6 [IN ] - SDC Protect */
    /*   0 - P2.7 [IN ] - SDC Detect */

    P3SEL = 0x00;   /* Port3 Pins 6 & 7 for USART */
    P3OUT = 0x00;   /* Port3 Output register: 01001001: 0x49 */
    P3DIR = 0xFF;   /* Port3 Direction: 10101011: 0xAB */
    /*   1 - P3.0 */
    /*   1 - P3.1 */
    /*   0 - P3.2 */
    /*   1 - P3.3 */
    /*   0 - P3.4 [IN ] - SHT 11 DATA (OUT/IN) */
    /*   1 - P3.5 [OUT] - SHT 11 CLK */
    /*   0 - P3.6 [2-Funktion] - RS232_RxD */
    /*   1 - P3.7 [2-Funktion] - RS232_TxD */

    /* Port 4: Free port, for energy saving all outputs are set to zero. */
    P4SEL = 0x00;   /* Port4 I/O Function */
    P4OUT = 0x00;   /* Port4 Output register: 00000000 = 0x00 */
    P4DIR = 0xFF;   /* Port4 Direction: 11111111 = 0xFF */
    /*   1 - P4.0 [OUT] - unused */
    /*   1 - P4.1 [OUT] - unused */
    /*   1 - P4.2 [OUT] - unused */
    /*   1 - P4.3 [OUT] - unused */
    /*   1 - P4.4 [OUT] - unused */
    /*   1 - P4.5 [OUT] - unused */
    /*   1 - P4.6 [OUT] - unused */
    /*   1 - P4.7 [OUT] - unused */

    P5SEL = 0x00;   /* Port5 I/O Function: 00000000 = 0x00 */
    P5OUT = 0x80;   /* Port5 Output register: 00001001 = 0x09 */
    P5DIR = 0xFF;   /* Port5 Direction: 11111011 = 0xFB */
    /*   1 - P5.0 [OUT] - SDC /CS */
    /*   1 - P5.1 [OUT] - SDC DI */
    /*   0 - P5.2 [IN ] - SDC DO */
    /*   1 - P5.3 [OUT] - SDC DCLK */
    /*   1 - P5.4 [OUT] - MMA GS1 */
    /*   1 - P5.5 [OUT] - MMA GS2 */
    /*   1 - P5.6 [OUT] - MMA /SLEEP */
    /*   1 - P5.7 [OUT] - LED_RED 0-on, 1-off */

    P6SEL = 0x00;   /* Port6 I/O Function = 0x07 */
    P6OUT = 0x00;   /* Port6 Output register: 00000000 = 0x00 */
    P6DIR = 0xFF;   /* Port6 Direction: 11111000 = 0xF8 */
    /*   0 - P6.0 [AD-IN] - MMA X-Axis */
    /*   0 - P6.1 [AD-IN] - MMA Y-Axis */
    /*   0 - P6.2 [AD-IN] - MMA Z-Axis */
    /*   1 - P6.3 [OUT] - unused */
    /*   1 - P6.4 [OUT] - unused */
    /*   1 - P6.5 [OUT] - unused */
    /*   1 - P6.6 [OUT] - unused */
    /*   1 - P6.7 [OUT] - unused */
}

void board_init(void)
{
    msb_ports_init();
}
