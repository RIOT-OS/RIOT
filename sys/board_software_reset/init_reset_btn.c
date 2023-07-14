/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @@defgroup   module_board_software_reset board_software_reset
 * @brief       Use any software-only reset button on the board to reboot
 *
 * Some boards have reset buttons that are not wired to the MCU's reset line,
 * but merely are configured to cause a reset by convention.
 *
 * If this module is active, the button will be configured thusly (and then not
 * be advertised in any other capacity, e.g. through @ref sys_auto_init_saul).
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#include "auto_init.h"
#include "auto_init_utils.h"
#include "board.h"
#include "periph/gpio.h"

#ifndef BTN_RST_INT_FLANK
#define BTN_RST_INT_FLANK   GPIO_FALLING
#endif

extern void pm_reboot(void*);

static void init_reset_btn(void)
{
    /* configure software RST button */
    gpio_init_int(BTN_RST_PIN, BTN_RST_MODE, BTN_RST_INT_FLANK,
                  pm_reboot, NULL);
}

AUTO_INIT(init_reset_btn, 0);
