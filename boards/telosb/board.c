/*
 * board.c - Board initiazilation for the TelosB
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "board.h"
#include "cpu.h"
#include "periph_conf.h"

static void telosb_ports_init(void)
{
    /* Port 1: GDO, Flash, BSL TX */
    P1SEL = 0x02;    /* Port1 Select: 00000010 = 0x02 */
    P1OUT = 0x00;    /* Port1 Output: 00000000 = 0x00 */
    P1DIR = 0x87;    /* Port1 Direction: 10000111 = 0x87 */

    /* Port 2: GPIO, BSL RX, 1wire */
    P2SEL = 0x04;    /* Port2 Select: 00000100 = 0x04 */
    P2OUT = 0x00;    /* Port2 Output: 00000000 = 0x00 */
    P2DIR = 0xFF;    /* Port2 Direction: 11111111 = 0xFF */

    /* Port 3: UART and SPI */
    P3SEL = 0x0E;    /* Port3 Select: 11001110 = 0xCE */
    P3OUT = 0x00;    /* Port3 Output: 00000000 = 0x00 */
    P3DIR = 0xFE;    /* Port3 Direction: 01001110 = 0x4E */

    /* Port 4: CS */
    P4SEL = 0x02;    /* Port4 Select: 00000010 = 0x02 */
    P4OUT = 0x04;    /* Port4 Output: 00000100 = 0x04 */
    P4DIR = 0x64;    /* Port4 Direction: 01100100 = 0x64 */

    /* Port 5: SPI, LED */
    P5SEL = 0x00;    /* Port5 Select: 00000000 = 0x00 */
    P5OUT = 0x70;    /* Port5 Output: 01110000 = 0x70 */
    P5DIR = 0x70;    /* Port5 Direction: 01110000 = 0x70 */

    P6SEL = 0xFF;    /* Port6 Select: 11111111 = 0xFF */
    P6OUT = 0x00;    /* Port6 Output: 00000000 = 0x00 */
    P6DIR = 0xFF;    /* Port6 Direction: 11111000 = 0xFF */

}

void board_init(void)
{
    telosb_ports_init();
}
