/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example of a Modbus server that wraps around SAUL switches and
 *              buttons.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"

#include "saul.h"
#include "shell.h"
#include "saul_reg.h"
#include "thread.h"

#include "modbus.h"
#if defined(MODBUS_TRANSPORT_ASCII)
#  include "modbus_ascii.h"
#  include "modbus_ascii_params.h"
#elif defined(MODBUS_TRANSPORT_RTU)
#  include "modbus_rtu.h"
#  include "modbus_rtu_params.h"
#elif defined(MODBUS_TRANSPORT_TCP)
#  include "modbus_tcp.h"
#  include "modbus_tcp_params.h"
#else
#  error "Please enable a Modbus transport module."
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define MODBUS_SERVER_ID 1

static char stack[THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE];

static modbus_t modbus;
#if defined(MODBUS_TRANSPORT_ASCII)
static modbus_ascii_t modbus_dev;
#elif defined(MODBUS_TRANSPORT_RTU)
static modbus_rtu_t modbus_dev;
#elif defined(MODBUS_TRANSPORT_TCP)
static modbus_tcp_t modbus_dev;
#endif
static modbus_server_t modbus_server;
static modbus_message_t message;

/**
 * @brief   Modbus request handler that wraps around SAUL
 */
static int request_cb(modbus_t *modbus, modbus_server_t *server, modbus_message_t *message)
{
    (void)server;

    int res;
    phydat_t data = { 0 };
    uint8_t val;
    saul_reg_t *reg;

    DEBUG("[main] request_cb: id=%u, function=%u, address=%u, count=%u\n",
          message->id, message->func, message->addr, message->count);

    if (modbus_check_message(message) != 0) {
        DEBUG("[main] request_cb: invalid message\n");

        message->exc = MODBUS_EXC_ILLEGAL_VALUE;
    }
    else {
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

            /* only buttons are supported */
            if (reg->driver->type != SAUL_SENSE_BTN) {
                message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
                break;
            }

            /* read switch status and write it to the message (in-place) */
            saul_reg_read(reg, &data);

            val = data.val[0];

            modbus_copy_bit(message->data, 0, &val, 0, true);

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
    }

    res = modbus_server_reply(modbus, message);

    if (res != 0) {
        printf("Reply failed, result is %d\n", res);
        return res;
    }

    return 0;
}

static void *thread_server(void *arg)
{
    (void)arg;

    /* initialize the driver with the configured transport */
    int res;

#if defined(MODBUS_TRANSPORT_ASCII)
    res = modbus_init(&modbus, &modbus_ascii_transport, &modbus_dev,
                      (void *)modbus_ascii_params);
#elif defined(MODBUS_TRANSPORT_RTU)
    res = modbus_init(&modbus, &modbus_rtu_transport, &modbus_dev,
                      (void *)modbus_rtu_params);
#elif defined(MODBUS_TRANSPORT_TCP)
    res = modbus_init(&modbus, &modbus_tcp_transport, &modbus_dev,
                      (void *)modbus_tcp_params);
#endif

    if (res != 0) {
        printf("Server initialization failed, result is %d.\n", res);
        return NULL;
    }

    /* set-up server */
    modbus_server.id = MODBUS_SERVER_ID;
    modbus_server.cb = request_cb;
    modbus_server.arg = NULL;

    modbus_server_add(&modbus, &modbus_server);

    /* keep listening for requests */
    while (1) {
        res = modbus_server_listen(&modbus, &message);

        if (res != 0) {
            printf("Listen failed, result is %d\n", res);
        }
    }

    return NULL;
}

static void print_usage(void)
{
    printf("Modbus server is running (server ID: %u).\n", MODBUS_SERVER_ID);

    printf("Connect using:\n");
#if defined(MODBUS_TRANSPORT_ASCII)
    printf("  Transport:  Modbus ASCII\n");
    printf("  UART:       UART_DEV(%u)\n", modbus_ascii_params[0].uart);
    printf("  Baudrate:   %lu baud\n", (unsigned long)modbus_ascii_params[0].baudrate);
#elif defined(MODBUS_TRANSPORT_RTU)
    printf("  Transport:  Modbus RTU\n");
    printf("  UART:       UART_DEV(%u)\n", modbus_rtu_params[0].uart);
    printf("  Baudrate:   %lu baud\n", (unsigned long)modbus_rtu_params[0].baudrate);
#elif defined(MODBUS_TRANSPORT_TCP)
    printf("  Transport:  Modbus TCP\n");
    printf("  Port:       %u\n", modbus_tcp_params[0].endpoint.port);
#endif
    printf("\n");

    printf("SAUL registrations exposed:\n");
    printf("  %-8s %-24s %s\n", "Address", "Name", "Function codes");

    saul_reg_t *reg = saul_reg;
    int addr = 0;

    while (reg != NULL) {
        const char *funcs;

        if (reg->driver->type == SAUL_SENSE_BTN) {
            funcs = "Read Coils";
        }
        else if (reg->driver->type == SAUL_ACT_SWITCH) {
            funcs = "Write Single Coil";
        }
        else {
            funcs = "(not supported)";
        }

        printf("  %-8d %-24s %s\n", addr, reg->name, funcs);

        reg = reg->next;
        addr++;
    }
}

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* run above the shell so requests are served while the shell blocks */
    thread_create(stack, sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_server, NULL,
                  "modbus");

    print_usage();

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
