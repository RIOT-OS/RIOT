/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for using Modbus as a client, server or both.
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 *
 * @}
 */

#ifndef TEST_BAUDRATE
#  error "TEST_BAUDRATE not defined"
#endif
#ifndef TEST_CLIENT_UART
#  error "TEST_CLIENT_UART not defined"
#endif
#ifndef TEST_CLIENT_PIN_RTS
#  error "TEST_CLIENT_PIN_RTS not defined"
#endif
#ifndef TEST_SERVER_UART
#  error "TEST_SERVER_UART not defined"
#endif
#ifndef TEST_SERVER_PIN_RTS
#  error "TEST_SERVER_PIN_RTS not defined"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modbus_rtu.h"
#include "shell.h"
#include "thread.h"
#include "timex.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define COUNT_REGISTERS 16

static bool loopback = false;
static bool misbehave = false;
static int iterations = 0;
static int servers = 1;
static int server_id = 1;

static char stack_client[THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE];
static modbus_rtu_params_t params_client_rtu;
static modbus_rtu_t client_rtu;
static modbus_t client;
static uint8_t client_read[COUNT_REGISTERS * 2];
static uint8_t client_write[COUNT_REGISTERS * 2];
static modbus_message_t message_client;

static char stack_server[THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE];
static modbus_rtu_params_t params_server_rtu;
static modbus_rtu_t server_rtu;
static modbus_t server;
static modbus_server_t server_reg;
static uint8_t server_read_write[COUNT_REGISTERS * 2];
static modbus_message_t message_server;

#if TEST_CLIENT_UART != TEST_SERVER_UART
static modbus_message_t message_server_copy;
static modbus_message_t message_client_copy;
#endif

/**
 * @brief   Copy the server message
 *
 * For loopback mode to work properly, assertions from a server thread on the
 * client message (and vice versa) are not reliable, because either thread
 * can already have modified the message in the meantime.
 */
static void copy_message_server(void)
{
#if TEST_CLIENT_UART != TEST_SERVER_UART
    memcpy(&message_server_copy, &message_server, sizeof(modbus_message_t));
#endif
}

/**
 * @brief   Copy the client message
 *
 * For loopback mode to work properly, assertions from a server thread on the
 * client message (and vice versa) are not reliable, because either thread
 * can already have modified the message in the meantime.
 */
static void copy_message_client(void)
{
#if TEST_CLIENT_UART != TEST_SERVER_UART
    memcpy(&message_client_copy, &message_client, sizeof(modbus_message_t));
#endif
}

/**
 * @brief   Check loopback message fields from client perspective
 *
 * This compares the client message fields against the server copy to verify
 * that the server received the correct message in loopback mode.
 *
 * @return  @c true if all checked fields match, or not in loopback mode
 */
static bool check_loopback_server(void)
{
#if TEST_CLIENT_UART != TEST_SERVER_UART
    if (!loopback) {
        return true;
    }
    else if (message_client.id != message_server_copy.id) {
        puts("Failed, server mismatch (id)");
        return false;
    }
    else if (message_client.func != message_server_copy.func) {
        puts("Failed, server mismatch (function)");
        return false;
    }
    else if (message_client.addr != message_server_copy.addr) {
        puts("Failed, server mismatch (address)");
        return false;
    }
    else if (message_client.count != message_server_copy.count) {
        puts("Failed, server mismatch (count)");
        return false;
    }
#endif

    return true;
}

/**
 * @brief   Check loopback message fields from server perspective
 *
 * This compares the server message fields against the client copy to verify
 * that the client sent the correct message in loopback mode.
 *
 * @return  true if all checked fields match or not in loopback mode
 */
static bool check_loopback_client(void)
{
#if TEST_CLIENT_UART != TEST_SERVER_UART
    if (!loopback) {
        return true;
    }
    else if (message_server.id != message_client_copy.id) {
        puts("Failed, client mismatch (id)");
        return false;
    }
    else if (message_server.func != message_client_copy.func) {
        puts("Failed, client mismatch (function)");
        return false;
    }
    else if (message_server.addr != message_client_copy.addr) {
        puts("Failed, client mismatch (address)");
        return false;
    }
    else if (message_server.count != message_client_copy.count) {
        puts("Failed, client mismatch (count)");
        return false;
    }
    return true;
#endif

    return true;
}

/**
 * @brief   Check if two pointers to registers are equal
 *
 * @param[in] a         First register pointer
 * @param[in] b         Second register pointer
 * @param[in] addr      Address offset in registers
 * @param[in] count     Number of registers to compare
 *
 * @return 0 if equal, non-zero otherwise
 */
static int compare_regs(const uint8_t *a, const uint8_t *b, uint16_t addr, uint16_t count)
{
    uint8_t offset = modbus_reg_count_to_size(addr);

    return memcmp(a, &(b[offset]), modbus_reg_count_to_size(count));
}

/**
 * @brief   Check if two pointers to bit (coil) strings are equal
 *
 * @param[in] a         First bit string pointer
 * @param[in] b         Second bit string pointer
 * @param[in] addr      Address offset in bits
 * @param[in] count     Number of bits to compare
 *
 * @return 0 if equal, non-zero otherwise
 */
static int compare_bits(const uint8_t *a, const uint8_t *b, uint16_t addr, uint16_t count)
{
    uint8_t a_bits[COUNT_REGISTERS];
    uint8_t b_bits[COUNT_REGISTERS];

    modbus_copy_bits(a_bits, 0, a, 0, count, true);
    modbus_copy_bits(b_bits, 0, b, addr, count, true);

    return memcmp(a_bits, b_bits, modbus_bit_count_to_size(count));
}

/**
 * @brief   Basic Modbus request handler that copies from/to the server buffer
 *
 * @param[in] modbus        Modbus instance
 * @param[in] server        Server instance
 * @param[in,out] message   Modbus message
 *
 * @return 0 on success, negative error code otherwise
 */
static int request_cb(modbus_t *modbus, modbus_server_t *server, modbus_message_t *message)
{
    (void)server;

    printf("Request callback, id = %u, function = %u, address = %u, count = %u\n",
           message->id, message->func, message->addr, message->count);

    if (message->id == MODBUS_ID_BROADCAST) {
        puts("Broadcasted message received, not responding.");
        return 0;
    }

    if (modbus_check_message(message) != 0) {
        message->exc = MODBUS_EXC_ILLEGAL_VALUE;
    }
    else if (message->addr >= COUNT_REGISTERS) {
        message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
    }
    else {
        uint8_t reg_offset = modbus_reg_count_to_size(message->addr);
        uint8_t bit_offset = message->addr;

        switch (message->func) {
        case MODBUS_FC_READ_COILS:
        case MODBUS_FC_READ_DISCRETE_INPUTS:
            modbus_copy_bits(message->data, 0, server_read_write, bit_offset, message->count, true);
            break;
        case MODBUS_FC_READ_INPUT_REGISTERS:
        case MODBUS_FC_READ_HOLDING_REGISTERS:
            modbus_copy_regs(message->data, &(server_read_write[reg_offset]), message->count);
            break;
        case MODBUS_FC_WRITE_SINGLE_COIL:
            modbus_copy_bit(server_read_write, bit_offset, message->data, 0, false);
            break;
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
            modbus_copy_bits(server_read_write, bit_offset, message->data, 0, message->count,
                             false);
            break;
        case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
            modbus_copy_reg(&(server_read_write[reg_offset]), message->data);
            break;
        case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
            modbus_copy_regs(&(server_read_write[reg_offset]), message->data, message->count);
            break;
        default:
            message->exc = MODBUS_EXC_ILLEGAL_FUNCTION;
            break;
        }

        if (misbehave && (rand() % 3) == 0) {
            switch (rand() % 8) {
            case 0:
                puts("Misbehaving: altering count (downwards)");
                if (message->count > 1) {
                    message->count -= 1;
                }
                break;
            case 1:
                puts("Misbehaving: altering count (upwards)");
                message->count += 1;
                break;
            case 2:
                puts("Misbehaving: altering size (downwards)");
                if (message->data_size > 1) {
                    message->data_size -= 1;
                }
                break;
            case 3:
                puts("Misbehaving: altering size (upwards)");
                message->data_size += 1;
                break;
            case 4:
                puts("Misbehaving: altering id");
                message->id += 1;
                break;
            case 5:
                puts("Misbehaving: altering function");
                message->func += 1;
                break;
            case 6:
                puts("Misbehaving: omitting response");
                return 0;
                break;
            case 7:
                puts("Misbehaving: additional response");
                modbus_server_reply(modbus, message);
                break;
            }
        }
    }

    copy_message_server();

    int res = modbus_server_reply(modbus, message);

    if (res != 0) {
        printf("Failed, reply result is %d\n", res);
        return res;
    }

    return 0;
}

static void test_reset(bool server_read_write_random)
{
    /* clear read/write buffers */
    for (unsigned i = 0; i < ARRAY_SIZE(client_read); i++) {
        client_read[i] = 0;
    }
    for (unsigned i = 0; i < ARRAY_SIZE(client_write); i++) {
        client_write[i] = rand();
    }
    for (unsigned i = 0; i < ARRAY_SIZE(server_read_write); i++) {
        server_read_write[i] = server_read_write_random ? rand() : 0;
    }

    /* clear client message */
    memset(&message_client, 0, sizeof(message_client));

    /* clear server message */
    memset(&message_server, 0, sizeof(message_server));
}

static void test_write_multiple_holding_registers(uint8_t id, uint16_t addr, uint16_t count)
{
    puts("Testing MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS...");

    test_reset(false);

    message_client.id = id;
    message_client.func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS;
    message_client.addr = addr;
    message_client.count = count;
    message_client.data = client_write;
    message_client.data_size = sizeof(client_write);

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
        }
        else if (loopback && compare_regs(
                     client_write,
                     server_read_write,
                     addr,
                     count) != 0) {
            puts("Failed, server mismatch (data)");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_HOLDING_REGISTERS...");

        message_client.func = MODBUS_FC_READ_HOLDING_REGISTERS;
        message_client.data = client_read;
        message_client.data_size = sizeof(client_read);

        copy_message_client();

        res = modbus_client_request(&client, &message_client);

        if (res != 0) {
            printf("Failed, request result is %d\n", res);
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
            }
            else if (compare_regs(client_read, client_write, 0, count) != 0) {
                puts("Failed, response mismatch");
            }
            else {
                puts("OK");
            }
        }
    }

    puts("");
}

static void test_write_single_holding_register(uint8_t id, uint16_t addr)
{
    puts("Testing MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER...");

    test_reset(false);

    message_client.id = id;
    message_client.func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER;
    message_client.addr = addr;
    message_client.data = client_write;
    message_client.data_size = sizeof(client_write);

    /* not used, but satisfies the generic loopback check in server thread */
    message_client.count = 1;

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
        }
        else if (loopback && compare_regs(
                     client_write,
                     server_read_write,
                     addr,
                     1) != 0) {
            puts("Failed, server mismatch (data)");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_HOLDING_REGISTERS...");

        message_client.func = MODBUS_FC_READ_HOLDING_REGISTERS;
        message_client.count = 1;
        message_client.data = client_read;
        message_client.data_size = sizeof(client_read);

        copy_message_client();

        res = modbus_client_request(&client, &message_client);

        if (res != 0) {
            printf("Failed, request result is %d\n", res);
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
            }
            else if (compare_regs(client_read, client_write, 0, 1) != 0) {
                puts("Failed, response mismatch");
            }
            else {
                puts("OK");
            }
        }
    }

    puts("");
}

static void test_read_discrete_inputs(uint8_t id, uint16_t addr, uint16_t count)
{
    puts("Testing MODBUS_FC_READ_DISCRETE_INPUTS...");

    test_reset(true);

    message_client.id = id;
    message_client.func = MODBUS_FC_READ_DISCRETE_INPUTS;
    message_client.addr = addr;
    message_client.count = count;
    message_client.data = client_read;
    message_client.data_size = sizeof(client_read);

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
        }
        else if (loopback && compare_bits(client_read,
                                          server_read_write,
                                          addr,
                                          count) != 0) {
            puts("Failed, server mismatch (data)");
        }
        else {
            puts("OK");
        }
    }

    puts("");
}

static void test_read_input_registers(uint8_t id, uint16_t addr, uint16_t count)
{
    puts("Testing MODBUS_FC_READ_INPUT_REGISTERS...");

    test_reset(true);

    message_client.id = id;
    message_client.func = MODBUS_FC_READ_INPUT_REGISTERS;
    message_client.addr = addr;
    message_client.count = count;
    message_client.data = client_read;
    message_client.data_size = sizeof(client_read);

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
        }
        else if (loopback && compare_regs(client_read,
                                          server_read_write,
                                          addr,
                                          count) != 0) {
            puts("Failed, server mismatch (data)");
        }
        else {
            puts("OK");
        }
    }

    puts("");
}

static void test_write_multiple_coils(uint8_t id, uint16_t addr, uint16_t count)
{
    puts("Testing MODBUS_FC_WRITE_MULTIPLE_COILS...");

    test_reset(false);

    message_client.id = id;
    message_client.func = MODBUS_FC_WRITE_MULTIPLE_COILS;
    message_client.addr = addr;
    message_client.count = count;
    message_client.data = client_write;
    message_client.data_size = sizeof(client_write);

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
        }
        else if (loopback && compare_bits(client_write,
                                          server_read_write,
                                          addr,
                                          count) != 0) {
            puts("Failed, server mismatch (data)");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_COILS...");

        message_client.func = MODBUS_FC_READ_COILS;
        message_client.data = client_read;
        message_client.data_size = sizeof(client_read);

        copy_message_client();

        res = modbus_client_request(&client, &message_client);

        if (res != 0) {
            printf("Failed, request result is %d\n", res);
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
            }
            else if (compare_bits(client_read, client_write, 0, message_client.count) != 0) {
                puts("Failed, response mismatch");
            }
            else {
                puts("OK");
            }
        }
    }

    puts("");
}

static void test_write_single_coil(uint8_t id, uint16_t addr)
{
    puts("Testing MODBUS_FC_WRITE_SINGLE_COIL...");

    test_reset(false);

    message_client.id = id;
    message_client.func = MODBUS_FC_WRITE_SINGLE_COIL;
    message_client.addr = addr;
    message_client.data = client_write;
    message_client.data_size = sizeof(client_write);

    /* not used, but satisfies the generic loopback check in server thread */
    message_client.count = 1;

    /* single coil data must be 0xff00 or 0x0000 */
    client_write[0] = rand() & 0x01 ? 0xff : 0x00;
    client_write[1] = 0x00;

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
        }
        else if (loopback && compare_bits(client_write,
                                          server_read_write,
                                          addr,
                                          1) != 0) {
            puts("Failed, server mismatch (data)");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_COILS...");

        message_client.func = MODBUS_FC_READ_COILS;
        message_client.count = 1;
        message_client.data = client_read;
        message_client.data_size = sizeof(client_read);

        copy_message_client();

        res = modbus_client_request(&client, &message_client);

        if (res != 0) {
            printf("Failed, request result is %d\n", res);
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
            }
            else if (compare_bits(client_read, client_write, 0, 1) != 0) {
                puts("Failed, response mismatch");
            }
            else {
                puts("OK");
            }
        }
    }

    puts("");
}

static void test_broadcast(uint8_t addr, uint16_t count)
{
    puts("Testing MODBUS_ID_BROADCAST...");

    test_reset(false);

    message_client.id = MODBUS_ID_BROADCAST;
    message_client.func = MODBUS_FC_WRITE_MULTIPLE_COILS;
    message_client.addr = addr;
    message_client.count = count;
    message_client.data = client_write;
    message_client.data_size = sizeof(client_write);

    copy_message_client();

    int res = modbus_client_broadcast(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        puts("OK");
    }

    /* wait for at least the response timeout, so that servers have time to
     * process the request and start waiting for next request */
    ztimer_sleep(ZTIMER_USEC, params_client_rtu.response_timeout);

    puts("");
}

static void test_exception(uint8_t id, uint16_t addr, uint16_t count)
{
    puts("Testing MODBUS_EXC_ILLEGAL_ADDRESS...");

    test_reset(false);

    message_client.id = id;
    message_client.func = MODBUS_FC_READ_HOLDING_REGISTERS;
    message_client.addr = addr + COUNT_REGISTERS; /* outside address range */
    message_client.count = count;
    message_client.data = client_read;
    message_client.data_size = sizeof(client_read);

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != 0) {
        printf("Failed, request result is %d\n", res);
    }
    else if (message_client.exc != MODBUS_EXC_ILLEGAL_ADDRESS) {
        puts("Failed, response mismatch");
    }
    else {
        puts("OK");
    }

    puts("");
}

static void test_timeout(void)
{
    puts("Testing -ETIMEDOUT...");

    test_reset(false);

    message_client.id = servers + 1; /* intentionally non-existing */
    message_client.func = MODBUS_FC_READ_HOLDING_REGISTERS;
    message_client.addr = 1;
    message_client.count = 1;
    message_client.data = client_read;
    message_client.data_size = sizeof(client_read);

    copy_message_client();

    int res = modbus_client_request(&client, &message_client);

    if (res != -ETIMEDOUT) {
        printf("Failed, request result is %d\n", res);
    }
    else {
        puts("OK");
    }

    puts("");
}

static void *thread_client(void *arg)
{
    (void)arg;

    params_client_rtu.pin_rts = TEST_CLIENT_PIN_RTS;
    params_client_rtu.pin_rts_enable = 1;
    params_client_rtu.uart = UART_DEV(TEST_CLIENT_UART);
    params_client_rtu.baudrate = TEST_BAUDRATE;
    params_client_rtu.response_timeout = 1000 * US_PER_MS;

    /* initialize the driver */
    puts("Initializing driver...");

    int res = modbus_init(&client, &modbus_rtu_driver, &client_rtu, &params_client_rtu);

    if (res != 0) {
        printf("Failed, init result is %d\n.", res);
        return NULL;
    }
    else {
        puts("OK");
    }

    /* explicit yield to ensure other thread can initialize too */
    if (loopback) {
        thread_yield();
    }

    /* send messages to a randomly selected server */
    while (iterations < 0 || iterations > 0) {
        srand(ztimer_now(ZTIMER_USEC));

        /* set-up test parameters */
        uint8_t id = (rand() % servers) + 1;
        uint16_t addr = rand() % COUNT_REGISTERS;
        uint16_t count = rand() % (COUNT_REGISTERS - addr) + 1;

        /* run tests */
        printf("Selected server = %u, address = %u, count = %u\n\n",
               id, addr, count);

        test_write_multiple_holding_registers(id, addr, count);
        test_write_single_holding_register(id, addr);
        test_read_discrete_inputs(id, addr, count);
        test_read_input_registers(id, addr, count);
        test_write_multiple_coils(id, addr, count);
        test_write_single_coil(id, addr);

        test_broadcast(addr, count);
        test_exception(id, addr, count);
        test_timeout();

        /* sleep a little */
        ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

        if (iterations > 0) {
            iterations--;
        }
    }

    return NULL;
}

static void *thread_server(void *arg)
{
    (void)arg;

    params_server_rtu.baudrate = TEST_BAUDRATE;
    params_server_rtu.pin_rts = TEST_SERVER_PIN_RTS;
    params_server_rtu.pin_rts_enable = 1;
    params_server_rtu.uart = UART_DEV(TEST_SERVER_UART);
    params_server_rtu.response_timeout = 1000 * US_PER_MS;

    /* initialize the driver */
    puts("Initializing driver...");

    int res = modbus_init(&server, &modbus_rtu_driver, &server_rtu, &params_server_rtu);

    if (res != 0) {
        printf("Failed, init result is %d\n.", res);
        return NULL;
    }
    else {
        puts("OK");
    }

    /* set-up server */
    server_reg.id = server_id;
    server_reg.flags = MODBUS_SERVER_FLAG_RECEIVE_BROADCAST;
    server_reg.cb = request_cb;
    server_reg.arg = NULL;

    modbus_server_add(&server, &server_reg);

    /* explicit yield to ensure other thread can initialize too */
    if (loopback) {
        thread_yield();
    }

    /* keep listening for requests */
    while (1) {
        puts("Listening for request...");

        res = modbus_server_listen(&server, &message_server);

        if (res == -ENOENT) {
            puts("OK (not for us)");
        }
        else if (res != 0) {
            printf("Failed, receive result is %d\n", res);
        }
        else if (!check_loopback_client()) {
            /* error already printed */
        }
        else {
            puts("OK");
        }

        puts("");
    }

    return NULL;
}

static int cmd_client(int argc, char **argv)
{
    if (argc > 1) {
        servers = atoi(argv[1]);
    }
    else {
        servers = 1;
    }

    if (argc > 2) {
        iterations = atoi(argv[2]);
    }
    else {
        iterations = -1;
    }

    loopback = false;

    printf("Initializing as client with %d servers.\n", servers);

    thread_create(stack_client, sizeof(stack_client),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_client, NULL,
                  "client");

    /* wait forever */
    while (1) {
        thread_yield();
    }

    return 0;
}

static int cmd_server(int argc, char **argv)
{
    if (argc > 1) {
        server_id = atoi(argv[1]);
    }
    else {
        server_id = 1;
    }

    if (argc > 2) {
        misbehave = atoi(argv[2]) > 0 ? 1 : 0;
    }
    else {
        misbehave = false;
    }

    loopback = false;

    printf("Initializing as server with identifier %u.\n", server_id);

    thread_create(stack_server, sizeof(stack_server),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_server, NULL,
                  "server");

    /* wait forever */
    while (1) {
        thread_yield();
    }

    return 0;
}

#if TEST_CLIENT_UART != TEST_SERVER_UART
static int cmd_loopback(int argc, char **argv)
{
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }
    else {
        iterations = -1;
    }

    servers = 1;
    server_id = 1;
    loopback = true;
    misbehave = false;

    puts("Initializing loopback mode.");

    thread_create(stack_server, sizeof(stack_server),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_server, NULL,
                  "server");

    thread_create(stack_client, sizeof(stack_client),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_client, NULL,
                  "client");

    /* wait forever */
    while (1) {
        thread_yield();
    }

    return 0;
}
#endif

static const shell_command_t shell_commands[] = {
    { "client", "Run as client (args: <servers> <iterations>)", cmd_client },
    { "server", "Run as server (args: <id> <misbehave>)", cmd_server },
#if TEST_CLIENT_UART != TEST_SERVER_UART
    { "loopback", "Run in loopback mode (arg: <iterations>)", cmd_loopback },
#endif
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
