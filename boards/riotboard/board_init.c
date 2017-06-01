/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_riotboard
 * @{
 *
 * @file        board_init.c
 *
 * TODO needs to be revised
 *
 * @author      Leon George <l.george@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
/**
 * @brief				Initialise board-specific hardware
 */
void board_init(void) {
		gpio_set_gpio(LED1_PORT, LED1_PIN);
    gpio_set_direction(LED1_PORT, LED1_PIN, GPIO_GDIR_OUTPUT);

    gpio_set_gpio(LED2_PORT, LED2_PIN);
    gpio_set_direction(LED2_PORT, LED2_PIN, GPIO_GDIR_OUTPUT);

    LED_1_OFF;
    // TODO LED2 stopped working?!? Mixed up muxing or pin/port?
    LED_2_OFF;

    /* LED-test (CPU): */
    // TODO testietestie
    unsigned int cnt = 0;
    printf("counting variable driven LED test\n");

    while (1) {
        if (((cnt++) & 0x3FFFF) == 0) {
            LED_1_TOGGLE;
            LED_2_TOGGLE;
        }
    }
}
