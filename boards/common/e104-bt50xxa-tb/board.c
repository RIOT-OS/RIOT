/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
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
    /* configure software RST button */
#ifdef MODULE_BOARD_SOFTWARE_RESET
    gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_FALLING,
                  pm_reboot, NULL);
#endif

}
