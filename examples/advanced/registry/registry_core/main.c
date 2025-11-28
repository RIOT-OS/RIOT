/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       RIOT registry core minimal example application to demonstrate
 *              how to use the riot registry without any of its extensions.
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

registry_error_t board_led_instance_apply_cb(
    const registry_apply_cb_scope_t scope,
    const registry_group_or_parameter_id_t *group_or_parameter_id,
    const void *context);

/* This belongs into the BOARD or Driver for example */
registry_sys_board_led_instance_t board_led_instance_0_data = {
    .enabled = 0,
};

registry_instance_t board_led_instance = {
    .data = &board_led_instance_0_data,
    .apply_cb = &board_led_instance_apply_cb,
};

/* this callback is usually implemented drivers such as an RGB LED driver */
registry_error_t board_led_instance_apply_cb(
    const registry_apply_cb_scope_t scope,
    const registry_group_or_parameter_id_t *group_or_parameter_id,
    const void *context)
{
    (void)scope;
    (void)context;

    /* Either apply all parameters of the instance or only the given parameter.
     * For a single LED there is no difference as it only has one parameter. */
    if ((group_or_parameter_id == NULL) ||
        (*group_or_parameter_id == REGISTRY_SYS_BOARD_LED_ENABLED)) {
        /* The Driver owns the board_led data instance, so we can just get our value from there */
        bool led_state = board_led_instance_0_data.enabled;
        /* Turn the LED on or off depending on the led_state */
        if (led_state == true) {
            /* This is the apply_cb function of instance 0, so we toggle LED 0 as well */
            LED_ON(0);
        }
        else {
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

        /* Create registry_node_t for the board_led_parameter */
        const registry_node_t parameter_node = {
            .type = REGISTRY_NODE_PARAMETER,
            .value.parameter = {
                .instance = &board_led_instance,
                .parameter = &registry_sys_board_led_enabled,
            },
        };

        /* Set new registry value */
        registry_set(&parameter_node, &board_led_enabled, sizeof(board_led_enabled));

        /* Apply the registry value to change the LED state */
        /* (in this case calls the apply_cb function: "board_led_instance_apply_cb") */
        registry_apply(&parameter_node);

        /* Sleep for 1 second and then do it again*/
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    return 0;
}
