/*
 * Copyright (C) 2023 Samuel Kayode
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
 * @brief       Testing of the ST l6470 microstepping motor driver board
 *
 * @author      Samuel Kayode <samkay014@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "l6470.h"
#include "l6470_params.h"

#include "ztimer.h"
#include "debug.h"
#include <string.h>

int main(void)
{
    l6470_t dev;
    l6470_speed_t fast;
    l6470_position_t pos;
    l6470_step_t step;
    int res;
    int daisy_id;
    int max_dev;
    uint8_t buff[50];

    puts("Generated RIOT application: 'driver_l6470'\n");

    printf("+------------Initializing------------+\n");
    if ((res = l6470_init(&dev, &l6470_params[0])) != L6470_OK) {
        puts("Initialization failed");
    }
    else {
        puts("Intialization successful\n");
    }

    max_dev = dev.params.spi_daisy_chain_devs;

    printf("\n+--------Status-------+\n");

    for (daisy_id = 0; daisy_id < max_dev; daisy_id++) {
        if ((res = l6470_mgmt_get_status(&dev, daisy_id)) != L6470_OK) {
            return 1;
        }
    }

    memset(buff, 0, 50);

    if ((res = l6470_send_command(&dev, buff)) == L6470_OK) {
        printf("Daisy chain devices present %d\n", dev.params.spi_daisy_chain_devs);
        printf("Dev 0 status: 0x%02x%02x\n", buff[0], buff[1]);
        printf("Dev 1 status: 0x%02x%02x\n", buff[2], buff[3]);
    }
    else {
        printf("Status command failed, %d", res);
    }   

    printf("\n+--------Run at constant speed--------+\n");
    fast.speed = 10000;
    fast.direction = 1;

    for (daisy_id = 0; daisy_id < max_dev; daisy_id++) {
        if ((res = l6470_mgmt_run(&dev, fast, daisy_id)) != L6470_OK) {
            puts("Invalid ID. Exiting");
            return 1;
        }
    }

    if ((res = l6470_send_command(&dev, buff)) == L6470_OK) {
        puts("Running at 10000 steps/tick\n");
    }
    else {
        printf("run command failed, %d", res);
    }

    ztimer_sleep(ZTIMER_MSEC, 2000);

    printf("\n+--------Stopping-------+\n");

    for (daisy_id = 0; daisy_id < max_dev; daisy_id++) {
        if ((res = l6470_mgmt_soft_stop(&dev, daisy_id)) != L6470_OK) {
            puts("Invalid ID. Exiting");
            return 1;
        }
    }

    if ((res = l6470_send_command(&dev, buff)) == L6470_OK) {
        puts("Stopped\n");
    }
    else {
        printf("Stop command failed, %d", res);
    }

    ztimer_sleep(ZTIMER_MSEC, 3000);

    printf("\n+--------Run to position--------+\n");
    pos.target = 327151;
    pos.direction = 1;

    for (daisy_id = 0; daisy_id < max_dev; daisy_id++) {
        if ((res = l6470_mgmt_go_to_dir(&dev, pos, daisy_id)) != L6470_OK) {
            puts("Invalid ID. Exiting");
            return 1;
        }
    }

    if ((res = l6470_send_command(&dev, buff)) == L6470_OK) {
        puts("At target position \n");
    }
    else {
        printf("go to command failed, %d", res);
    }

    ztimer_sleep(ZTIMER_MSEC, 3000);

    printf("\n+--------Switch step mode--------+\n");

    for (daisy_id = 0; daisy_id < max_dev; daisy_id++) {
        if ((res = l6470_mgmt_set_step_mode(&dev, L6470_STEP_MICRO_64,
                                            daisy_id)) != L6470_OK) {
            puts("Invalid ID. Exiting");
            return 1;
        }
    }

    if ((res = l6470_send_command(&dev, buff)) == L6470_OK) {
        puts("Switched step mode \n");
    }
    else {
        printf("move command failed. Error %d\n", res);
    }

    printf("\n+--------Move steps--------+\n");
    step.step = 100;
    step.direction = 0;

    for (daisy_id = 0; daisy_id < max_dev; daisy_id++) {
        if ((res = l6470_mgmt_move(&dev, step, daisy_id)) != L6470_OK) {
            puts("Invalid ID. Exiting");
            return 1;
        }
    }

    if ((res = l6470_send_command(&dev, buff)) == L6470_OK) {
        puts("Moved given steps");
    }
    else {
        puts("move command failed");
    }

    ztimer_sleep(ZTIMER_MSEC, 2000);

    printf("\n+--------Hard Stopping-------+\n");

    for (daisy_id = 0; daisy_id < max_dev; daisy_id++) {
        if ((res = l6470_mgmt_hard_stop(&dev, daisy_id)) != L6470_OK) {
            puts("Invalid ID. Exiting");
            return 1;
        }
    }

    if ((res = l6470_send_command(&dev, buff)) == L6470_OK) {
        puts("Stopped\n");
    }
    else {
        printf("Stop command failed. Error %d\n", res);
    }

    ztimer_sleep(ZTIMER_MSEC, 5000);

    return 0;
}
