/*
 * Copyright (C) 2024 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Example of a Modbus slave that wraps around SAUL
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"

#include "modbus_rtu_params.h"
#include "modbus_rtu.h"
#include "modbus.h"
#include "saul.h"
#include "shell.h"
#include "saul_reg.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define MODBUS_SLAVE_ID 1

static char stack[THREAD_STACKSIZE_DEFAULT];
static modbus_rtu_t modbus;
static modbus_message_t message;

/**
 * @brief   Modbus request handler that wraps around SAUL
 */
static int request_cb(modbus_message_t *message, void *arg)
{
    (void)arg;

    phydat_t data = { 0 };
    uint8_t val;
    saul_reg_t *reg;

    DEBUG("[main] request_cb: id = %u, function = %u, address = %u, count = %u\n",
          message->id, message->func, message->addr, message->count);

    if (message->id != MODBUS_SLAVE_ID) {
        return MODBUS_DROP;
    }

    if (modbus_check_message(message) != MODBUS_OK) {
        return message->exc = MODBUS_EXC_ILLEGAL_VALUE;
        return MODBUS_OK;
    }

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
        if (message->count != 1) {
            message->exc = MODBUS_EXC_ILLEGAL_VALUE;
            break;
        }

        printf("Reading coil: SAUL registration %u\n", message->addr);

        reg = saul_reg_find_nth(message->addr);

        /* check if a registration exists */
        if (reg == NULL) {
            message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
            break;
        }

        /* only switches are supported */
        if (reg->driver->type != SAUL_ACT_SWITCH) {
            message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
            break;
        }

        /* read switch status and write it to the message (in-place) */
        saul_reg_read(reg, &data);

        val = data.val[0];

        modbus_copy_bit(message->data, 0, &val, 0, true);

        break;
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        if (message->count != 1) {
            message->exc = MODBUS_EXC_ILLEGAL_VALUE;
            break;
        }

        printf("Reading discrete input: SAUL registration %u\n",
               message->addr);

        reg = saul_reg_find_nth(message->addr);

        /* check if a registration exists */
        if (reg == NULL) {
            message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
            break;
        }

        /* only buttons are supported */
        if (reg->driver->type != SAUL_SENSE_BTN) {
            message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
            break;
        }

        /* read registration and write it to the message (in-place) */
        saul_reg_read(reg, &data);

        val = data.val[0];

        modbus_copy_bit(message->data, 0, &val, 0, true);

        break;
    case MODBUS_FC_READ_HOLDING_REGISTERS:
        if (message->count != 1) {
            message->exc = MODBUS_EXC_ILLEGAL_VALUE;
            break;
        }

        printf("Reading holding register: SAUL registration %u\n",
               message->addr);

        reg = saul_reg_find_nth(message->addr);

        /* check if a registration exists */
        if (reg == NULL) {
            message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
            break;
        }

        /* read registration and write it to the message (in-place) */
        saul_reg_read(reg, &data);

        modbus_copy_reg(message->data, &(data.val[0]));

        break;
    case MODBUS_FC_WRITE_SINGLE_COIL:
        printf("Writing coil: SAUL registration %u\n", message->addr);

        reg = saul_reg_find_nth(message->addr);

        /* check if a registration exists */
        if (reg == NULL) {
            message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
            break;
        }

        /* only switches are supported */
        if (reg->driver->type != SAUL_ACT_SWITCH) {
            message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
            break;
        }

        /* per Modbus specification, value 0xff00 is ON, 0x0000 is OFF */
        if (message->data[0] == 0xff && message->data[1] == 0x00) {
            data.val[0] = 1;
        }
        else {
            data.val[0] = 0;
        }

        saul_reg_write(reg, &data);

        break;
    default:
        message->exc = MODBUS_EXC_ILLEGAL_FUNCTION;
        break;
    }

    return MODBUS_OK;
}

static void *thread_slave(void *arg)
{
    (void)arg;

    /* initialize the driver */
    int res = modbus_rtu_init(&modbus, &(modbus_rtu_params[0]));

    if (res != 0) {
        printf("Initialization failed, result is %u\n.", res);
        return NULL;
    }

    /* keep listening for requests */
    while (1) {
        res = modbus_rtu_recv_request(&modbus, &message, request_cb, NULL);

        if (res != 0) {
            printf("Receive request failed, result is %u\n", res);
        }
    }

    return NULL;
}

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    thread_create(stack, sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_slave, NULL,
                  "modbus");

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
