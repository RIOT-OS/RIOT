/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for DALI peripheral device
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "shell_commands.h"
#include "shell.h"
#include "dali.h"
#include "dali_params.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef SHELL_BUFSIZE
#define SHELL_BUFSIZE       (128U)
#endif

#define DALI_CMD    0x90
#define DALI_BROADCAST_MASK 0xFF

static dali_peripheral_t dali_dev;
static char dali_peripheral_stack[THREAD_STACKSIZE_DEFAULT];

static void _onboard_leds_on(void)
{
#ifdef LED0_ON
    LED0_ON;
#endif /* LED0_ON */
#ifdef LED1_ON
    LED1_ON;
#endif /* LED1_ON */
#ifdef LED2_ON
    LED2_ON;
#endif /* LED2_ON */
#ifdef LED3_ON
    LED3_ON;
#endif /* LED3_ON */
}

static void _onboard_leds_off(void)
{
#ifdef LED0_OFF
    LED0_OFF;
#endif /* LED0_OFF */
#ifdef LED1_OFF
    LED1_OFF;
#endif /* LED1_OFF */
#ifdef LED2_OFF
    LED2_OFF;
#endif /* LED2_OFF */
#ifdef LED3_OFF
    LED3_OFF;
#endif /* LED3_OFF */
}

/**
 * @brief Thread that will manage DALI communication
 */
static void *dali_peripheral_thread(void *arg)
{
    uint8_t addr, data;
    dali_peripheral_t *dev = (dali_peripheral_t*)arg;

    while (1) {
        int ret;
        /* Wait for event on DALI bus */
        ret = dali_peripheral_recv(dev, &addr, &data);
        if (ret != 0) {
            printf("[dali_thread]: Error %d on reception\n", ret);
        } else {
            /* Check if DALI msg is for our node */
            if (addr == dev->addr) {
                /* If QUERY_STATUS (0x90) was sent, prepare answer */
                if (data == 0x90) {
                    ret = dali_peripheral_reply(dev, 0x00);
                    /* Print this AFTER reply so we won't kill DALI timing w/
                       a blocking stdio ... */
                    puts("[dali_thread]: QUERY_STATUS received");
                    if (ret != 0) {
                        printf("[dali_thread]: Error %d on transmission\n", ret);
                    }
                }
                /* data == 0x05 -> DALI CMD: Set arc power to max level */
                else if (data == 0x05) {
                    /* set onboard LEDs on if any */
                    _onboard_leds_on();
                }
                /* data == 0x00 -> DALI CMD: Extinguish Lamp without fading */
                else if (data == 0x00) {
                    /* set onboard LEDs off if any */
                    _onboard_leds_off();
                }
            } else if ((addr & DALI_BROADCAST_MASK) == 0xFF) {
                puts("[dali_thread]: broadcast command received");
            } else {
                /* Do nothing, ignore message */
            }
        }
    }

    /* unreachable */
    return NULL;
}

static int _cmd_dali_set_addr(int argc, char **argv)
{
    unsigned num = 0;

    if (argc < 2) {
        puts("dali_set_addr [addr]");
        return -EINVAL;
    }
    /* Register number in hex */
    if (strstr(argv[1], "0x") != NULL) {
        num = strtol(argv[1], NULL, 16);
    }
    else {
        num = atoi(argv[1]);
    }

    printf("update DALI peripheral address to 0x%x\n", num);
    return 0;
}
static const shell_command_t shell_commands[] = {
    {"dali_set_addr", "Modify DALI peripheral address (for test purpose only)", _cmd_dali_set_addr},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nDALI peripheral test application");

    memset(&dali_dev, 0, sizeof(dali_peripheral_t));
    dali_peripheral_init(&dali_dev, &dali_config[0], 0x21);

    /* Start the thread that will listen on DALI bus */
    thread_create(dali_peripheral_stack, sizeof(dali_peripheral_stack),
                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_STACKTEST,
                  dali_peripheral_thread, &dali_dev, "dali_peripheral");

    /* run the shell */
    char line_buf[SHELL_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_BUFSIZE);
    return 0;
}
