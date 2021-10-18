/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_e104-bt50xxa-tb
 * @{
 *
 * @file
 * @brief       Board initialization for the E104-BT50xxA Test Board
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

extern void pm_reboot(void*);

void board_init(void)
{
    /* initialize the boards LEDs */
    LED_PORT->DIRSET = (LED_MASK);
    LED_PORT->OUTSET = (LED_MASK);

    /* configure software RST button */
#ifdef MODULE_BOARD_SOFTWARE_RESET
    gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_FALLING,
                  pm_reboot, NULL);
#endif

}
