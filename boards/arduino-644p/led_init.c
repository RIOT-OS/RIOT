/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015-18 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mega-xplained
 * @{
 *
 * @file
 * @brief       Board specific implementation for the Mega Xplained
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Matthew Blue <matthew.blu.neuro@gmail.com>
 *
 * @}
 */

#include "board.h"

void led_init(void)
{
    /* LED0,2 currently unsupported due to lack of GPIO_OD support */

    LED1_ENABLE_PORT;
    LED1_OFF;

    LED3_ENABLE_PORT;
    LED3_OFF;
}
