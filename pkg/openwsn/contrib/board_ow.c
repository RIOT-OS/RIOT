/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption definition of the "board" bsp module
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 *
 * @}
 */

#include <stdio.h>

#include "sctimer.h"
#include "radio.h"
#include "uart_ow.h"
#include "board_ow.h"
#include "debugpins_riot.h"
#include "ledpins_riot.h"
#include "thread.h"
#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void board_init_ow(void)
{
    DEBUG("OpenWSN board_init\n");

    DEBUG("pins & leds init\n");
#if defined (BOARD_NUCLEO_F103) || defined (BOARD_SAMR21_XPRO)
    ledpins_riot_init(openwsn_ledpins_params);
#else
    ledpins_riot_init(NULL);
#endif
#if defined (BOARD_NUCLEO_F103)
    debugpins_riot_init(openwsn_debugpins_params);
#else
    debugpins_riot_init(NULL);
#endif

    DEBUG("sctimer_init\n");
    sctimer_init();

    DEBUG("uart_ow init\n");
    uart_init_ow();

    DEBUG("Finished board_init\n");
}

void board_sleep(void)
{
}

void board_reset(void)
{
    puts("OpenWSN board_reset");
    pm_reboot();
}
