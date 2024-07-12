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
 * @brief       LEDs - sample application for demonstrating internal
 *              board LEDs on/off and basic GPIO
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

static int init_command(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port no.> <pin no.>\n", argv[0]);
        return -1;
    }

    int port_no = atoi(argv[1]);
    int pin_no = atoi(argv[2]);

    printf("GPIO initialization PORT %d, PIN %d\n", port_no, pin_no);

    int result;

    result = gpio_init(GPIO_PIN(port_no, pin_no), GPIO_OUT);

    if (result == 0) {
        printf("Success!\n");
    }
    else if (result == -1) {
        printf("Failure!\n");
    }

    return 0;
}

static int set_command(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port no.> <pin no.>\n", argv[0]);
        return -1;
    }

    int port_no = atoi(argv[1]);
    int pin_no = atoi(argv[2]);

    printf("Set HIGH to PORT %d, PIN %d\n", port_no, pin_no);

    gpio_set(GPIO_PIN(port_no, pin_no));

    return 0;
}

static int clear_command(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port no.> <pin no.>\n", argv[0]);
        return -1;
    }

    int port_no = atoi(argv[1]);
    int pin_no = atoi(argv[2]);

    printf("Set LOW to PORT %d, PIN %d\n", port_no, pin_no);

    gpio_clear(GPIO_PIN(port_no, pin_no));

    return 0;
}

#ifdef LED0_IS_PRESENT
static int led0_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED0_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED0_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED0_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif

#ifdef LED1_IS_PRESENT
static int led1_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED1_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED1_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED1_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif

#ifdef LED2_IS_PRESENT
static int led2_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED2_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED2_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED2_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif

#ifdef LED3_IS_PRESENT
static int led3_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED3_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED3_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED3_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif

#ifdef LED4_IS_PRESENT
static int led4_command(int argc, char **argv)
{
    if (argc <= 1) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED4_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED4_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED4_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif

#ifdef LED5_IS_PRESENT
static int led5_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED5_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED5_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED5_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif

#ifdef LED6_IS_PRESENT
static int led6_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED6_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED6_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED6_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif

#ifdef LED7_IS_PRESENT
static int led7_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "on") == 0) {
        LED7_ON;
    }
    else if (strcmp(argv[1], "off") == 0) {
        LED7_OFF;
    }
    else if (strcmp(argv[1], "toggle") == 0) {
        LED7_TOGGLE;
    }
    else {
        printf("usage: %s <on|off|toggle>\n", argv[0]);
    }

    return 0;
}
#endif


static const shell_command_t commands[] = {
    { "init", "GPIO pin initialization", init_command },
    { "set", "Set GPIO pin to HIGH", set_command },
    { "clear", "Set GPIO pin to LOW", clear_command },
#ifdef LED0_IS_PRESENT
    { "led0", "Switch on/off on-board LED0", led0_command },
#endif
#ifdef LED1_IS_PRESENT
    { "led1", "Switch on/off on-board LED1", led1_command },
#endif
#ifdef LED2_IS_PRESENT
    { "led2", "Switch on/off on-board LED2", led2_command },
#endif
#ifdef LED3_IS_PRESENT
    { "led3", "Switch on/off on-board LED3", led3_command },
#endif
#ifdef LED4_IS_PRESENT
    { "led4", "Switch on/off on-board LED4", led4_command },
#endif
#ifdef LED5_IS_PRESENT
    { "led5", "Switch on/off on-board LED5", led5_command },
#endif
#ifdef LED6_IS_PRESENT
    { "led6", "Switch on/off on-board LED6", led6_command },
#endif
#ifdef LED7_IS_PRESENT
    { "led7", "Switch on/off on-board LED7", led7_command },
#endif
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    printf("leds_shell, version 1.0.0\n");

    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
