/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for using a Modbus transport as a client, server or both.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#if defined(MODBUS_TRANSPORT_ASCII) || defined(MODBUS_TRANSPORT_RTU)
#  ifndef TEST_BAUDRATE
#    error "TEST_BAUDRATE not defined"
#  endif
#  ifndef TEST_CLIENT_UART
#    error "TEST_CLIENT_UART not defined"
#  endif
#  ifndef TEST_CLIENT_PIN_RTS
#    error "TEST_CLIENT_PIN_RTS not defined"
#  endif
#  ifndef TEST_SERVER_UART
#    error "TEST_SERVER_UART not defined"
#  endif
#  ifndef TEST_SERVER_PIN_RTS
#    error "TEST_SERVER_PIN_RTS not defined"
#  endif
#elif defined(MODBUS_TRANSPORT_TCP)
#  ifndef TEST_SERVER_PORT
#    error "TEST_SERVER_PORT not defined"
#  endif
#  ifndef TEST_SERVER_ADDR
#    error "TEST_SERVER_ADDR not defined"
#  endif
#else
#  error "Please enable a Modbus transport module."
#endif

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "shell.h"
#include "thread.h"
#include "timex.h"
#include "tiny_strerror.h"
#include "ztimer.h"

#include "modbus.h"
#if defined(MODBUS_TRANSPORT_ASCII)
#  include "modbus_ascii.h"
#elif defined(MODBUS_TRANSPORT_RTU)
#  include "modbus_rtu.h"
#elif defined(MODBUS_TRANSPORT_TCP)
#  include "net/ipv4/addr.h"
#  include "string_utils.h"
#  include "modbus_tcp.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Whether a single-node loopback mode is possible for this transport
 *
 * For the Modbus ASCII and Modbus RTU, loopback requires two UART peripherals
 * wired together. Modbus TCP has no single-node loopback mode in this test, as
 * the client and server would need a network loopback interface.
 */
#if defined(MODBUS_TRANSPORT_ASCII) || defined(MODBUS_TRANSPORT_RTU)
#  define TEST_LOOPBACK_AVAILABLE   (TEST_CLIENT_UART != TEST_SERVER_UART)
#elif defined(MODBUS_TRANSPORT_TCP)
#  define TEST_LOOPBACK_AVAILABLE   0
#endif

#ifndef TEST_LOOPBACK_AVAILABLE
#  define TEST_LOOPBACK_AVAILABLE   0
#endif

#define COUNT_REGISTERS 16

static bool loopback = false;
static bool fail = false;
static bool misbehave = false;
static int iterations = 0;
static int total_iterations = 0;
static int servers = 1;
static int server_id = 1;

#if defined(MODBUS_TRANSPORT_TCP)
static char server_addr[IPV4_ADDR_MAX_STR_LEN] = TEST_SERVER_ADDR;
#endif

static char stack_client[THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE];
static modbus_t client_bus;
static uint8_t client_read[COUNT_REGISTERS * MODBUS_REGISTER_SIZE];
static uint8_t client_write[COUNT_REGISTERS * MODBUS_REGISTER_SIZE];
static modbus_message_t message_client;

static char stack_server[THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE];
static modbus_t server_bus;
static modbus_server_t server_reg;
static uint8_t server_read_write[COUNT_REGISTERS * MODBUS_REGISTER_SIZE];
static modbus_message_t message_server;

#if defined(MODBUS_TRANSPORT_ASCII)
static modbus_ascii_params_t params_client;
static modbus_ascii_t client_dev;
static modbus_ascii_params_t params_server;
static modbus_ascii_t server_dev;
#elif defined(MODBUS_TRANSPORT_RTU)
static modbus_rtu_params_t params_client;
static modbus_rtu_t client_dev;
static modbus_rtu_params_t params_server;
static modbus_rtu_t server_dev;
#elif defined(MODBUS_TRANSPORT_TCP)
static modbus_tcp_params_t params_client;
static modbus_tcp_t client_dev;
static modbus_tcp_params_t params_server;
static modbus_tcp_t server_dev;
#endif

static modbus_message_t message_server_copy;
static modbus_message_t message_client_copy;

/**
 * @brief   Fail the test and exit.
 *
 * For stability testing, it can be useful to halt the test on the first
 * failure, so this is only done if the `fail` flag is set. Otherwise, the test
 * will continue to run
 */
static void fail_test(void)
{
    if (fail) {
        printf("Test failed after %d iterations.\n", total_iterations);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief   Copy the server message
 *
 * For loopback mode to work properly, assertions from a server thread on the
 * client message (and vice versa) are not reliable, because either thread
 * can already have modified the message in the meantime.
 */
static void copy_message_server(void)
{
    if (TEST_LOOPBACK_AVAILABLE) {
        memcpy(&message_server_copy, &message_server, sizeof(modbus_message_t));
    }
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
    if (TEST_LOOPBACK_AVAILABLE) {
        memcpy(&message_client_copy, &message_client, sizeof(modbus_message_t));
    }
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
    else if (message->addr >= COUNT_REGISTERS ||
             message->count > (uint16_t)(COUNT_REGISTERS - message->addr)) {
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
        printf("Failed, reply result is %s\n", tiny_strerror(res));
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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
            fail_test();
        }
        else if (loopback && compare_regs(
                     client_write,
                     server_read_write,
                     addr,
                     count) != 0) {
            puts("Failed, server mismatch (data)");
            fail_test();
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

        res = modbus_client_request(&client_bus, &message_client);

        if (res != 0) {
            printf("Failed, request result is %s\n", tiny_strerror(res));
            fail_test();
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
                fail_test();
            }
            else if (compare_regs(client_read, client_write, 0, count) != 0) {
                puts("Failed, response mismatch");
                fail_test();
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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
            fail_test();
        }
        else if (loopback && compare_regs(
                     client_write,
                     server_read_write,
                     addr,
                     1) != 0) {
            puts("Failed, server mismatch (data)");
            fail_test();
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

        res = modbus_client_request(&client_bus, &message_client);

        if (res != 0) {
            printf("Failed, request result is %s\n", tiny_strerror(res));
            fail_test();
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
                fail_test();
            }
            else if (compare_regs(client_read, client_write, 0, 1) != 0) {
                puts("Failed, response mismatch");
                fail_test();
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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
            fail_test();
        }
        else if (loopback && compare_bits(client_read,
                                          server_read_write,
                                          addr,
                                          count) != 0) {
            puts("Failed, server mismatch (data)");
            fail_test();
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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
            fail_test();
        }
        else if (loopback && compare_regs(client_read,
                                          server_read_write,
                                          addr,
                                          count) != 0) {
            puts("Failed, server mismatch (data)");
            fail_test();
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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
            fail_test();
        }
        else if (loopback && compare_bits(client_write,
                                          server_read_write,
                                          addr,
                                          count) != 0) {
            puts("Failed, server mismatch (data)");
            fail_test();
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

        res = modbus_client_request(&client_bus, &message_client);

        if (res != 0) {
            printf("Failed, request result is %s\n", tiny_strerror(res));
            fail_test();
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
                fail_test();
            }
            else if (compare_bits(client_read, client_write, 0, message_client.count) != 0) {
                puts("Failed, response mismatch");
                fail_test();
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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        if (!check_loopback_server()) {
            /* error already printed */
            fail_test();
        }
        else if (loopback && compare_bits(client_write,
                                          server_read_write,
                                          addr,
                                          1) != 0) {
            puts("Failed, server mismatch (data)");
            fail_test();
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

        res = modbus_client_request(&client_bus, &message_client);

        if (res != 0) {
            printf("Failed, request result is %s\n", tiny_strerror(res));
            fail_test();
        }
        else {
            if (!check_loopback_server()) {
                /* error already printed */
                fail_test();
            }
            else if (compare_bits(client_read, client_write, 0, 1) != 0) {
                puts("Failed, response mismatch");
                fail_test();
            }
            else {
                puts("OK");
            }
        }
    }

    puts("");
}

static void test_broadcast(uint16_t addr, uint16_t count)
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

    int res = modbus_client_broadcast(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        puts("OK");
    }

    /* wait for at least the response timeout, so that servers have time to
     * process the request and start waiting for next request */
    ztimer_sleep(ZTIMER_USEC, params_client.response_timeout);

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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != 0) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else if (message_client.exc != MODBUS_EXC_ILLEGAL_ADDRESS) {
        puts("Failed, response mismatch");
        fail_test();
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

    int res = modbus_client_request(&client_bus, &message_client);

    if (res != -ETIMEDOUT) {
        printf("Failed, request result is %s\n", tiny_strerror(res));
        fail_test();
    }
    else {
        puts("OK");
    }

    puts("");
}

/**
 * @brief   Fill the transport-specific parameters for the client role
 */
static void configure_client(void)
{
#if defined(MODBUS_TRANSPORT_ASCII)
    params_client.pin_rts = TEST_CLIENT_PIN_RTS;
    params_client.pin_rts_enable = true;
    params_client.uart = UART_DEV(TEST_CLIENT_UART);
    params_client.baudrate = TEST_BAUDRATE;
    params_client.response_timeout = 1000 * US_PER_MS;
    params_client.char_timeout = 0;
#elif defined(MODBUS_TRANSPORT_RTU)
    params_client.pin_rts = TEST_CLIENT_PIN_RTS;
    params_client.pin_rts_enable = true;
    params_client.uart = UART_DEV(TEST_CLIENT_UART);
    params_client.baudrate = TEST_BAUDRATE;
    params_client.response_timeout = 1000 * US_PER_MS;
#elif defined(MODBUS_TRANSPORT_TCP)
    ipv4_addr_t addr;

    params_client.mode = MODBUS_TCP_MODE_CLIENT;
    params_client.response_timeout = 1000 * US_PER_MS;

    /* connect to the configured server address */
    memset(&params_client.endpoint, 0, sizeof(params_client.endpoint));
    params_client.endpoint.family = AF_INET;
    params_client.endpoint.netif = SOCK_ADDR_ANY_NETIF;
    params_client.endpoint.port = TEST_SERVER_PORT;

    if (ipv4_addr_from_str(&addr, server_addr) == NULL) {
        printf("Failed to parse server address '%s'.\n", server_addr);
    }
    else {
        memcpy(params_client.endpoint.addr.ipv4, &addr, sizeof(addr));
    }
#endif
}

/**
 * @brief   Fill the transport-specific parameters for the server role
 */
static void configure_server(void)
{
#if defined(MODBUS_TRANSPORT_ASCII)
    params_server.pin_rts = TEST_SERVER_PIN_RTS;
    params_server.pin_rts_enable = true;
    params_server.uart = UART_DEV(TEST_SERVER_UART);
    params_server.baudrate = TEST_BAUDRATE;
    params_server.response_timeout = 1000 * US_PER_MS;
    params_server.char_timeout = 0;
#elif defined(MODBUS_TRANSPORT_RTU)
    params_server.pin_rts = TEST_SERVER_PIN_RTS;
    params_server.pin_rts_enable = true;
    params_server.uart = UART_DEV(TEST_SERVER_UART);
    params_server.baudrate = TEST_BAUDRATE;
    params_server.response_timeout = 1000 * US_PER_MS;
#elif defined(MODBUS_TRANSPORT_TCP)
    params_server.mode = MODBUS_TCP_MODE_SERVER;
    params_server.response_timeout = 1000 * US_PER_MS;
    params_server.idle_timeout = 30 * US_PER_SEC;

    /* listen on all interfaces */
    memset(&params_server.endpoint, 0, sizeof(params_server.endpoint));
    params_server.endpoint.family = AF_INET;
    params_server.endpoint.netif = SOCK_ADDR_ANY_NETIF;
    params_server.endpoint.port = TEST_SERVER_PORT;
#endif
}

static void *thread_client(void *arg)
{
    (void)arg;

    configure_client();

    /* initialize the driver */
    puts("Initializing driver...");

#if defined(MODBUS_TRANSPORT_ASCII)
    int res = modbus_init(&client_bus, &modbus_ascii_transport, &client_dev, &params_client);
#elif defined(MODBUS_TRANSPORT_RTU)
    int res = modbus_init(&client_bus, &modbus_rtu_transport, &client_dev, &params_client);
#elif defined(MODBUS_TRANSPORT_TCP)
    int res = modbus_init(&client_bus, &modbus_tcp_transport, &client_dev, &params_client);
#endif

    if (res != 0) {
        printf("Failed, init result is %s.\n", tiny_strerror(res));
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
    while (iterations != 0) {
        total_iterations++;

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

    configure_server();

    /* initialize the driver */
    puts("Initializing driver...");

#if defined(MODBUS_TRANSPORT_ASCII)
    int res = modbus_init(&server_bus, &modbus_ascii_transport, &server_dev, &params_server);
#elif defined(MODBUS_TRANSPORT_RTU)
    int res = modbus_init(&server_bus, &modbus_rtu_transport, &server_dev, &params_server);
#elif defined(MODBUS_TRANSPORT_TCP)
    int res = modbus_init(&server_bus, &modbus_tcp_transport, &server_dev, &params_server);
#endif

    if (res != 0) {
        printf("Failed, init result is %s.\n", tiny_strerror(res));
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

    modbus_server_add(&server_bus, &server_reg);

    /* explicit yield to ensure other thread can initialize too */
    if (loopback) {
        thread_yield();
    }

    /* keep listening for requests */
    while (1) {
        puts("Listening for request...");

        res = modbus_server_listen(&server_bus, &message_server);

        if (res == -ENOENT) {
            puts("OK (not for us)");
        }
        else if (res != 0) {
            printf("Failed, receive result is %s\n", tiny_strerror(res));
        }
        else if (!check_loopback_client()) {
            /* error already printed */
            fail_test();
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
#if defined(MODBUS_TRANSPORT_TCP)
    if (argc > 1) {
        ipv4_addr_t addr;

        if (ipv4_addr_from_str(&addr, argv[1]) == NULL) {
            printf("Failed to parse server address '%s'.\n", argv[1]);
            return 1;
        }

        strscpy(server_addr, argv[1], sizeof(server_addr));
    }
    else {
        strscpy(server_addr, TEST_SERVER_ADDR, sizeof(server_addr));
    }

    servers = 1;
#else
    if (argc > 1) {
        servers = atoi(argv[1]);

        if (servers < 1) {
            printf("Invalid number of servers '%s'.\n", argv[1]);
            return 1;
        }
    }
    else {
        servers = 1;
    }
#endif

    if (argc > 2) {
        fail = atoi(argv[2]) > 0;
    }
    else {
        fail = false;
    }

    if (argc > 3) {
        iterations = atoi(argv[3]);
    }
    else {
        iterations = -1;
    }

    loopback = false;

#if defined(MODBUS_TRANSPORT_TCP)
    printf("Initializing as client, connecting to %s.\n", server_addr);
#else
    printf("Initializing as client with %d servers.\n", servers);
#endif

    /* run above the shell so requests are served while the shell blocks */
    thread_create(stack_client, sizeof(stack_client),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_client, NULL,
                  "client");

    /* wait forever */
    while (1) {
        thread_sleep();
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
        fail = atoi(argv[2]) > 0;
    }
    else {
        fail = false;
    }

    if (argc > 3) {
        misbehave = atoi(argv[3]) > 0;
    }
    else {
        misbehave = false;
    }

    loopback = false;

    printf("Initializing as server with identifier %d.\n", server_id);

    /* run above the shell so requests are served while the shell blocks */
    thread_create(stack_server, sizeof(stack_server),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_server, NULL,
                  "server");

    /* wait forever */
    while (1) {
        thread_sleep();
    }

    return 0;
}

#if TEST_LOOPBACK_AVAILABLE
static int cmd_loopback(int argc, char **argv)
{
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }
    else {
        iterations = -1;
    }

    if (argc > 2) {
        fail = atoi(argv[2]) > 0;
    }
    else {
        fail = false;
    }

    servers = 1;
    server_id = 1;
    loopback = true;
    misbehave = false;

    puts("Initializing loopback mode.");

    /* run above the shell so requests are served while the shell blocks */
    thread_create(stack_server, sizeof(stack_server),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_server, NULL,
                  "server");

    thread_create(stack_client, sizeof(stack_client),
                  THREAD_PRIORITY_MAIN - 1, 0, thread_client, NULL,
                  "client");

    /* wait forever */
    while (1) {
        thread_sleep();
    }

    return 0;
}
#endif

#if defined(MODBUS_TRANSPORT_TCP)
SHELL_COMMAND(client, "Run as client (args: <ip> <fail> <iterations>)", cmd_client);
#elif defined(MODBUS_TRANSPORT_ASCII) || defined(MODBUS_TRANSPORT_RTU)
SHELL_COMMAND(client, "Run as client (args: <servers> <fail> <iterations>)", cmd_client);
#endif
SHELL_COMMAND(server, "Run as server (args: <id> <fail> <misbehave>)", cmd_server);

#if TEST_LOOPBACK_AVAILABLE
SHELL_COMMAND(loopback, "Run in loopback mode (args: <iterations> <fail>)", cmd_loopback);
#endif

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
