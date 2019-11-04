/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_atmega256rfr2-xpro
 * @{
 *
 * @file
 * @brief       Board specific implementation for the Atmega256RFR2 Xplained Pro
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"

void led_init(void)
{
    LED0_ENABLE_PORT;
    LED0_OFF;
}
