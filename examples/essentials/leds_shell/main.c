/*
 * Copyright (C) 2024 Krzysztof Cabaj <kcabaj@gmail.com>
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
 * @brief       leds_shell - sample application for demonstrating internal
 *              board LEDs on/off and basic GPIO using interactive RIOT shell
 *
 * @author      Krzysztof Cabaj <kcabaj@gmail.com>
 *
 * @}
 */

#include "stdio.h"
#include "stdlib.h"
#include "shell.h"
#include "led.h"
#include <periph/gpio.h>

static int gpio_command(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s <init/set/clear> <port no.> <pin no.>\n", argv[0]);
        return -1;
    }

    int port_no = atoi(argv[2]);
    int pin_no = atoi(argv[3]);

    if (strcmp(argv[1], "init") == 0) {
        printf("GPIO initialization PORT %d, PIN %d\n", port_no, pin_no);

        int result;

        result = gpio_init(GPIO_PIN(port_no, pin_no), GPIO_OUT);

        if (result == 0) {
            printf("Success!\n");
        }
        else {
            printf("Failure!\n");
        }
    }
    else if (strcmp(argv[1], "set") == 0) {
         printf("Set HIGH to PORT %d, PIN %d\n", port_no, pin_no);
         gpio_set(GPIO_PIN(port_no, pin_no));
    }
    else if (strcmp(argv[1], "clear") == 0) {
         printf("Set LOW to PORT %d, PIN %d\n", port_no, pin_no);
         gpio_clear(GPIO_PIN(port_no, pin_no));
    }
    else {
        printf("usage: %s <init/set/clear> <port no.> <pin no.>\n", argv[0]);
    }

    return 0;
}

static int led_command(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <id> <on|off|toggle>\n", argv[0]);
        return -1;
    }

    int led_id = atoi(argv[1]);

    if (led_id >= LED_NUMOF) {
        printf("This board has %d LEDs\n", LED_NUMOF);
        return -1;
    }

    if (strcmp(argv[2], "on") == 0) {
        led_on(led_id);
    }
    else if (strcmp(argv[2], "off") == 0) {
        led_off(led_id);
    }
    else if (strcmp(argv[2], "toggle") == 0) {
        led_toggle(led_id);
    }
    else {
        printf("usage: %s <id> <on|off|toggle>\n", argv[0]);
    }

    return 0;
}

static const shell_command_t commands[] = {
    { "gpio", "GPIO pin initialization and set port state HIGH/LOW", gpio_command },
    { "led", "Switch on/off or toggle on-board LEDs", led_command},
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    printf("This board has %d LEDs\n", LED_NUMOF);

    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
