/*
 * Copyright (C) 2021 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_thingy91
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Thingy:91 board
 *
 * @author      Thanh-Viet Nguyen <TopologicRose@protonmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "stdio_base.h"
#if defined(MODULE_LPS22HB) || defined(MODULE_HTS221)
#include "periph/gpio.h"
#endif


void board_init(void)
{
    /* LP22HB and HTS221 can only be powered with VREG when VDD_PWR_CTRL_PIN
       is set. */
#if defined(MODULE_LPS22HB) || defined(MODULE_HTS221)
    gpio_init(VDD_PWR_CTRL_PIN, GPIO_OUT);
    gpio_set(VDD_PWR_CTRL_PIN);
#endif

}
