/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief       ACD52832 test application
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "mcp23017.h"
#include "mcp23017_regs.h"
#include "mcp23017_params.h"
#include "xtimer.h"

#define SLEEP_DELAY 1000 * 1000U
#define PROGRAM_DELAY 10 * 1000U

mcp23017_t dev;

static void int_handler(void *arg)
{
    uint8_t value;
    /* Reset and capture interrupt */
    value = mcp23017_capture_interrupt(&dev, PORT_A);
    if (~value & PIN_3) {
        puts("S4-R pressed\n");
    }
    else if (~value & PIN_4) {
        puts("S4-DN pressed\n");
    }
    else if (~value & PIN_5) {
        puts("S4-L pressed\n");
    }
    else if (~value & PIN_6) {
        puts("S4-UP pressed\n");
    }
    else if (~value & PIN_7) {
        puts("S4-CTR pressed\n");
    }
}

int main(void)
{
    int int_a_pin = mcp23017_params[0].int_a_pin;
    // int int_b_pin = mcp23017_params[0].int_b_pin;

    dev.i2c = MCP23017_PARAM_I2C;
    dev.addr = MCP23017_PARAM_ADDR;

    puts("[MCP23017]: test application");
    printf("[MCP23017]: initializing at I2C_DEV(%i)...\n", dev.i2c);

    if (mcp23017_init(&dev, (mcp23017_params_t*)mcp23017_params) == MCP23017_OK) {
        puts("[MCP23017]: init MCP23017 OK\n");
    }
    else {
        puts("[MCP23017]: init MCP23017 failed\n");
        return -1;
    }

    /* LED 2 */
    if (mcp23017_set_dir(&dev, PORT_B, PIN_1, OUTPUT) == MCP23017_OK) {
        puts("[MCP23017]: init LED 2 OK\n");
    }
    else {
        puts("[MCP23017]: init LED 2 failed\n");
        return -1;
    }
    xtimer_usleep(PROGRAM_DELAY);

    /* SWITCH 4 */
    if (mcp23017_set_int(&dev, PORT_A, PIN_3, ENABLE) == MCP23017_OK) {
        puts("[MCP23017]: init S4-R  OK\n");
    }
    else {
        puts("[MCP23017]: init S4-R failed\n");
        return -1;
    }
    xtimer_usleep(PROGRAM_DELAY);
    if (mcp23017_set_int(&dev, PORT_A, PIN_4, ENABLE) == MCP23017_OK) {
        puts("[MCP23017]: init S4-DN OK\n");
    }
    else {
        puts("[MCP23017]: init S4-DN failed\n");
        return -1;
    }
    xtimer_usleep(PROGRAM_DELAY);
    if (mcp23017_set_int(&dev, PORT_A, PIN_5, ENABLE) == MCP23017_OK) {
        puts("[MCP23017]: init S4-L OK\n");
    }
    else {
        puts("[MCP23017]: init S4-L failed\n");
        return -1;
    }
    xtimer_usleep(PROGRAM_DELAY);
    if (mcp23017_set_int(&dev, PORT_A, PIN_6, ENABLE) == MCP23017_OK) {
        puts("[MCP23017]: init S4-UP OK\n");
    }
    else {
        puts("[MCP23017]: init S4-UP failed\n");
        return -1;
    }
    xtimer_usleep(PROGRAM_DELAY);
    if (mcp23017_set_int(&dev, PORT_A, PIN_7, ENABLE) == MCP23017_OK) {
        puts("[MCP23017]: init S4-CTR OK\n");
    }
    else {
        puts("[MCP23017]: init S4-CTR failed\n");
        return -1;
    }
    xtimer_usleep(PROGRAM_DELAY);
    mcp23017_capture_interrupt(&dev, PORT_A);

    // /* SWITCH 2 */
    // if (mcp23017_set_int(&dev, PORT_B, PIN_0, ENABLE) == MCP23017_OK) {
    //     puts("[MCP23017]: init S2 OK\n");
    // }
    // else {
    //     puts("[MCP23017]: init S2 failed\n");
    //     return -1;
    // }
    // xtimer_usleep(PROGRAM_DELAY);
    // mcp23017_capture_interrupt(&dev, PORT_B);

    /* Interrupt setup */
    if (gpio_init_int(GPIO_PIN(0, int_a_pin), GPIO_IN, GPIO_FALLING, int_handler, (void *)int_a_pin) < 0) {
        puts("[MCP23017] error: init of INT A failed\n");
        return 1;
    }

    while(1) {
        /* Toggle LED 2 */
        puts("[MCP23017]: LED 2 on\n");
        if (mcp23017_set_pin_value(&dev, PORT_B, PIN_1, HIGH) != MCP23017_OK) {
            return -1;
        }
        xtimer_usleep(SLEEP_DELAY);
        puts("[MCP23017]: LED 2 off\n");
        if (mcp23017_set_pin_value(&dev, PORT_B, PIN_1, LOW) != MCP23017_OK) {
            return -1;
        }
        xtimer_usleep(SLEEP_DELAY);
    }
    return 0;
}
