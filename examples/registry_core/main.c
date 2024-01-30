/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       RIOT Registry example application
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "periph_cpu.h"
#include "led.h"
#include "board.h"
#include "registry.h"
#include "registry/namespace/sys.h"
#include "registry/namespace/sys/board_led.h"
#include "ztimer.h"

int board_led_instance_0_commit_cb(const registry_commit_cb_scope_t scope,
                                 const registry_group_or_parameter_id_t *group_or_parameter_id,
                                 const void *context);

/* This belongs into the BOARD or Driver for example */
registry_sys_board_led_instance_t board_led_instance_0_data = {
    .enabled = 0,
};

registry_instance_t board_led_instance = {
    .data = &board_led_instance_0_data,
    .commit_cb = &board_led_instance_0_commit_cb,
};

int board_led_instance_0_commit_cb(const registry_commit_cb_scope_t scope,
                                 const registry_group_or_parameter_id_t *group_or_parameter_id,
                                 const void *context)
{
    (void)scope;
    (void)context;

    if (group_or_parameter_id != NULL) {
        if (*group_or_parameter_id == REGISTRY_SYS_BOARD_LED_ENABLED) {
            /* The Driver owns the board_led data instance, so we can just get our value from there */
            bool led_state = board_led_instance_0_data.enabled;

            /* Turn the LED on or off depending on the led_state */
            if (led_state == true) {
                /* This is the commit_cb function of instance 0, so we toggle LED 0 as well */
                LED_ON(0);
            } else {
                LED_OFF(0);
            }
        }
    }
    else {
        /* The whole instance got committed in one go, so apply all parameters (BOARD_LED has only one anyways)*/

        bool led_state = board_led_instance_0_data.enabled;
        if (led_state == true) {
            LED_ON(0);
        } else {
            LED_OFF(0);
        }
    }

    return 0;
}

/* This belongs into our main application */
int main(void)
{
    registry_init();

    /* init schemas */
    registry_add_schema_instance(&registry_sys_board_led, &board_led_instance);
    
    bool board_led_enabled = false;

    while (true) {
        /* Invert the BOARD LED, to make it turn on and off on each subsequent cycle */
        board_led_enabled = !board_led_enabled;

        /* Set new registry value */
        registry_set(&board_led_instance, &registry_sys_board_led_enabled, &board_led_enabled, sizeof(board_led_enabled));

        /* Apply the registry value to change the LED state (calls the commit_cb function implemented by the BOARD for example)*/
        registry_commit_parameter(&board_led_instance, &registry_sys_board_led_enabled);

        /* Sleep for 1 second and then do it again*/
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    return 0;
}
