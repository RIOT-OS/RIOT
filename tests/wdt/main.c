/*
 * Copyright (C) 2017 Technische Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        tests
 * @{
 *
 * @file
 * @brief           example/test for watchdog timer (WDT)
 *
 * @author          Thomas Geithner <thomas.geithner@dai-labor.de>
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "thread.h"
#include "mutex.h"
#include "xtimer.h"
#include "shell.h"
#include "shell_commands.h"

#include "periph/wdt.h"


#ifdef WDT_GPIO_DEBUG
#include "periph/gpio.h"

#ifndef REBOOT_GPIO
#define REBOOT_GPIO GPIO_PIN(3,2)
#endif

#ifndef WDT_RESET_GPIO
define WDT_RESET_GPIO GPIO_PIN(3,0)
#endif

#ifndef WDT_BLOCK_GPIO
#define WDT_BLOCK_GPIO GPIO_PIN(0,7)
#endif

gpio_t reboot_pin = REBOOT_GPIO;
gpio_t wdt_reset_pin = WDT_RESET_GPIO;
gpio_t wdt_block_pin = WDT_BLOCK_GPIO;
#endif

mutex_t block_mtx;

char wdt_reset_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t wdt_reset_pid;
volatile uint32_t wdt_update_interval = 1000 * 1000;

int wdt_cmd(int argc, char** argv);

static const shell_command_t shell_commands[] = { { "wdt", "WDT management",
    wdt_cmd }, { NULL, NULL, NULL } };

int wdt_cmd(int argc, char** argv) {
    if (argc < 2) {
        puts("WDT command required: [init[_min|_max|_ex]|enable|disable|block|help]");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        puts("init[_min|_max|_ex] <wdt_time in ms> [<wdt_update_interval in ms>]\n");
        puts("init_max <wdt_time in ms> [<wdt_update_interval in ms>]\n");
        puts("init_ex <wdt_time in ms> [<wdt_update_interval in ms>]\n");
        puts("enable\n");
        puts("disable\n");
        puts("block <time in ms>\n");
    }

    if (strcmp(argv[1], "enable") == 0) {
        if (wdt_enable() == 0) {
            puts("enabled WDT\n");
            return 0;
        } else {
            puts("enabling WDT failed\n");
            return -1;
        }
    }

    if (strcmp(argv[1], "disable") == 0) {
        if (wdt_disable() == 0) {
            puts("disabled WDT\n");
            return 0;
        } else {
            puts("disabling WDT failed\n");
            return -1;
        }
    }


    if (strncmp(argv[1], "init", 4) == 0) {

        uint32_t wdt_time = 1000;
        uint32_t wdt_update_time = 0;
        int32_t real_wdt_time;

        if (argc >= 3) {
            wdt_time = atoi(argv[2]);
            if (wdt_time == 0) {
                printf("invalid wdt_time: %s\n", argv[2]);
                return -1;
            }
        }
        if (argc >= 4) {
            wdt_update_time = atoi(argv[3]);
            if (wdt_update_time == 0) {
                printf("invalid wdt_update_interval: %s\n", argv[3]);
                return -1;
            }
        }
        printf("wdt_time=%ld\n", wdt_time);
        wdt_time *= 1000;

        if (strcmp(argv[1], "init_max") == 0) {
            real_wdt_time = wdt_init(wdt_time, WDT_MAX);
        } else if (strcmp(argv[1], "init_ex") == 0) {
            real_wdt_time = wdt_init(wdt_time, WDT_EXACT);
        } else if (strcmp(argv[1], "init_min") == 0) {
            real_wdt_time = wdt_init(wdt_time, WDT_MIN);
        } else{
            /* default: WDT_MIN */
            real_wdt_time = wdt_init(wdt_time, WDT_MIN);
        }

        if(real_wdt_time < 0){
            printf("got error initializing WDT\n");
            return -1;
        }

        if (wdt_update_time == 0) {
            wdt_update_interval = real_wdt_time / 4;
        } else {
            wdt_update_interval = wdt_update_time * 1000;
        }
        printf(
            "initialized WDT, interval=%ld.%ld ms, update_interval=%ld.%ld ms\n",
            real_wdt_time / 1000, (real_wdt_time % 1000) / 100,
            wdt_update_interval / 1000, (wdt_update_interval % 1000) / 100);

        return 0;
    }

    if (strcmp(argv[1], "block") == 0) {
        int time;
        if (argc < 3) {
            return -1;
        }

        time = atoi(argv[2]);

        printf("%s: time=%d\n", __func__, time);

        mutex_lock(&block_mtx);
#if WDT_GPIO_DEBUG
        gpio_set(wdt_block_pin);
#endif
        //xtimer_spin(xtimer_ticks_from_usec(time * 1000));
        xtimer_usleep(time * 1000);
#if WDT_GPIO_DEBUG
        gpio_clear(wdt_block_pin);
#endif
        mutex_unlock(&block_mtx);

        printf("%s: done.\n", __func__);
        return 0;
    }
    return 0;
}

void *wdt_reset_thread(void *arg) {
    (void) arg;

    //wdt_enable();
    while (1) {
        //printf("resetting WDT...\n");

        mutex_lock(&block_mtx);
        if (wdt_is_enabled()) {
            wdt_reset();
#if WDT_GPIO_DEBUG
            gpio_toggle(wdt_reset_pin);
#endif
        }
        mutex_unlock(&block_mtx);

        xtimer_usleep(wdt_update_interval);
    }
}

int main(void) {

#if WDT_GPIO_DEBUG
    gpio_init(reboot_pin, GPIO_OUT);
    gpio_init(wdt_reset_pin, GPIO_OUT);
    gpio_init(wdt_block_pin, GPIO_OUT);

    gpio_clear(wdt_reset_pin);
    gpio_clear(wdt_block_pin);
    gpio_set(reboot_pin);
#endif

    mutex_init(&block_mtx);

#if WDT_GPIO_DEBUG
    gpio_clear(reboot_pin);
#endif

    wdt_reset_pid = thread_create(wdt_reset_stack, sizeof(wdt_reset_stack),
    THREAD_PRIORITY_MAIN - 1,
    THREAD_CREATE_STACKTEST, wdt_reset_thread, NULL, "wdt_reset_thread");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
