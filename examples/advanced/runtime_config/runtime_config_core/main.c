/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Runtime configuration core minimal example application to demonstrate
 *              how to use the "Runtime configuration" module without any of its extensions.
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
#include "runtime_config.h"
#include "runtime_config/namespace/sys.h"
#include "runtime_config/namespace/sys/board_led.h"
#include "ztimer.h"

/* This callback gets called, when new configurations shall be applied.
 * If interacting with configurations of drivers then this callback should be
 * implemented by the driver itself. For custom application logic, we need to
 * define this for ourselves. */
static runtime_config_error_t board_led_instance_apply_cb(
    const runtime_config_group_or_parameter_id_t *group_or_parameter_id,
    const runtime_config_schema_instance_t *instance)
{
    const runtime_config_sys_board_led_instance_t instance_data = *(const runtime_config_sys_board_led_instance_t *)instance->data;

    /* Either apply all parameters of the instance or only the given parameter.
     * For a single LED there is no difference as it only has one parameter. */
    if ((group_or_parameter_id == NULL) ||
        (*group_or_parameter_id == RUNTIME_CONFIG_SYS_BOARD_LED_ENABLED)) {
        /* Get the correct field from the instance_data variable */
        bool led_state = instance_data.enabled;
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

/* This belongs into the BOARD or Driver for example */
static runtime_config_sys_board_led_instance_t board_led_instance_data = {
    .enabled = 0,
};

static runtime_config_schema_instance_t board_led_instance = {
    .data = &board_led_instance_data,
    .apply_cb = &board_led_instance_apply_cb,
};

/* This belongs into our main application */
int main(void)
{
    runtime_config_init();

    /* init schemas */
    runtime_config_add_schema_instance(&runtime_config_sys_board_led, &board_led_instance);

    bool board_led_enabled = false;

    while (true) {
        /* Invert the BOARD LED, to make it turn on and off on each subsequent cycle */
        board_led_enabled = !board_led_enabled;

        /* Create runtime_config_node_t for the board_led_parameter */
        const runtime_config_node_t parameter_node = {
            .type = RUNTIME_CONFIG_NODE_PARAMETER,
            .value.parameter = {
                .instance = &board_led_instance,
                .parameter = &runtime_config_sys_board_led_enabled,
            },
        };

        /* Set new runtime configuration value */
        runtime_config_set(&parameter_node, &board_led_enabled, sizeof(board_led_enabled));

        /* Apply the runtime configuration value to change the LED state
         * (in this case calls the apply_cb function: "board_led_instance_apply_cb") */
        runtime_config_apply(&parameter_node);

        /* Sleep for 1 second and then do it again*/
        ztimer_sleep(ZTIMER_SEC, 1);
    }

    return 0;
}
