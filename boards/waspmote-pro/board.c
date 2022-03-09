/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015-18 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 INRIA, Francisco Acosta
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waspmote-pro
 * @{
 *
 * @file
 * @brief       Board specific initializations
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* initialize UART_1 on AUX1 */
    SET_MUX_AUX1_MODULE;

#ifdef XBEE_UART
#if XBEE_UART == 0
    /* initialize UART0 on SOCKET0 (XBee) */
    SET_MUX_SOCKET0;
#else
    /* Initialize UART0 on USB */
    SET_MUX_USB_MODULE;
#endif
#endif
}
