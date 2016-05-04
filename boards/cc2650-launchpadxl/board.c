/*
 * Copyright (C) 2016 Nicholas Jackson <nicholas.jackson@griffithuni.edu.au>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_cc2650launchpadxl CC2650LAUNCHPADXL
 * @ingroup     boards
 * @brief       Board specific files for the SimpleLink multi-standard CC2650 Launchpad board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SimpleLink multi-standard CC2650 Launchpad board
 *
 * @author      Nicholas Jackson <nicholas.jackson@griffithuni.edu.au>
 */

#include "cpu.h"
#include "board.h"

/**
 * @brief           Initialise the board.
 */

void board_init(void){
    cpu_init();

    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
}