/*
 * Copyright (C) 2020-2023 Denis Litvinov <li.denis.iv@gmail.com>
 * Copyright (C) 2023-2024 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for using Modbus as a master, slave or both.
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 *
 * @}
 */

#ifndef TEST_BAUDRATE
#error "TEST_BAUDRATE not defined"
#endif
#ifndef TEST_MASTER_UART
#error "TEST_MASTER_UART not defined"
#endif
#ifndef TEST_MASTER_PIN_RTS
#error "TEST_MASTER_PIN_RTS not defined"
#endif
#ifndef TEST_SLAVE_UART
#error "TEST_SLAVE_UART not defined"
#endif
#ifndef TEST_SLAVE_PIN_RTS
#error "TEST_SLAVE_PIN_RTS not defined"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "periph/gpio.h"

#include "modbus_rtu.h"
#include "msg.h"
#include "shell.h"
#include "thread.h"
#include "timex.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define COUNT_REGISTERS 16

static bool loopback = false;
static int slaves = 1;
static int slave_id = 1;

static char stack_master[THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE];
static kernel_pid_t pid_master;
static modbus_rtu_params_t params_master;
static modbus_rtu_t master;
static uint8_t master_read[COUNT_REGISTERS * 2];
static uint8_t master_write[COUNT_REGISTERS * 2];
static modbus_message_t message_master;

static char stack_slave[THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE];
static kernel_pid_t pid_slave;
static modbus_rtu_params_t params_slave;
static modbus_rtu_t slave;
static uint8_t slave_read_write[COUNT_REGISTERS * 2];
static modbus_message_t message_slave;

/**
 * @brief   Check if two pointers to registers are equal
 */
inline static int compare_regs(const uint8_t *a, const uint8_t *b, uint16_t addr, uint16_t count)
{
    uint8_t offset = modbus_reg_count_to_size(addr);

    return memcmp(a, &(b[offset]), modbus_reg_count_to_size(count));
}

/**
 * @brief   Check if two pointers to bit (coil) strings are equal
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
 * @brief   Basic Modbus request handler that copies from/to the slave buffer
 */
static int request_cb(modbus_message_t *message, void *arg)
{
    (void)arg;

    printf("Request callback, id = %u, function = %u, address = %u, count = %u\n",
           message->id, message->func, message->addr, message->count);

    if (message->id != slave_id) {
        return MODBUS_DROP;
    }

    if (modbus_check_message(message) != MODBUS_OK) {
        message->exc = MODBUS_EXC_ILLEGAL_VALUE;
        return MODBUS_OK;
    }

    if (message->addr >= COUNT_REGISTERS) {
        message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;
        return MODBUS_OK;
    }

    uint8_t reg_offset = modbus_reg_count_to_size(message->addr);
    uint8_t bit_offset = message->addr;

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        modbus_copy_bits(message->data, 0, slave_read_write, bit_offset, message->count, true);
        break;
    case MODBUS_FC_READ_INPUT_REGISTERS:
    case MODBUS_FC_READ_HOLDING_REGISTERS:
        modbus_copy_regs(message->data, &(slave_read_write[reg_offset]), message->count);
        break;
    case MODBUS_FC_WRITE_SINGLE_COIL:
        modbus_copy_bit(slave_read_write, bit_offset, message->data, 0, false);
        break;
    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        modbus_copy_bits(slave_read_write, bit_offset, message->data, 0, message->count, false);
        break;
    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        modbus_copy_reg(&(slave_read_write[reg_offset]), message->data);
        break;
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        modbus_copy_regs(&(slave_read_write[reg_offset]), message->data, message->count);
        break;
    }

    return MODBUS_OK;
}

static void test_reset(bool slave_read_write_random)
{
    /* clear read/write buffers */
    for (unsigned i = 0; i < ARRAY_SIZE(master_read); i++) {
        master_read[i] = 0;
    }
    for (unsigned i = 0; i < ARRAY_SIZE(master_write); i++) {
        master_write[i] = rand();
    }
    for (unsigned i = 0; i < ARRAY_SIZE(slave_read_write); i++) {
        slave_read_write[i] = slave_read_write_random ? rand() : 0;
    }

    /* clear master message */
    memset(&message_master, 0, sizeof(message_master));

    /* clear slave message */
    memset(&message_slave, 0, sizeof(message_slave));
}

static void test_write_multiple_holding_registers(uint8_t id, uint16_t addr, uint16_t count)
{
    puts("Testing MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS...");

    test_reset(false);

    message_master.id = id;
    message_master.func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS;
    message_master.addr = addr;
    message_master.count = count;
    message_master.data = master_write;
    message_master.data_size = sizeof(master_write);

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != 0) {
        printf("Failed, send result is %u\n", res);
    }
    else {
        ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

        if (loopback && (
                message_master.func != message_slave.func ||
                message_master.addr != message_slave.addr ||
                message_master.count != message_slave.count ||
                compare_regs(
                    master_write,
                    slave_read_write,
                    addr,
                    count) != 0)) {
            puts("Failed, slave mismatch");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_HOLDING_REGISTERS...");

        message_master.func = MODBUS_FC_READ_HOLDING_REGISTERS;
        message_master.data = master_read;
        message_master.data_size = sizeof(master_read);

        res = modbus_rtu_send_request(&master, &message_master);

        if (res != 0) {
            printf("Failed, send result is %u\n", res);
        }
        else {
            ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

            if (loopback && (
                    message_master.func != message_slave.func ||
                    message_master.addr != message_slave.addr ||
                    message_master.count != message_slave.count)) {
                puts("Failed, slave mismatch");
            }
            else if (compare_regs(master_read, master_write, 0, count) != 0) {
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

    message_master.id = id;
    message_master.func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER;
    message_master.addr = addr;
    message_master.data = master_write;
    message_master.data_size = sizeof(master_write);

    /* not used, but satisfies the generic loopback check in slave thread */
    message_master.count = 1;

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != 0) {
        printf("Failed, send result is %u\n", res);
    }
    else {
        ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

        if (loopback && (
                message_master.func != message_slave.func ||
                message_master.addr != message_slave.addr ||
                compare_regs(
                    master_write,
                    slave_read_write,
                    addr,
                    1) != 0)) {
            puts("Failed, slave mismatch");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_HOLDING_REGISTERS...");

        message_master.func = MODBUS_FC_READ_HOLDING_REGISTERS;
        message_master.count = 1;
        message_master.data = master_read;
        message_master.data_size = sizeof(master_read);

        res = modbus_rtu_send_request(&master, &message_master);

        if (res != 0) {
            printf("Failed, send result is %u\n", res);
        }
        else {
            ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

            if (loopback && (
                    message_master.func != message_slave.func ||
                    message_master.addr != message_slave.addr)) {
                puts("Failed, slave mismatch");
            }
            else if (compare_regs(master_read, master_write, 0, 1) != 0) {
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

    message_master.id = id;
    message_master.func = MODBUS_FC_READ_DISCRETE_INPUTS;
    message_master.addr = addr;
    message_master.count = count;
    message_master.data = master_read;
    message_master.data_size = sizeof(master_read);

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != 0) {
        printf("Failed, send result is %u\n", res);
    }
    else {
        ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

        if (loopback && (
                message_master.func != message_slave.func ||
                message_master.addr != message_slave.addr ||
                message_master.count != message_slave.count ||
                compare_bits(master_read,
                             slave_read_write,
                             addr,
                             count) != 0)) {
            puts("Failed, slave mismatch");
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

    message_master.id = id;
    message_master.func = MODBUS_FC_READ_INPUT_REGISTERS;
    message_master.addr = addr;
    message_master.count = count;
    message_master.data = master_read;
    message_master.data_size = sizeof(master_read);

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != 0) {
        printf("Failed, send result is %u\n", res);
    }
    else {
        ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

        if (loopback && (
                message_master.func != message_slave.func ||
                message_master.addr != message_slave.addr ||
                message_master.count != message_slave.count ||
                compare_regs(
                    master_read,
                    slave_read_write,
                    addr,
                    count) != 0)) {
            puts("Failed, slave mismatch");
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

    message_master.id = id;
    message_master.func = MODBUS_FC_WRITE_MULTIPLE_COILS;
    message_master.addr = addr;
    message_master.count = count;
    message_master.data = master_write;
    message_master.data_size = sizeof(master_write);

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != 0) {
        printf("Failed, send result is %u\n", res);
    }
    else {
        ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

        if (loopback && (
                message_master.func != message_slave.func ||
                message_master.addr != message_slave.addr ||
                message_master.count != message_slave.count ||
                compare_bits(
                    master_write,
                    slave_read_write,
                    addr,
                    count) != 0)) {
            puts("Failed, slave mismatch");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_COILS...");

        message_master.func = MODBUS_FC_READ_COILS;
        message_master.data = master_read;
        message_master.data_size = sizeof(master_read);

        res = modbus_rtu_send_request(&master, &message_master);

        if (res != 0) {
            printf("Failed, send result is %u\n", res);
        }
        else {
            ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

            if (loopback && (
                    message_master.func != message_slave.func ||
                    message_master.addr != message_slave.addr ||
                    message_master.count != message_slave.count)) {
                puts("Failed, slave mismatch");
            }
            else if (compare_bits(master_read, master_write, 0, message_master.count) != 0) {
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

    message_master.id = id;
    message_master.func = MODBUS_FC_WRITE_SINGLE_COIL;
    message_master.addr = addr;
    message_master.data = master_write;
    message_master.data_size = sizeof(master_write);

    /* not used, but satisfies the generic loopback check in slave thread */
    message_master.count = 1;

    /* single coil data must be 0xff00 or 0x0000 */
    master_write[0] = rand() & 0x01 ? 0xff : 0x00;
    master_write[1] = 0x00;

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != 0) {
        printf("Failed, send result is %u\n", res);
    }
    else {
        ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

        if (loopback && (
                message_master.func != message_slave.func ||
                message_master.addr != message_slave.addr ||
                compare_bits(
                    master_write,
                    slave_read_write,
                    addr,
                    1) != 0)) {
            puts("Failed, slave mismatch");
        }
        else {
            puts("OK");
        }

        puts("");
        puts("Testing MODBUS_FC_READ_COILS...");

        message_master.func = MODBUS_FC_READ_COILS;
        message_master.count = 1;
        message_master.data = master_read;
        message_master.data_size = sizeof(master_read);

        res = modbus_rtu_send_request(&master, &message_master);

        if (res != 0) {
            printf("Failed, send result is %u\n", res);
        }
        else {
            ztimer_sleep(ZTIMER_USEC, master.rx_timeout * 3);

            if (loopback && (
                    message_master.func != message_slave.func ||
                    message_master.addr != message_slave.addr ||
                    message_master.count != message_slave.count)) {
                puts("Failed, slave mismatch");
            }
            else if (compare_bits(master_read, master_write, 0, 1)) {
                puts("Failed, response mismatch");
            }
            else {
                puts("OK");
            }
        }
    }

    puts("");
}

static void test_err_timeout(void)
{
    puts("Testing MODBUS_ERR_TIMEOUT...");

    test_reset(false);

    message_master.id = slaves + 1; /* intentionally non-existing */
    message_master.func = MODBUS_FC_READ_HOLDING_REGISTERS;
    message_master.addr = 1;
    message_master.count = 1;
    message_master.data = master_read;
    message_master.data_size = sizeof(master_read);

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != MODBUS_ERR_TIMEOUT) {
        printf("Failed, send result is %u\n", res);
    }
    else {
        puts("OK");
    }

    puts("");
}

static void test_err_exception(uint8_t id, uint16_t addr, uint16_t count)
{
    puts("Testing MODBUS_ERR_EXCEPTION...");

    test_reset(false);

    message_master.id = id;
    message_master.func = MODBUS_FC_READ_HOLDING_REGISTERS;
    message_master.addr = addr + COUNT_REGISTERS; /* outside address range */
    message_master.count = count;
    message_master.data = master_read;
    message_master.data_size = sizeof(master_read);

    int res = modbus_rtu_send_request(&master, &message_master);

    if (res != MODBUS_OK) {
        printf("Failed, send result is %u\n", res);
    }
    else if (message_master.exc != MODBUS_EXC_ILLEGAL_ADDRESS) {
        puts("Failed, response mismatch");
    }
    else {
        puts("OK");
    }

    puts("");
}

static void *thread_master(void *arg)
{
    (void)arg;

    master.timeout = 1000000;

    params_master.baudrate = TEST_BAUDRATE;
    params_master.pin_rts = TEST_MASTER_PIN_RTS;
    params_master.pin_rts_enable = 1;
    params_master.uart = UART_DEV(TEST_MASTER_UART);

    /* initialize the driver */
    puts("Initializing driver...");

    int res = modbus_rtu_init(&master, &params_master);

    if (res != 0) {
        printf("Failed, init result is %u\n.", res);
        return NULL;
    }
    else {
        puts("OK");
    }

    /* send messages to a randomly selected slave */
    while (1) {
        srand(ztimer_now(ZTIMER_USEC));

        /* set-up test parameters */
        uint8_t id = (rand() % slaves) + 1;
        uint16_t addr = rand() % COUNT_REGISTERS;
        uint16_t count = rand() % (COUNT_REGISTERS - addr) + 1;

        /* run tests */
        printf("Selected slave = %u, address = %u, count = %u\n\n",
               id, addr, count);

        test_write_multiple_holding_registers(id, addr, count);
        test_write_single_holding_register(id, addr);
        test_read_discrete_inputs(id, addr, count);
        test_read_input_registers(id, addr, count);
        test_write_multiple_coils(id, addr, count);
        test_write_single_coil(id, addr);

        test_err_timeout();
        test_err_exception(id, addr, count);

        /* sleep a little */
        //ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    }

    return NULL;
}

static void *thread_slave(void *arg)
{
    (void)arg;

    params_slave.baudrate = TEST_BAUDRATE;
    params_slave.pin_rts = TEST_SLAVE_PIN_RTS;
    params_slave.pin_rts_enable = 1;
    params_slave.uart = UART_DEV(TEST_SLAVE_UART);

    /* initialize the driver */
    puts("Initializing driver...");

    int res = modbus_rtu_init(&slave, &params_slave);

    if (res != 0) {
        printf("Failed, init result is %u\n.", res);
        return NULL;
    }
    else {
        puts("OK");
    }

    /* keep listening for requests */
    while (1) {
        puts("Waiting for request...");

        res = modbus_rtu_recv_request(&slave, &message_slave, request_cb, NULL);

        if (res != 0) {
            printf("Failed, receive result is %u\n", res);
        }
        else if (loopback && (
                     message_slave.func != message_master.func ||
                     message_slave.addr != message_master.addr ||
                     message_slave.count != message_master.count)) {
            puts("Failed, master mismatch");
        }
        else {
            puts("OK");
        }
    }

    return NULL;
}

static int cmd_master(int argc, char **argv)
{
    if (argc > 1) {
        slaves = atoi(argv[1]);
    }

    loopback = false;

    printf("Initializing as master with %d slaves.\n", slaves);

    pid_master = thread_create(stack_master, sizeof(stack_master),
                               THREAD_PRIORITY_MAIN - 1, 0, thread_master, NULL,
                               "master");

    /* wait forever */
    while (1) {
        thread_yield();
    }

    return 0;
}

static int cmd_slave(int argc, char **argv)
{
    if (argc > 1) {
        slave_id = atoi(argv[1]);
    }

    loopback = false;

    printf("Initializing as slave with identifier %u.\n", slave_id);

    pid_slave = thread_create(stack_slave, sizeof(stack_slave),
                              THREAD_PRIORITY_MAIN - 1, 0, thread_slave, NULL,
                              "slave");

    /* wait forever */
    while (1) {
        thread_yield();
    }

    return 0;
}

#if TEST_MASTER_UART != TEST_SLAVE_UART
static int cmd_loopback(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    slaves = 1;
    slave_id = 1;
    loopback = true;

    puts("Initializing loopback mode.");

    pid_slave = thread_create(stack_slave, sizeof(stack_slave),
                              THREAD_PRIORITY_MAIN - 1, 0, thread_slave, NULL,
                              "slave");

    pid_master = thread_create(stack_master, sizeof(stack_master),
                               THREAD_PRIORITY_MAIN - 1, 0, thread_master, NULL,
                               "master");

    /* wait forever */
    while (1) {
        thread_yield();
    }

    return 0;
}
#endif

static const shell_command_t shell_commands[] = {
    { "master", "Run as master (args: <slaves>)", cmd_master },
    { "slave", "Run as slave (args: <id>)", cmd_slave },
#if TEST_MASTER_UART != TEST_SLAVE_UART
    { "loopback", "Run in loopback mode", cmd_loopback },
#endif
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
