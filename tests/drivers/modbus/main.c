/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Modbus driver tests
 *
 *
 * @author  Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <string.h>

#include "embUnit.h"

#include "modbus.h"

/**
 * @brief Mock Modbus device structure for unit testing purposes. It provides
 *        separate buffers for asserting requests and responses.
 */
typedef struct {
    modbus_t *bus;

    int init_called;

    uint8_t send_buf[32];
    uint8_t send_len;

    uint8_t recv_buf[32];
    uint8_t recv_len;

    bool recv_broadcast;
} modbus_mock_t;

static int _mock_init(modbus_t *bus)
{
    modbus_mock_t *dev = (modbus_mock_t *)bus->dev;

    dev->bus = bus;
    dev->init_called++;

    return 0;
}

static int _mock_send(modbus_t *bus, modbus_message_t *message, uint8_t flags)
{
    (void)message;
    (void)flags;

    modbus_mock_t *dev = (modbus_mock_t *)bus->dev;

    /* the send buffer is much smaller and should be sufficient for unit
     * testing purposes */
    assert(bus->buffer_size <= sizeof(dev->send_buf));

    memcpy(dev->send_buf, bus->buffer, bus->buffer_size);
    dev->send_len = bus->buffer_size;

    return 0;
}

static int _mock_recv(modbus_t *bus, modbus_message_t *message, size_t size, uint8_t flags)
{
    modbus_mock_t *dev = (modbus_mock_t *)bus->dev;

    /* by default, set the ID of any received message to 1 (or broadcast address) */
    if (flags & MODBUS_RECV_FLAG_START) {
        dev->bus->buffer_size = 0;

        if (dev->recv_broadcast) {
            message->id = MODBUS_ID_BROADCAST;
        }
        else {
            message->id = 1;
        }
    }

    /* simulate a time-out */
    if ((dev->bus->buffer_size + size) > dev->recv_len) {
        return -ETIMEDOUT;
    }

    memcpy(bus->buffer + dev->bus->buffer_size, dev->recv_buf + dev->bus->buffer_size, size);
    dev->bus->buffer_size += size;

    return 0;
}

/**
 * @brief Mock Modbus driver descriptor for unit testing purposes.
 */
static const modbus_desc_t _mock_driver = {
    .init = _mock_init,
    .send = _mock_send,
    .recv = _mock_recv,
};

/**
 * @brief Buffer for server responses in unit testing purposes.
 */
static uint8_t server_buffer[16];

/**
 * @brief Server callback function for unit testing purposes.
 */
static void server_callback_noreturn(modbus_t *modbus, modbus_server_t *server,
                                     modbus_message_t *message)
{
    /* change the data pointer to an user-provided buffer if a non-zero-copy
     * test is executed */
    if (server->arg != NULL) {
        bool *zero_copy = server->arg;

        if (*zero_copy == false) {
            message->data = server_buffer;
            message->data_size = sizeof(server_buffer);
        }
    }

    /* request should be valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(message));

    /* exception should not be set for a request */
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message->exc);

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
        /* return 31 coils starting at address 0x001D */
        TEST_ASSERT_EQUAL_INT(0x001D, message->addr);
        TEST_ASSERT_EQUAL_INT(31, message->count);
        TEST_ASSERT(message->data_size >= 4);
        message->data[0] = 0xCD;
        message->data[1] = 0x6B;
        message->data[2] = 0xB2;
        message->data[3] = 0x7F;
        break;

    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* return 28 discrete inputs starting at address 0x007A */
        TEST_ASSERT_EQUAL_INT(0x007A, message->addr);
        TEST_ASSERT_EQUAL_INT(28, message->count);
        TEST_ASSERT(message->data_size >= 4);
        message->data[0] = 0xAC;
        message->data[1] = 0xDB;
        message->data[2] = 0xFB;
        message->data[3] = 0x0D;
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
        /* return 3 holding registers starting at address 0x006F */
        TEST_ASSERT_EQUAL_INT(0x006F, message->addr);
        TEST_ASSERT_EQUAL_INT(3, message->count);
        TEST_ASSERT(message->data_size >= 6);
        message->data[0] = 0xAE;
        message->data[1] = 0x41;
        message->data[2] = 0x56;
        message->data[3] = 0x52;
        message->data[4] = 0x43;
        message->data[5] = 0x40;
        break;

    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* return 1 input register starting at address 0x000A */
        TEST_ASSERT_EQUAL_INT(0x000A, message->addr);
        TEST_ASSERT_EQUAL_INT(1, message->count);
        TEST_ASSERT(message->data_size >= 2);
        message->data[0] = 0x10;
        message->data[1] = 0x2F;
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        /* echo the address and value, but assert the request */
        TEST_ASSERT_EQUAL_INT(0x00BF, message->addr);
        TEST_ASSERT_EQUAL_INT(1, message->count);
        TEST_ASSERT_EQUAL_INT(2, message->data_size);
        TEST_ASSERT_EQUAL_INT(0x00, message->data[0]);
        TEST_ASSERT_EQUAL_INT(0x00, message->data[1]);
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* echo the address and value, but assert the request */
        TEST_ASSERT_EQUAL_INT(0x0004, message->addr);
        TEST_ASSERT_EQUAL_INT(1, message->count);
        TEST_ASSERT_EQUAL_INT(2, message->data_size);
        TEST_ASSERT_EQUAL_INT(0xAB, message->data[0]);
        TEST_ASSERT_EQUAL_INT(0xCD, message->data[1]);
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        /* echo the address and value, but assert the request */
        TEST_ASSERT_EQUAL_INT(0x001B, message->addr);
        TEST_ASSERT_EQUAL_INT(9, message->count);
        TEST_ASSERT_EQUAL_INT(2, message->data_size);
        TEST_ASSERT_EQUAL_INT(0x4D, message->data[0]);
        TEST_ASSERT_EQUAL_INT(0x01, message->data[1]);
        break;

    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* echo the address and value, but assert the request */
        TEST_ASSERT_EQUAL_INT(0x0012, message->addr);
        TEST_ASSERT_EQUAL_INT(2, message->count);
        TEST_ASSERT_EQUAL_INT(4, message->data_size);
        TEST_ASSERT_EQUAL_INT(0x0B, message->data[0]);
        TEST_ASSERT_EQUAL_INT(0x0A, message->data[1]);
        TEST_ASSERT_EQUAL_INT(0xC1, message->data[2]);
        TEST_ASSERT_EQUAL_INT(0x02, message->data[3]);
        break;

    default:
        message->exc = MODBUS_EXC_ILLEGAL_FUNCTION;
        break;
    }

    /* response should be valid too */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(message));

    /* send reply */
    TEST_ASSERT_EQUAL_INT(0, modbus_server_reply(modbus, message));
}

static int server_callback(modbus_t *modbus, modbus_server_t *server,
                           modbus_message_t *message)
{
    /* assertions cannot be performed in a method with a return type */
    server_callback_noreturn(modbus, server, message);

    return 0;
}

/**
 * @brief Server callback that implemented no function (returns illegal function).
 */
static int server_callback_illegal_function(modbus_t *modbus, modbus_server_t *server,
                                            modbus_message_t *message)
{
    (void)modbus;
    (void)server;

    /* set exception to illegal function for any request */
    message->exc = MODBUS_EXC_ILLEGAL_FUNCTION;

    return modbus_server_reply(modbus, message);
}

/**
 * @brief Server callback that returns an exception for any request.
 */
static int server_callback_exception(modbus_t *modbus, modbus_server_t *server,
                                     modbus_message_t *message)
{
    (void)server;

    /* set exception to illegal address for any request */
    message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;

    return modbus_server_reply(modbus, message);
}

/**
 * @brief Server callback that does not send a reply (for broadcast tests).
 */
static int server_callback_no_reply(modbus_t *modbus, modbus_server_t *server,
                                    modbus_message_t *message)
{
    (void)modbus;
    (void)server;
    (void)message;

    /* for broadcast, do nothing and don't send a reply */
    return 0;
}

static void test_modbus_init(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    int ret = modbus_init(&bus, &_mock_driver, &dev, NULL);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT(bus.driver == &_mock_driver);
    TEST_ASSERT_EQUAL_INT(1, dev.init_called);
}

static void test_modbus_server_registry(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    modbus_init(&bus, &_mock_driver, &dev, NULL);

    modbus_server_t a = { .id = 10, .flags = MODBUS_SERVER_FLAG_RECEIVE_BROADCAST };
    modbus_server_t b = { .id = 20 };
    modbus_server_t c = { .id = 30, .flags = MODBUS_SERVER_FLAG_RECEIVE_BROADCAST };
    modbus_server_t d = { .id = 40, .flags = MODBUS_SERVER_FLAG_RECEIVE_ANY };

    /* add */
    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &a));
    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &b));
    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &c));
    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &d));

    /* get */
    TEST_ASSERT(modbus_server_get(&bus, 10) == &a);
    TEST_ASSERT(modbus_server_get(&bus, 20) == &b);
    TEST_ASSERT(modbus_server_get(&bus, 30) == &c);
    TEST_ASSERT(modbus_server_get(&bus, 40) == &d);
    TEST_ASSERT(modbus_server_get(&bus, 50) == NULL);

    /* iter */
    modbus_server_t *server;

    server = modbus_server_iter(&bus, NULL, 20);
    TEST_ASSERT(server == &d);
    server = modbus_server_iter(&bus, server, 20);
    TEST_ASSERT(server == &b);
    server = modbus_server_iter(&bus, server, 20);
    TEST_ASSERT(server == NULL);

    server = modbus_server_iter(&bus, NULL, MODBUS_ID_BROADCAST);
    TEST_ASSERT(server == &d);
    server = modbus_server_iter(&bus, server, MODBUS_ID_BROADCAST);
    TEST_ASSERT(server == &c);
    server = modbus_server_iter(&bus, server, MODBUS_ID_BROADCAST);
    TEST_ASSERT(server == &a);
    server = modbus_server_iter(&bus, server, MODBUS_ID_BROADCAST);
    TEST_ASSERT(server == NULL);

    /* remove */
    TEST_ASSERT_EQUAL_INT(0, modbus_server_remove(&bus, &a));
    TEST_ASSERT(modbus_server_get(&bus, 10) == NULL);
    TEST_ASSERT(modbus_server_get(&bus, 20) == &b);
}

static void test_modbus_server_registry__duplicate_id(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    modbus_init(&bus, &_mock_driver, &dev, NULL);

    modbus_server_t a = { .id = 10 };
    modbus_server_t b = { .id = 10 };

    /* add two servers with the same ID; newest should shadow the previous */
    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &a));
    TEST_ASSERT_EQUAL_INT(-EEXIST, modbus_server_add(&bus, &b));
}

static void test_modbus_server_registry__remove_nonexistent(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    modbus_init(&bus, &_mock_driver, &dev, NULL);

    modbus_server_t a = { .id = 1 };
    modbus_server_t b = { .id = 2 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &a));

    /* removing a server that was never added return -ENOENT */
    TEST_ASSERT_EQUAL_INT(-ENOENT, modbus_server_remove(&bus, &b));

    /* the existing server must still be present */
    TEST_ASSERT(modbus_server_get(&bus, 1) == &a);
}

static void test_modbus_bit_count_to_size(void)
{
    TEST_ASSERT_EQUAL_INT(0, modbus_bit_count_to_size(0));
    TEST_ASSERT_EQUAL_INT(1, modbus_bit_count_to_size(1));
    TEST_ASSERT_EQUAL_INT(1, modbus_bit_count_to_size(8));
    TEST_ASSERT_EQUAL_INT(2, modbus_bit_count_to_size(9));
    TEST_ASSERT_EQUAL_INT(2, modbus_bit_count_to_size(16));
    TEST_ASSERT_EQUAL_INT(3, modbus_bit_count_to_size(17));
}

static void test_modbus_reg_count_to_size(void)
{
    TEST_ASSERT_EQUAL_INT(0, modbus_reg_count_to_size(0));
    TEST_ASSERT_EQUAL_INT(2, modbus_reg_count_to_size(1));
    TEST_ASSERT_EQUAL_INT(4, modbus_reg_count_to_size(2));
}

static void test_modbus_copy_bits(void)
{
    uint8_t src[2] = { 0xAA, 0xCC }; /* 0b10101010, 0b11001100 */
    uint8_t dst[2] = { 0xFF, 0xFF };

    /* copy 8 bits starting at 0 into dst[0], pad zeroes */
    modbus_copy_bits(dst, 0, src, 0, 8, true);
    TEST_ASSERT_EQUAL_INT(0xAA, dst[0]);

    /* copy single bit (bit 3 from src[0]) into bit 0 of dst[1], pad zeroes */
    modbus_copy_bit(&dst[1], 0, &src[0], 3, true);
    TEST_ASSERT_EQUAL_INT(0x01, dst[1]);
}

static void test_modbus_copy_regs(void)
{
    uint16_t src16[2] = { 0x1234, 0xABCD };
    uint16_t dst16[2] = { 0, 0 };

    modbus_copy_regs(dst16, src16, 2);
    TEST_ASSERT_EQUAL_INT(0, memcmp(src16, dst16, sizeof(src16)));

    uint16_t single = 0;
    modbus_copy_reg(&single, &src16[1]);
    TEST_ASSERT_EQUAL_INT(0xABCD, single);
}

static void test_modbus_check_message__invalid_id(void)
{
    modbus_message_t msg = {
        .id = 255,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__invalid_function(void)
{
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_NONE,
        .addr = 0,
        .count = 1
    };

    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__exception(void)
{
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .exc = MODBUS_EXC_ILLEGAL_ADDRESS,
        .addr = 0,
        .count = 1
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));
}

static void test_modbus_check_message__exception_mask(void)
{
    modbus_message_t msg = {
        .id = 1,
        .func = 0x81, /* function code with exception bit set */
        .exc = MODBUS_EXC_NONE,
        .addr = 0,
        .count = 1
    };

    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__broadcast_exception(void)
{
    modbus_message_t msg = {
        .id = MODBUS_ID_BROADCAST,
        .func = MODBUS_FC_READ_COILS,
        .exc = MODBUS_EXC_ILLEGAL_ADDRESS,
        .addr = 0,
        .count = 1
    };

    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__read_coils(void)
{
    uint8_t buf8[8] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0,
        .count = 1,
        .data = buf8,
        .data_size = sizeof(buf8),
    };

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* count */
    msg.count = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 2001;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 10;
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small data */
    msg.data = buf8;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__read_discrete_inputs(void)
{
    uint8_t buf8[8] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0,
        .count = 1,
        .data = buf8,
        .data_size = sizeof(buf8),
    };

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* count */
    msg.count = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 2001;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 9;
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small data */
    msg.data = buf8;
    msg.data_size = 1; /* needs 2 */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__read_input_registers(void)
{
    uint8_t buf8[8] = { 0 };
    uint8_t buf2[2] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0,
        .count = 1,
        .data = buf8,
        .data_size = sizeof(buf8),
    };

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* invalid count */
    msg.count = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 126;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 2;
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small data */
    msg.data = buf2;
    msg.data_size = sizeof(buf2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__read_holding_registers(void)
{
    uint8_t buf8[8] = { 0 };
    uint8_t buf2[2] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0,
        .count = 1,
        .data = buf8,
        .data_size = sizeof(buf8),
    };

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* unexpected count */
    msg.count = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 126;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 2;
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small data */
    msg.data = buf2; /* too small */
    msg.data_size = sizeof(buf2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__write_multiple_coils(void)
{
    uint8_t buf8[8] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_MULTIPLE_COILS,
        .addr = 0,
        .count = 1,
        .data = buf8,
        .data_size = sizeof(buf8),
    };

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* unexpected count */
    msg.count = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 1969;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small data */
    msg.count = 9;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__write_multiple_holding_registers(void)
{
    uint8_t buf8[8] = { 0 };
    uint8_t buf2[2] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS,
        .addr = 0,
        .count = 1,
        .data = buf8,
        .data_size = sizeof(buf8),
    };

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* unexpected count */
    msg.count = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 126;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 2;
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small data */
    msg.data = buf2; /* needs 4 */
    msg.data_size = sizeof(buf2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__write_single_coil(void)
{
    uint8_t buf2[2] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0,
        .count = 1,
        .data = buf2,
        .data_size = sizeof(buf2),
    };

    buf2[0] = 0xFF;
    buf2[1] = 0x00;

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* null data */
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small */
    msg.data = buf2;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* invalid payload */
    buf2[0] = 0x12;
    buf2[1] = 0x34;
    msg.data_size = sizeof(buf2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_check_message__write_single_holding_register(void)
{
    uint8_t buf2[2] = { 0 };

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER,
        .addr = 0,
        .count = 1,
        .data = buf2,
        .data_size = sizeof(buf2),
    };

    /* valid */
    TEST_ASSERT_EQUAL_INT(0, modbus_check_message(&msg));

    /* null data */
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small */
    msg.data = buf2;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));
}

static void test_modbus_client_request__read_coils(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;
    uint8_t data[4];

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31
    };

    if (!zero_copy) {
        msg.data = data;
        msg.data_size = sizeof(data);
    }

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    if (!zero_copy) {
        TEST_ASSERT(msg.data == data);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x001D, msg.addr);
    TEST_ASSERT_EQUAL_INT(31, msg.count);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xCD, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0x6B, msg.data[1]);
    TEST_ASSERT_EQUAL_INT(0xB2, msg.data[2]);
    TEST_ASSERT_EQUAL_INT(0x7F, msg.data[3]);
}

static void test_modbus_client_request__read_coils__with_zero_copy(void)
{
    test_modbus_client_request__read_coils(true);
}

static void test_modbus_client_request__read_coils__without_zero_copy(void)
{
    test_modbus_client_request__read_coils(false);
}

static void test_modbus_client_request__read_discrete_inputs(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;
    uint8_t data[4];

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28
    };

    if (!zero_copy) {
        msg.data = data;
        msg.data_size = sizeof(data);
    }

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x02, 0x04, 0xAC, 0xDB, 0xFB, 0x0D };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x02, 0x00, 0x7A, 0x00, 0x1C };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    if (!zero_copy) {
        TEST_ASSERT(msg.data == data);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_DISCRETE_INPUTS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x007A, msg.addr);
    TEST_ASSERT_EQUAL_INT(28, msg.count);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xAC, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0xDB, msg.data[1]);
    TEST_ASSERT_EQUAL_INT(0xFB, msg.data[2]);
    TEST_ASSERT_EQUAL_INT(0x0D, msg.data[3]);
}

static void test_modbus_client_request__read_discrete_inputs__with_zero_copy(void)
{
    test_modbus_client_request__read_discrete_inputs(true);
}

static void test_modbus_client_request__read_discrete_inputs__without_zero_copy(void)
{
    test_modbus_client_request__read_discrete_inputs(false);
}

static void test_modbus_client_request__read_holding_registers(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;
    uint8_t data[6];

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3
    };

    if (!zero_copy) {
        msg.data = data;
        msg.data_size = sizeof(data);
    }

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert ADU sent by client */
    uint8_t client_request_pdu[] = { 0x03, 0x00, 0x6F, 0x00, 0x03 };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    if (!zero_copy) {
        TEST_ASSERT(msg.data == data);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_HOLDING_REGISTERS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x006F, msg.addr);
    TEST_ASSERT_EQUAL_INT(3, msg.count);
    TEST_ASSERT_EQUAL_INT(6, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xAE, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0x41, msg.data[1]);
    TEST_ASSERT_EQUAL_INT(0x56, msg.data[2]);
    TEST_ASSERT_EQUAL_INT(0x52, msg.data[3]);
    TEST_ASSERT_EQUAL_INT(0x43, msg.data[4]);
    TEST_ASSERT_EQUAL_INT(0x40, msg.data[5]);
}

static void test_modbus_client_request__read_holding_registers__with_zero_copy(void)
{
    test_modbus_client_request__read_holding_registers(true);
}

static void test_modbus_client_request__read_holding_registers__without_zero_copy(void)
{
    test_modbus_client_request__read_holding_registers(false);
}

static void test_modbus_client_request__read_input_registers(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;
    uint8_t data[2];

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1
    };

    if (!zero_copy) {
        msg.data = data;
        msg.data_size = sizeof(data);
    }

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x04, 0x02, 0x10, 0x2F };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x04, 0x00, 0x0A, 0x00, 0x01 };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    if (!zero_copy) {
        TEST_ASSERT(msg.data == data);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_INPUT_REGISTERS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x000A, msg.addr);
    TEST_ASSERT_EQUAL_INT(1, msg.count);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0x10, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0x2F, msg.data[1]);
}

static void test_modbus_client_request__read_input_registers__with_zero_copy(void)
{
    test_modbus_client_request__read_input_registers(true);
}

static void test_modbus_client_request__read_input_registers__without_zero_copy(void)
{
    test_modbus_client_request__read_input_registers(false);
}

static void test_modbus_client_request__write_single_coil(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    uint8_t msg_data[] = { 0x00, 0x00 };
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
        .data = msg_data,
        .data_size = sizeof(msg_data)
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x05, 0x00, 0xBF, 0x00, 0x00 };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x05, 0x00, 0xBF, 0x00, 0x00 };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_COIL, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x00BF, msg.addr);
    TEST_ASSERT_EQUAL_INT(1, msg.count);
}

static void test_modbus_client_request__write_single_holding_register(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    uint8_t msg_data[] = { 0xAB, 0xCD };
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER,
        .addr = 0x0004,
        .count = 1,
        .data = msg_data,
        .data_size = sizeof(msg_data)
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x0004, msg.addr);
    TEST_ASSERT_EQUAL_INT(1, msg.count);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xAB, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0xCD, msg.data[1]);
}

static void test_modbus_client_request__write_multiple_coils(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    uint8_t msg_data[] = { 0x4D, 0x01 };
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_MULTIPLE_COILS,
        .addr = 0x001B,
        .count = 9,
        .data = msg_data,
        .data_size = sizeof(msg_data)
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09 };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09, 0x02, 0x4D, 0x01 };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x001B, msg.addr);
    TEST_ASSERT_EQUAL_INT(9, msg.count);
}

static void test_modbus_client_request__write_multiple_holding_registers(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    uint8_t msg_data[] = { 0x0B, 0x0A, 0xC1, 0x02 };
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS,
        .addr = 0x0012,
        .count = 2,
        .data = msg_data,
        .data_size = sizeof(msg_data)
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x10, 0x00, 0x12, 0x00, 0x02 };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x10, 0x00, 0x12, 0x00, 0x02, 0x04, 0x0B, 0x0A, 0xC1, 0x02 };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x0012, msg.addr);
    TEST_ASSERT_EQUAL_INT(2, msg.count);
}

static void test_modbus_client_request__illegal_function_code(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message with unknown function code */
    modbus_message_t msg = {
        .id = 1,
        .func = 0x2B,  /* unknown function code */
        .addr = 0x0000,
        .count = 1
    };

    /* mock exception response from server */
    /* 0xAB = 0x2B | 0x80, exception code 0x01 = illegal function */
    uint8_t server_response_pdu[] = { 0xAB, 0x01 };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_client_request(&client, &msg));
}

static void test_modbus_client_request__exception(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x0100,
        .count = 10
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x81, 0x02 };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x01, 0x01, 0x00, 0x00, 0x0A };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* assert exception in response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_ILLEGAL_ADDRESS, msg.exc);
}

static void test_modbus_client_request__invalid_exception(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x0100,
        .count = 10
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x81, 0x00 }; /* 0x00 is not a valid exception code */
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_client_request(&client, &msg));
}

static void test_modbus_client_request__wrong_id(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x02, 0x04, 0xCD, 0x6B, 0xB2, 0x7F }; /* id = 2 instead of 1 */
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu);

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_client_request(&client, &msg));
}

static void test_modbus_client_request__broadcast(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the broadcast request message */
    uint8_t msg_data[] = { 0xFF, 0x00 };
    modbus_message_t msg = {
        .id = MODBUS_ID_BROADCAST, /* will also be set by modbus_client_broadcast */
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x0050,
        .count = 1,
        .data = msg_data,
        .data_size = sizeof(msg_data)
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_client_broadcast(&client, &msg));

    /* assert PDU sent by client */
    uint8_t client_request_pdu[] = { 0x05, 0x00, 0x50, 0xFF, 0x00 };

    TEST_ASSERT_EQUAL_INT(sizeof(client_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, client_request_pdu, sizeof(client_request_pdu)));

    /* for broadcast, no response is expected */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_COIL, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
}

static void test_modbus_client_request__timeout(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_driver, &dev, NULL);

    /* the request message */
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31
    };

    /* mock response that could be returned by the server */
    uint8_t server_response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, server_response_pdu, sizeof(server_response_pdu));
    dev.recv_len = sizeof(server_response_pdu) - 1; /* returning one byte short */

    TEST_ASSERT_EQUAL_INT(-ETIMEDOUT, modbus_client_request(&client, &msg));
}

static void test_modbus_server_listen__read_coils(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = &zero_copy;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    if (!zero_copy) {
        TEST_ASSERT(message.data == server_buffer);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x001D, message.addr);
    TEST_ASSERT_EQUAL_INT(31, message.count);
    TEST_ASSERT_EQUAL_INT(4, message.data_size);
    TEST_ASSERT_EQUAL_INT(0xCD, message.data[0]);
    TEST_ASSERT_EQUAL_INT(0x6B, message.data[1]);
    TEST_ASSERT_EQUAL_INT(0xB2, message.data[2]);
    TEST_ASSERT_EQUAL_INT(0x7F, message.data[3]);

    /* assert server response PDU */
    uint8_t expected_response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__read_coils__with_zero_copy(void)
{
    test_modbus_server_listen__read_coils(true);
}

static void test_modbus_server_listen__read_coils__without_zero_copy(void)
{
    test_modbus_server_listen__read_coils(false);
}

static void test_modbus_server_listen__read_discrete_inputs(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = &zero_copy;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x02, 0x00, 0x7A, 0x00, 0x1C };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    if (!zero_copy) {
        TEST_ASSERT(message.data == server_buffer);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_DISCRETE_INPUTS, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x007A, message.addr);
    TEST_ASSERT_EQUAL_INT(28, message.count);
    TEST_ASSERT_EQUAL_INT(4, message.data_size);
    TEST_ASSERT_EQUAL_INT(0xAC, message.data[0]);
    TEST_ASSERT_EQUAL_INT(0xDB, message.data[1]);
    TEST_ASSERT_EQUAL_INT(0xFB, message.data[2]);
    TEST_ASSERT_EQUAL_INT(0x0D, message.data[3]);

    /* assert server response PDU */
    uint8_t expected_response_pdu[] = { 0x02, 0x04, 0xAC, 0xDB, 0xFB, 0x0D };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__read_discrete_inputs__with_zero_copy(void)
{
    test_modbus_server_listen__read_discrete_inputs(true);
}

static void test_modbus_server_listen__read_discrete_inputs__without_zero_copy(void)
{
    test_modbus_server_listen__read_discrete_inputs(false);
}

static void test_modbus_server_listen__read_holding_registers(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = &zero_copy;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x03, 0x00, 0x6F, 0x00, 0x03 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    if (!zero_copy) {
        TEST_ASSERT(message.data == server_buffer);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_HOLDING_REGISTERS, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x006F, message.addr);
    TEST_ASSERT_EQUAL_INT(3, message.count);
    TEST_ASSERT_EQUAL_INT(6, message.data_size);
    TEST_ASSERT_EQUAL_INT(0xAE, message.data[0]);
    TEST_ASSERT_EQUAL_INT(0x41, message.data[1]);
    TEST_ASSERT_EQUAL_INT(0x56, message.data[2]);
    TEST_ASSERT_EQUAL_INT(0x52, message.data[3]);
    TEST_ASSERT_EQUAL_INT(0x43, message.data[4]);
    TEST_ASSERT_EQUAL_INT(0x40, message.data[5]);

    /* assert server response PDU */
    uint8_t expected_response_pdu[] = { 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__read_holding_registers__with_zero_copy(void)
{
    test_modbus_server_listen__read_holding_registers(true);
}

static void test_modbus_server_listen__read_holding_registers__without_zero_copy(void)
{
    test_modbus_server_listen__read_holding_registers(false);
}

static void test_modbus_server_listen__read_input_registers(bool zero_copy)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = &zero_copy;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x04, 0x00, 0x0A, 0x00, 0x01 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    if (!zero_copy) {
        TEST_ASSERT(message.data == server_buffer);
    }

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_INPUT_REGISTERS, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x000A, message.addr);
    TEST_ASSERT_EQUAL_INT(1, message.count);
    TEST_ASSERT_EQUAL_INT(2, message.data_size);
    TEST_ASSERT_EQUAL_INT(0x10, message.data[0]);
    TEST_ASSERT_EQUAL_INT(0x2F, message.data[1]);

    /* assert server response PDU */
    uint8_t expected_response_pdu[] = { 0x04, 0x02, 0x10, 0x2F };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__read_input_registers__with_zero_copy(void)
{
    test_modbus_server_listen__read_input_registers(true);
}

static void test_modbus_server_listen__read_input_registers__without_zero_copy(void)
{
    test_modbus_server_listen__read_input_registers(false);
}

static void test_modbus_server_listen__write_single_coil(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x05, 0x00, 0xBF, 0x00, 0x00 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_COIL, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x00BF, message.addr);
    TEST_ASSERT_EQUAL_INT(1, message.count);
    TEST_ASSERT_EQUAL_INT(0, message.data_size);
    TEST_ASSERT_NULL(message.data);

    /* assert server response PDU (echo of request) */
    uint8_t expected_response_pdu[] = { 0x05, 0x00, 0xBF, 0x00, 0x00 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__write_single_holding_register(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x0004, message.addr);
    TEST_ASSERT_EQUAL_INT(1, message.count);
    TEST_ASSERT_EQUAL_INT(0, message.data_size);
    TEST_ASSERT_NULL(message.data);

    /* assert server response PDU (echo of request) */
    uint8_t expected_response_pdu[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__write_multiple_coils(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09, 0x02, 0x4D, 0x01 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_COILS, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x001B, message.addr);
    TEST_ASSERT_EQUAL_INT(9, message.count);
    TEST_ASSERT_EQUAL_INT(0, message.data_size);
    TEST_ASSERT_NULL(message.data);

    /* assert server response PDU (function code echo, address, count) */
    uint8_t expected_response_pdu[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__write_multiple_holding_registers(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x10, 0x00, 0x12, 0x00, 0x02, 0x04, 0x0B, 0x0A, 0xC1, 0x02 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server response message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, message.exc);
    TEST_ASSERT_EQUAL_INT(0x0012, message.addr);
    TEST_ASSERT_EQUAL_INT(2, message.count);
    TEST_ASSERT_EQUAL_INT(0, message.data_size);
    TEST_ASSERT_NULL(message.data);

    /* assert server response PDU (function code echo, address, count) */
    uint8_t expected_response_pdu[] = { 0x10, 0x00, 0x12, 0x00, 0x02 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__illegal_function_code(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback that handles exceptions */
    modbus_server.id = 1;
    modbus_server.cb = server_callback_illegal_function;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU with unknown function code 0x2B */
    uint8_t client_request_pdu[] = { 0x2B, 0x00, 0x00, 0x00, 0x01 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server received message with unknown function code */
    TEST_ASSERT_EQUAL_INT(0x2B, message.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_ILLEGAL_FUNCTION, message.exc);

    /* assert server response PDU */
    /* exception response: 0xAB = 0x2B | 0x80, exception code 0x01 */
    uint8_t expected_response_pdu[] = { 0xAB, 0x01 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__callback_sets_exception(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback that returns exception */
    modbus_server.id = 1;
    modbus_server.cb = server_callback_exception;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU (read coils) */
    uint8_t client_request_pdu[] = { 0x01, 0x00, 0x50, 0x00, 0x08 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server received message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, message.func);
    TEST_ASSERT_EQUAL_INT(0x0050, message.addr);
    TEST_ASSERT_EQUAL_INT(8, message.count);

    /* assert exception was set by callback and sent in response */
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_ILLEGAL_ADDRESS, message.exc);

    /* assert server response PDU */
    /* exception response: 0x81 = 0x01 | 0x80, exception code 0x02 */
    uint8_t expected_response_pdu[] = { 0x81, 0x02 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0,
                          memcmp(dev.send_buf, expected_response_pdu,
                                 sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__request_with_exception(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback */
    modbus_server.id = 1;
    modbus_server.cb = server_callback;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client request PDU */
    uint8_t client_request_pdu[] = { 0x81, 0x00, 0x1D, 0x00, 0x1F }; /* exception bit set */
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_server_listen(&server, &message));
}

static void test_modbus_server_listen__broadcast(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server;

    dev.recv_broadcast = true;
    modbus_init(&server, &_mock_driver, &dev, NULL);

    /* register server with callback that doesn't reply to broadcast */
    modbus_server.id = 1;
    modbus_server.flags = MODBUS_SERVER_FLAG_RECEIVE_BROADCAST;
    modbus_server.cb = server_callback_no_reply;
    modbus_server.arg = NULL;
    modbus_server_add(&server, &modbus_server);

    /* client broadcast request PDU (write single coil to address 0) */
    uint8_t client_request_pdu[] = { 0x05, 0x00, 0x50, 0xFF, 0x00 };
    memcpy(dev.recv_buf, client_request_pdu, sizeof(client_request_pdu));
    dev.recv_len = sizeof(client_request_pdu);

    /* create message for server to receive */
    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* assert server received message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_COIL, message.func);
    TEST_ASSERT_EQUAL_INT(0x0050, message.addr);
    TEST_ASSERT_EQUAL_INT(1, message.count);

    /* for broadcast, no response should be sent */
    TEST_ASSERT_EQUAL_INT(0, dev.send_len);
}

static Test *tests_modbus(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_modbus_init),

        new_TestFixture(test_modbus_server_registry),
        new_TestFixture(test_modbus_server_registry__duplicate_id),
        new_TestFixture(test_modbus_server_registry__remove_nonexistent),

        new_TestFixture(test_modbus_bit_count_to_size),
        new_TestFixture(test_modbus_reg_count_to_size),

        new_TestFixture(test_modbus_copy_bits),
        new_TestFixture(test_modbus_copy_regs),

        new_TestFixture(test_modbus_check_message__invalid_id),
        new_TestFixture(test_modbus_check_message__invalid_function),
        new_TestFixture(test_modbus_check_message__exception),
        new_TestFixture(test_modbus_check_message__exception_mask),
        new_TestFixture(test_modbus_check_message__broadcast_exception),
        new_TestFixture(test_modbus_check_message__read_coils),
        new_TestFixture(test_modbus_check_message__read_discrete_inputs),
        new_TestFixture(test_modbus_check_message__read_input_registers),
        new_TestFixture(test_modbus_check_message__read_holding_registers),
        new_TestFixture(test_modbus_check_message__write_multiple_coils),
        new_TestFixture(test_modbus_check_message__write_multiple_holding_registers),
        new_TestFixture(test_modbus_check_message__write_single_coil),
        new_TestFixture(test_modbus_check_message__write_single_holding_register),

        new_TestFixture(test_modbus_client_request__read_coils__with_zero_copy),
        new_TestFixture(test_modbus_client_request__read_coils__without_zero_copy),
        new_TestFixture(test_modbus_client_request__read_discrete_inputs__with_zero_copy),
        new_TestFixture(test_modbus_client_request__read_discrete_inputs__without_zero_copy),
        new_TestFixture(test_modbus_client_request__read_holding_registers__with_zero_copy),
        new_TestFixture(test_modbus_client_request__read_holding_registers__without_zero_copy),
        new_TestFixture(test_modbus_client_request__read_input_registers__with_zero_copy),
        new_TestFixture(test_modbus_client_request__read_input_registers__without_zero_copy),
        new_TestFixture(test_modbus_client_request__write_single_coil),
        new_TestFixture(test_modbus_client_request__write_single_holding_register),
        new_TestFixture(test_modbus_client_request__write_multiple_coils),
        new_TestFixture(test_modbus_client_request__write_multiple_holding_registers),
        new_TestFixture(test_modbus_client_request__illegal_function_code),
        new_TestFixture(test_modbus_client_request__exception),
        new_TestFixture(test_modbus_client_request__invalid_exception),
        new_TestFixture(test_modbus_client_request__wrong_id),
        new_TestFixture(test_modbus_client_request__broadcast),
        new_TestFixture(test_modbus_client_request__timeout),

        new_TestFixture(test_modbus_server_listen__read_coils__with_zero_copy),
        new_TestFixture(test_modbus_server_listen__read_coils__without_zero_copy),
        new_TestFixture(test_modbus_server_listen__read_discrete_inputs__with_zero_copy),
        new_TestFixture(test_modbus_server_listen__read_discrete_inputs__without_zero_copy),
        new_TestFixture(test_modbus_server_listen__read_holding_registers__with_zero_copy),
        new_TestFixture(test_modbus_server_listen__read_holding_registers__without_zero_copy),
        new_TestFixture(test_modbus_server_listen__read_input_registers__with_zero_copy),
        new_TestFixture(test_modbus_server_listen__read_input_registers__without_zero_copy),
        new_TestFixture(test_modbus_server_listen__write_single_coil),
        new_TestFixture(test_modbus_server_listen__write_single_holding_register),
        new_TestFixture(test_modbus_server_listen__write_multiple_coils),
        new_TestFixture(test_modbus_server_listen__write_multiple_holding_registers),
        new_TestFixture(test_modbus_server_listen__illegal_function_code),
        new_TestFixture(test_modbus_server_listen__callback_sets_exception),
        new_TestFixture(test_modbus_server_listen__request_with_exception),
        new_TestFixture(test_modbus_server_listen__broadcast),
    };

    EMB_UNIT_TESTCALLER(modbus_tests, NULL, NULL, fixtures);

    return (Test *)&modbus_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_modbus());
    TESTS_END();

    return 0;
}
