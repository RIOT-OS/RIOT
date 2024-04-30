/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* provide address for register maps by taking the address of the first
 * register (as provided by the vendor files) */

PROVIDE(PORT_1              = P1IN);
PROVIDE(PORT_2              = P2IN);
PROVIDE(PORT_3              = P3IN);
PROVIDE(PORT_3              = P3IN);
PROVIDE(PORT_4              = P4IN);
PROVIDE(PORT_5              = P5IN);
PROVIDE(PORT_6              = P6IN);

PROVIDE(TIMER_A             = TACTL);
PROVIDE(TIMER_B             = TBCTL);
PROVIDE(TIMER_A_IRQFLAGS    = TAIV);
PROVIDE(TIMER_B_IRQFLAGS    = TBIV);
