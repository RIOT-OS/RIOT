/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @brief       Test application for the PCA9633 I2C PWM controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "xtimer.h"
#include "shell.h"

#include "pca9633.h"
#include "pca9633_params.h"

pca9633_t pca9633_dev;

int turn_on(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    pca9633_turn_on(&pca9633_dev);
    return 0;
}

int turn_off(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    pca9633_turn_off(&pca9633_dev);
    return 0;
}

int wakeup(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    pca9633_wakeup(&pca9633_dev);
    return 0;
}

int enter_sleep(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    pca9633_sleep(&pca9633_dev);
    return 0;
}

int pwm(int argc, char **argv)
{
    if (argc != 3) {
        puts("usage: pwm <channel (0-3)> <signal (0-255)>");
    }
    else {
        uint8_t pwm = atoi(argv[2]);

        pca9633_pwm_channel_t pwm_channel;
        switch (atoi(argv[1])) {
            case 0:
                pwm_channel = PCA9633_PWM_CHANNEL_0;
                break;
            case 1:
                pwm_channel = PCA9633_PWM_CHANNEL_1;
                break;
            case 2:
                pwm_channel = PCA9633_PWM_CHANNEL_2;
                break;
            case 3:
                pwm_channel = PCA9633_PWM_CHANNEL_3;
                break;
            default:
                puts("channel needs to be one of [0-3]");
                return -1;
        }

        pca9633_set_pwm(&pca9633_dev, pwm_channel, pwm);
    }

    return 0;
}

int grp_pwm(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: grp_pwm <signal (0-255)>");
    }
    else {
        uint8_t pwm = atoi(argv[1]);

        pca9633_set_grp_pwm(&pca9633_dev, pwm);
    }

    return 0;
}

int blinking(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: blinking <0 or 1>");
    }
    else {
        uint8_t enabled = atoi(argv[1]);

        if (enabled >= 1) {
            pca9633_set_group_control_mode(&pca9633_dev,
                    PCA9633_GROUP_CONTROL_MODE_BLINKING);

            pca9633_set_blinking(&pca9633_dev,
                    1000,
                    PCA9633_BLINKING_RATIO_BALANCED);
        }
        else {
            pca9633_set_group_control_mode(&pca9633_dev,
                    PCA9633_GROUP_CONTROL_MODE_DIMMING);
        }

    }

    return 0;
}

int rgb(int argc, char **argv)
{
    if (argc != 4) {
        puts("usage: rgb <red (0-255)> <green (0-255)> <blue (0-255)>");
    }
    else {
        uint8_t r = atoi(argv[1]);
        uint8_t g = atoi(argv[2]);
        uint8_t b = atoi(argv[3]);

        pca9633_set_rgb(&pca9633_dev, r, g, b);
    }

    return 0;
}

int rgba(int argc, char **argv)
{
    if (argc != 5) {
        puts("usage: rgba <red (0-255)> <green (0-255)> <blue (0-255)> <amber (0-255)>");
    }
    else {
        uint8_t r = atoi(argv[1]);
        uint8_t g = atoi(argv[2]);
        uint8_t b = atoi(argv[3]);
        uint8_t w = atoi(argv[4]);

        pca9633_set_rgba(&pca9633_dev, r, g, b, w);
    }

    return 0;
}

int ldr_state(int argc, char **argv)
{
    if (argc != 3) {
        puts("usage: ldr_state <state (0-3)> pwm <channel (0-3)>");

        puts("  state 0: PCA9633_LDR_STATE_OFF");
        puts("  state 1: PCA9633_LDR_STATE_ON");
        puts("  state 2: PCA9633_LDR_STATE_IND");
        puts("  state 3: PCA9633_LDR_STATE_IND_GRP");

        puts("  channel 0: PCA9633_PWM_CHANNEL_0");
        puts("  channel 1: PCA9633_PWM_CHANNEL_1");
        puts("  channel 2: PCA9633_PWM_CHANNEL_2");
        puts("  channel 3: PCA9633_PWM_CHANNEL_3");
    }
    else {
        pca9633_ldr_state_t state;
        switch (atoi(argv[1])) {
            case 0:
                state = PCA9633_LDR_STATE_OFF;
                break;
            case 1:
                state = PCA9633_LDR_STATE_ON;
                break;
            case 2:
                state = PCA9633_LDR_STATE_IND;
                break;
            case 3:
                state = PCA9633_LDR_STATE_IND_GRP;
                break;
            default:
                puts("state needs to be one of [0-3]");
                puts("  state 0: PCA9633_LDR_STATE_OFF");
                puts("  state 1: PCA9633_LDR_STATE_ON");
                puts("  state 2: PCA9633_LDR_STATE_IND");
                puts("  state 3: PCA9633_LDR_STATE_IND_GRP");
                return -1;
        }

        pca9633_pwm_channel_t pwm_channel;
        switch (atoi(argv[2])) {
            case 0:
                pwm_channel = PCA9633_PWM_CHANNEL_0;
                break;
            case 1:
                pwm_channel = PCA9633_PWM_CHANNEL_1;
                break;
            case 2:
                pwm_channel = PCA9633_PWM_CHANNEL_2;
                break;
            case 3:
                pwm_channel = PCA9633_PWM_CHANNEL_3;
                break;
            default:
                puts("channel needs to be one of [0-3]");
                puts("  channel 0: PCA9633_PWM_CHANNEL_0");
                puts("  channel 1: PCA9633_PWM_CHANNEL_1");
                puts("  channel 2: PCA9633_PWM_CHANNEL_2");
                puts("  channel 3: PCA9633_PWM_CHANNEL_3");
                return -1;
        }

        pca9633_set_ldr_state(&pca9633_dev, state, pwm_channel);
    }

    return 0;
}

int ldr_state_all(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: ldr_state <state (0-3)>");

        puts("  state 0: PCA9633_LDR_STATE_OFF");
        puts("  state 1: PCA9633_LDR_STATE_ON");
        puts("  state 2: PCA9633_LDR_STATE_IND");
        puts("  state 3: PCA9633_LDR_STATE_IND_GRP");
    }
    else {
        pca9633_ldr_state_t state;
        switch (atoi(argv[1])) {
            case 0:
                state = PCA9633_LDR_STATE_OFF;
                break;
            case 1:
                state = PCA9633_LDR_STATE_ON;
                break;
            case 2:
                state = PCA9633_LDR_STATE_IND;
                break;
            case 3:
                state = PCA9633_LDR_STATE_IND_GRP;
                break;
            default:
                puts("state needs to be one of [0-3]");
                puts("  state 0: PCA9633_LDR_STATE_OFF");
                puts("  state 1: PCA9633_LDR_STATE_ON");
                puts("  state 2: PCA9633_LDR_STATE_IND");
                puts("  state 3: PCA9633_LDR_STATE_IND_GRP");
                return -1;
        }

        pca9633_set_ldr_state_all(&pca9633_dev, state);
    }

    return 0;
}

int auto_inc(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: auto_inc <option (0-4)>");

        puts("  option 0: PCA9633_AI_DISABLED");
        puts("  option 1: PCA9633_AI_ALL");
        puts("  option 2: PCA9633_AI_IND");
        puts("  option 3: PCA9633_AI_GBL");
        puts("  option 4: PCA9633_AI_IND_GBL");
    }
    else {
        pca9633_auto_inc_option_t option;
        switch (atoi(argv[1])) {
            case 0:
                option = PCA9633_AI_DISABLED;
                break;
            case 1:
                option = PCA9633_AI_ALL;
                break;
            case 2:
                option = PCA9633_AI_IND;
                break;
            case 3:
                option = PCA9633_AI_GBL;
                break;
            case 4:
                option = PCA9633_AI_IND_GBL;
                break;
            default:
                puts("option needs to be one of [0-4]");
                puts("  option 0: PCA9633_AI_DISABLED");
                puts("  option 1: PCA9633_AI_ALL");
                puts("  option 2: PCA9633_AI_IND");
                puts("  option 3: PCA9633_AI_GBL");
                puts("  option 4: PCA9633_AI_IND_GBL");
                return -1;
        }

        pca9633_set_auto_increment(&pca9633_dev, option);
    }

    return 0;
}

int grp_ctrl_mode(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: grp_ctrl_mode <mode (0-1)>");

        puts("  mode 0: PCA9633_GROUP_CONTROL_MODE_BLINKING");
        puts("  mode 1: PCA9633_GROUP_CONTROL_MODE_DIMMING");
    }
    else {
        pca9633_group_control_mode_t mode;
        switch (atoi(argv[1])) {
            case 0:
                mode = PCA9633_GROUP_CONTROL_MODE_BLINKING;
                break;
            case 1:
                mode = PCA9633_GROUP_CONTROL_MODE_DIMMING;
                break;
            default:
                puts("mode needs to be one of [0-1]");
                puts("  mode 0: PCA9633_GROUP_CONTROL_MODE_BLINKING");
                puts("  mode 1: PCA9633_GROUP_CONTROL_MODE_DIMMING");
                return -1;
        }

        pca9633_set_group_control_mode(&pca9633_dev, mode);
    }

    return 0;
}

int run_demo(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("[DEMO START]");
    pca9633_set_ldr_state_all(&pca9633_dev, PCA9633_LDR_STATE_IND_GRP);
    pca9633_set_group_control_mode(&pca9633_dev,
                                   PCA9633_GROUP_CONTROL_MODE_DIMMING);
    pca9633_set_rgb(&pca9633_dev, 255, 255, 255);
    xtimer_msleep(500);

    // 1. turn on/off
    puts("1. turn on/off");
    pca9633_turn_off(&pca9633_dev);
    xtimer_msleep(500);

    pca9633_turn_on(&pca9633_dev);
    xtimer_msleep(500);

    // 2. individual dimming (pca9633_set_rgb() uses pca9633_set_pwm() internally)
    puts("2. individual dimming");
    pca9633_set_rgb(&pca9633_dev, 255, 255, 255);
    xtimer_msleep(500);

    pca9633_set_rgb(&pca9633_dev, 255, 0, 0);
    xtimer_msleep(500);

    pca9633_set_rgb(&pca9633_dev, 0, 255, 0);
    xtimer_msleep(500);

    pca9633_set_rgb(&pca9633_dev, 0, 0, 255);
    xtimer_msleep(500);

    // 3. group dimming
    puts("3. group dimming");
    pca9633_set_rgb(&pca9633_dev, 255, 255, 255);
    pca9633_set_ldr_state_all(&pca9633_dev, PCA9633_LDR_STATE_IND_GRP);

    for (int pwm = 255; pwm >= 0; pwm--) {
        pca9633_set_grp_pwm(&pca9633_dev, pwm);
        xtimer_msleep(20);
    }
    xtimer_sleep(1);

    // 4. changing ldr state
    puts("4. changing ldr state");
    pca9633_set_grp_pwm(&pca9633_dev, 255);
    pca9633_set_rgb(&pca9633_dev, 255, 255, 255);
    pca9633_set_ldr_state(&pca9633_dev, PCA9633_LDR_STATE_OFF, PCA9633_PWM_CHANNEL_1);
    // color should be magenta
    xtimer_msleep(500);

    pca9633_set_grp_pwm(&pca9633_dev, 0);
    pca9633_set_rgb(&pca9633_dev, 0, 0, 0);
    pca9633_set_ldr_state(&pca9633_dev, PCA9633_LDR_STATE_ON, PCA9633_PWM_CHANNEL_1);
    // color should be green
    xtimer_msleep(500);

    pca9633_set_grp_pwm(&pca9633_dev, 255);
    pca9633_set_rgb(&pca9633_dev, 255, 128, 0);
    pca9633_set_ldr_state(&pca9633_dev,PCA9633_LDR_STATE_IND, PCA9633_PWM_CHANNEL_1);
    // color should be orange
    xtimer_msleep(500);

    pca9633_set_grp_pwm(&pca9633_dev, 0);
    pca9633_set_rgb(&pca9633_dev, 255, 255, 255);
    pca9633_set_ldr_state(&pca9633_dev, PCA9633_LDR_STATE_IND_GRP, PCA9633_PWM_CHANNEL_1);
    // should be no color at all
    xtimer_msleep(500);

    // 5. test blinking
    puts("5. test blinking");
    pca9633_set_grp_pwm(&pca9633_dev, 255);
    pca9633_set_rgb(&pca9633_dev, 255, 255, 255);
    pca9633_set_group_control_mode(&pca9633_dev,
            PCA9633_GROUP_CONTROL_MODE_BLINKING);
    pca9633_set_blinking(&pca9633_dev,
            1000,
            PCA9633_BLINKING_RATIO_BALANCED);
    xtimer_sleep(10);
    pca9633_set_group_control_mode(&pca9633_dev,
            PCA9633_GROUP_CONTROL_MODE_DIMMING);

    // 6. sleep mode
    puts("6. sleep mode");
    pca9633_set_rgb(&pca9633_dev, 0, 255, 255);
    xtimer_msleep(500);

    pca9633_sleep(&pca9633_dev);
    xtimer_sleep(2);

    pca9633_wakeup(&pca9633_dev);
    xtimer_msleep(500);

    puts("[DEMO END]");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "turn_on", "Turn on all LEDs.", turn_on },
    { "turn_off", "Turn off all LEDs.", turn_off },
    { "wakeup", "Switch to normal mode.", wakeup },
    { "sleep", "Switch to low power mode.", enter_sleep },
    { "pwm", "Set individual PWM signal for a given channel.", pwm },
    { "grp_pwm", "Set global PWM signal.", grp_pwm },
    { "blinking", "Set up values for blinking mode.", blinking },
    { "rgb", "Set PWM values for RGB.", rgb },
    { "rgba", "Set PWM values for RGBA.", rgba },
    { "ldr_state", "Set the LED driver output state for a given channel.", ldr_state },
    { "ldr_state_all", "Set the LED driver output state for all channels.", ldr_state_all },
    { "auto_increment", "Set an option for auto increment.", auto_inc },
    { "grp_ctrl_mode", "Set the group control mode.", grp_ctrl_mode },
    { "run_demo", "Demonstration of all functions.", run_demo },
    { NULL, NULL, NULL }
};

int main(void)
{

    if (pca9633_init(&pca9633_dev, &pca9633_params[0]) != PCA9633_OK) {
        puts("Initialization failed!");
        return 1;
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
