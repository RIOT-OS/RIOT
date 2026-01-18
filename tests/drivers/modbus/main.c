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
 * The tests in this file cover all four interfaces of the Modbus driver.
 *
 * The majority of the tests focus on encoding and decoding of messages into
 * protocol data units (PDUs), since this is where most of the protocol logic
 * resides.
 *
 * The client and server interfaces are also tested, but with a mock transport
 * that focuses on transport interaction, dispatching and error propagation.
 *
 * @author  Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "embUnit.h"

#include "modbus.h"

/**
 * @brief Mock Modbus transport structure.
 *
 * It provides separate buffers for asserting requests and responses.
 */
typedef struct {
    modbus_t *bus;

    int init_called;

    uint8_t pdu_buf[MODBUS_PDU_MAX_SIZE];

    uint8_t send_buf[32];
    uint8_t send_len;
    int send_result;

    uint8_t recv_buf[32];
    uint8_t recv_len;
    int recv_result;
    uint8_t recv_id;        /* override id when recv_id_set is true */
    bool recv_id_set;
    bool recv_skip_set_id;  /* when true, the mock does not touch message->id */
} modbus_mock_t;

static int _mock_init(modbus_t *bus)
{
    modbus_mock_t *dev = (modbus_mock_t *)bus->dev;

    dev->bus = bus;
    dev->init_called++;

    bus->buffer = dev->pdu_buf;
    bus->buffer_capacity = sizeof(dev->pdu_buf);
    bus->buffer_size = 0;

    return 0;
}

static int _mock_send(modbus_t *bus, const modbus_message_t *message)
{
    (void)message;

    modbus_mock_t *dev = (modbus_mock_t *)bus->dev;

    if (dev->send_result != 0) {
        return dev->send_result;
    }

    /* the send buffer is much smaller and should be sufficient for testing
     * purposes */
    assert(bus->buffer_size <= sizeof(dev->send_buf));

    memcpy(dev->send_buf, bus->buffer, bus->buffer_size);
    dev->send_len = bus->buffer_size;

    return 0;
}

static int _mock_recv(modbus_t *bus, modbus_message_t *message, bool request)
{
    (void)request;

    modbus_mock_t *dev = (modbus_mock_t *)bus->dev;

    if (dev->recv_result != 0) {
        return dev->recv_result;
    }

    /* simulate a time-out by setting recv_len to 0 */
    if (dev->recv_len == 0) {
        return -ETIMEDOUT;
    }

    /* set the identifier of the received message (default 1, unless
     * overridden) */
    if (!dev->recv_skip_set_id) {
        message->id = dev->recv_id_set ? dev->recv_id : 1;
    }

    assert(dev->recv_len <= bus->buffer_capacity);

    memcpy(bus->buffer, dev->recv_buf, dev->recv_len);
    bus->buffer_size = dev->recv_len;

    return 0;
}

/**
 * @brief Mock Modbus transport descriptor.
 */
static const modbus_transport_desc_t _mock_transport = {
    .init = _mock_init,
    .send = _mock_send,
    .recv = _mock_recv,
};

/**
 * @brief Captures information from the most recent invocation of an
 *        integration test callback.
 */
typedef struct {
    int call_count;
    modbus_message_t last_message;
    bool data_in_bus_buffer; /* whether message->data points into the bus buffer */
} cb_state_t;

/**
 * @brief Server callback that replies with a static read-coils payload
 *        (zero-copy).
 *
 * The payload is written directly into @p message->data, which on entry
 * points into the bus buffer (zero-copy).
 */
static int cb_reply_read_coils(modbus_t *modbus, modbus_server_t *server,
                               modbus_message_t *message)
{
    cb_state_t *state = server->arg;

    state->call_count++;
    state->last_message = *message;
    state->data_in_bus_buffer = (message->data == modbus->buffer + 2);

    /* fill in the response payload directly into the zero-copy */
    message->data[0] = 0xCD;
    message->data[1] = 0x6B;
    message->data[2] = 0xB2;
    message->data[3] = 0x7F;

    return modbus_server_reply(modbus, message);
}

static uint8_t cb_external_buffer[16];

/**
 * @brief Server callback that replies with a static read-coils payload using a
 *        caller-provided buffer.
 *
 * Overrides @p message->data and @p message->data_size to point to a buffer
 * outside of the bus buffer.
 */
static int cb_reply_read_coils_external(modbus_t *modbus, modbus_server_t *server,
                                        modbus_message_t *message)
{
    cb_state_t *state = server->arg;

    state->call_count++;

    message->data = cb_external_buffer;
    message->data_size = sizeof(cb_external_buffer);

    message->data[0] = 0xCD;
    message->data[1] = 0x6B;
    message->data[2] = 0xB2;
    message->data[3] = 0x7F;

    return modbus_server_reply(modbus, message);
}

/**
 * @brief Server callback that always replies with an exception.
 */
static int cb_reply_exception(modbus_t *modbus, modbus_server_t *server,
                              modbus_message_t *message)
{
    cb_state_t *state = server->arg;

    state->call_count++;
    message->exc = MODBUS_EXC_ILLEGAL_ADDRESS;

    return modbus_server_reply(modbus, message);
}

/**
 * @brief Server callback that counts invocations but does not reply (used for
 *        broadcast tests).
 */
static int cb_count_no_reply(modbus_t *modbus, modbus_server_t *server,
                             modbus_message_t *message)
{
    (void)modbus;
    (void)message;

    cb_state_t *state = server->arg;

    state->call_count++;
    return 0;
}

/**
 * @brief Server callback that always returns an error.
 */
static int cb_returns_error(modbus_t *modbus, modbus_server_t *server,
                            modbus_message_t *message)
{
    (void)modbus;
    (void)server;
    (void)message;

    return -1;
}

static void test_modbus_init(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    int ret = modbus_init(&bus, &_mock_transport, &dev, NULL);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT(bus.transport == &_mock_transport);
    TEST_ASSERT_EQUAL_INT(1, dev.init_called);
}

static void test_modbus_server_registry(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    modbus_init(&bus, &_mock_transport, &dev, NULL);

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

    modbus_init(&bus, &_mock_transport, &dev, NULL);

    modbus_server_t a = { .id = 10 };
    modbus_server_t b = { .id = 10 };

    /* add two servers with the same ID which is not allowed */
    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &a));
    TEST_ASSERT_EQUAL_INT(-EEXIST, modbus_server_add(&bus, &b));
}

static void test_modbus_server_registry__invalid_id(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    modbus_init(&bus, &_mock_transport, &dev, NULL);

    /* the broadcast id is not a valid server id */
    modbus_server_t a = { .id = MODBUS_ID_BROADCAST };
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_server_add(&bus, &a));

    /* out of range */
    a.id = MODBUS_ID_MAX + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_server_add(&bus, &a));
}

static void test_modbus_server_registry__remove_nonexistent(void)
{
    modbus_t bus;
    modbus_mock_t dev = { 0 };

    modbus_init(&bus, &_mock_transport, &dev, NULL);

    modbus_server_t a = { .id = 1 };
    modbus_server_t b = { .id = 2 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_add(&bus, &a));

    /* removing a server that was never added returns -ENOENT */
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

static void test_modbus_copy_bits__zero_count(void)
{
    uint8_t src[2] = { 0xAA, 0xCC };
    uint8_t dst[2] = { 0xFF, 0xFF };

    /* zero count must be a no-op: dst must not be modified */
    modbus_copy_bits(dst, 0, src, 0, 0, true);
    TEST_ASSERT_EQUAL_INT(0xFF, dst[0]);
    TEST_ASSERT_EQUAL_INT(0xFF, dst[1]);
}

static void test_modbus_copy_bits__no_pad_zeroes(void)
{
    uint8_t src[1] = { 0x0F }; /* bits 0-3 set, bits 4-7 clear */
    uint8_t dst[1] = { 0xFF };

    /* copy bit 4 of src (which is 0) into bit 0 of dst, without zeroing the
     * surrounding bits in the destination byte */
    modbus_copy_bit(dst, 0, src, 4, false);

    /* bit 0 is cleared, bits 1-7 remain 1 */
    TEST_ASSERT_EQUAL_INT(0xFE, dst[0]);

    /* copy bit 0 of src (which is 1) into bit 1 of dst, without padding */
    dst[0] = 0x00;
    modbus_copy_bit(dst, 1, src, 0, false);
    TEST_ASSERT_EQUAL_INT(0x02, dst[0]);
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

static void test_modbus_copy_regs__zero_count(void)
{
    uint16_t src16[2] = { 0x1234, 0xABCD };
    uint16_t dst16[2] = { 0xDEAD, 0xBEEF };

    /* zero count must be a no-op: dst must not be modified */
    modbus_copy_regs(dst16, src16, 0);
    TEST_ASSERT_EQUAL_INT(0xDEAD, dst16[0]);
    TEST_ASSERT_EQUAL_INT(0xBEEF, dst16[1]);
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

    /* address range wraps around */
    msg.data_size = sizeof(buf8);
    msg.count = 10;
    msg.addr = 0xFFFF;
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

    /* address range wraps around */
    msg.data_size = sizeof(buf8);
    msg.count = 10;
    msg.addr = 0xFFFF;
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

    /* address range wraps around */
    msg.data = buf8;
    msg.data_size = sizeof(buf8);
    msg.count = 2;
    msg.addr = 0xFFFF;
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

    /* address range wraps around */
    msg.data = buf8;
    msg.data_size = sizeof(buf8);
    msg.count = 2;
    msg.addr = 0xFFFF;
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

    /* null data */
    msg.data_size = sizeof(buf8);
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* address range wraps around */
    msg.data = buf8;
    msg.count = 10;
    msg.addr = 0xFFFF;
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

    msg.count = 124;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    msg.count = 2;
    msg.data = NULL;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* too small data */
    msg.data = buf2; /* needs 4 */
    msg.data_size = sizeof(buf2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_check_message(&msg));

    /* address range wraps around */
    msg.data = buf8;
    msg.data_size = sizeof(buf8);
    msg.count = 2;
    msg.addr = 0xFFFF;
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

static void test_modbus_encode_request__read_coils(void)
{
    uint8_t buf[5] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__read_discrete_inputs(void)
{
    uint8_t buf[5] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28,
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x02, 0x00, 0x7A, 0x00, 0x1C };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__read_holding_registers(void)
{
    uint8_t buf[5] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3,
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x03, 0x00, 0x6F, 0x00, 0x03 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__read_input_registers(void)
{
    uint8_t buf[5] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1,
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x04, 0x00, 0x0A, 0x00, 0x01 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__write_single_coil(void)
{
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xFF, 0x00 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x05, 0x00, 0xBF, 0xFF, 0x00 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__write_single_holding_register(void)
{
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xAB, 0xCD };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER,
        .addr = 0x0004,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__write_multiple_coils(void)
{
    uint8_t buf[8] = { 0 };
    uint8_t data[] = { 0x4D, 0x01 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_COILS,
        .addr = 0x001B,
        .count = 9,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09, 0x02, 0x4D, 0x01 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__write_multiple_holding_registers(void)
{
    uint8_t buf[10] = { 0 };
    uint8_t data[] = { 0x0B, 0x0A, 0xC1, 0x02 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS,
        .addr = 0x0012,
        .count = 2,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_request(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x10, 0x00, 0x12, 0x00, 0x02, 0x04, 0x0B, 0x0A, 0xC1, 0x02 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_request__unsupported_function(void)
{
    uint8_t buf[8] = { 0 };
    modbus_message_t msg = {
        .func = 0x2B, /* unknown function code */
        .addr = 0x0000,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_encode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_request__buffer_too_small(void)
{
    uint8_t buf[4]; /* read request needs 5 bytes */
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_request(buf, sizeof(buf), &msg));

    /* write multiple coils needs 6 + size */
    uint8_t buf2[6];
    uint8_t data[] = { 0x4D, 0x01 };
    msg.func = MODBUS_FC_WRITE_MULTIPLE_COILS;
    msg.addr = 0x001B;
    msg.count = 9; /* size = 2 → needs 8 bytes */
    msg.data = data;
    msg.data_size = sizeof(data);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_request(buf2, sizeof(buf2), &msg));
}

static void test_modbus_encode_request__buffer_too_small__write_single(void)
{
    uint8_t buf[4]; /* write single requests need 5 bytes */
    uint8_t data[] = { 0xFF, 0x00 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_request(buf, sizeof(buf), &msg));

    uint8_t data2[] = { 0xAB, 0xCD };
    msg.func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER;
    msg.addr = 0x0004;
    msg.data = data2;
    msg.data_size = sizeof(data2);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_request__write_single_coil__invalid_data(void)
{
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xFF, 0x00 };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x0050,
        .count = 1,
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));

    /* data size too small */
    msg.data = data;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__read_coils(void)
{
    uint8_t buf[6] = { 0 };
    uint8_t data[] = { 0xCD, 0x6B, 0xB2, 0x7F };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__read_coils__zero_copy(void)
{
    /* the message data points into the response buffer (zero-copy):
     * encode_response must not try to copy from data into itself */
    uint8_t buf[6] = { 0 };

    buf[2] = 0xCD;
    buf[3] = 0x6B;
    buf[4] = 0xB2;
    buf[5] = 0x7F;

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
        .data = buf + 2,
        .data_size = 4,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__read_discrete_inputs(void)
{
    uint8_t buf[6] = { 0 };
    uint8_t data[] = { 0xAC, 0xDB, 0xFB, 0x0D };

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x02, 0x04, 0xAC, 0xDB, 0xFB, 0x0D };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__read_discrete_inputs__zero_copy(void)
{
    uint8_t buf[6] = { 0 };

    buf[2] = 0xAC;
    buf[3] = 0xDB;
    buf[4] = 0xFB;
    buf[5] = 0x0D;

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28,
        .data = buf + 2,
        .data_size = 4,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x02, 0x04, 0xAC, 0xDB, 0xFB, 0x0D };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__read_holding_registers(void)
{
    uint8_t buf[8] = { 0 };
    uint8_t data[] = { 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__read_holding_registers__zero_copy(void)
{
    uint8_t buf[8] = { 0 };

    buf[2] = 0xAE;
    buf[3] = 0x41;
    buf[4] = 0x56;
    buf[5] = 0x52;
    buf[6] = 0x43;
    buf[7] = 0x40;

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3,
        .data = buf + 2,
        .data_size = 6,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__read_input_registers(void)
{
    uint8_t buf[4] = { 0 };
    uint8_t data[] = { 0x10, 0x2F };

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x04, 0x02, 0x10, 0x2F };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__read_input_registers__zero_copy(void)
{
    uint8_t buf[4] = { 0 };

    buf[2] = 0x10;
    buf[3] = 0x2F;

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1,
        .data = buf + 2,
        .data_size = 2,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x04, 0x02, 0x10, 0x2F };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__write_single_coil(void)
{
    /* echo of request: func(1) + addr(2) + data(2), encoded explicitly */
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xFF, 0x00 };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x05, 0x00, 0xBF, 0xFF, 0x00 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__write_single_coil__zero_copy(void)
{
    /* server path: the request decode placed the value at buf + 3; encoding
     * the response in the same buffer must not corrupt it */
    uint8_t buf[5] = { 0xAA, 0xAA, 0xAA, 0xFF, 0x00 };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
        .data = buf + 3,
        .data_size = 2,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x05, 0x00, 0xBF, 0xFF, 0x00 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__write_single_holding_register(void)
{
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xAB, 0xCD };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER,
        .addr = 0x0004,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__write_multiple_coils(void)
{
    /* abbreviated echo: func(1) + addr(2) + count(2), encoded explicitly */
    uint8_t buf[5] = { 0 };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_COILS,
        .addr = 0x001B,
        .count = 9,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__write_multiple_holding_registers(void)
{
    uint8_t buf[5] = { 0 };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS,
        .addr = 0x0012,
        .count = 2,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    uint8_t expected[] = { 0x10, 0x00, 0x12, 0x00, 0x02 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__exception(void)
{
    uint8_t buf[2] = { 0 };

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .exc = MODBUS_EXC_ILLEGAL_ADDRESS,
        .addr = 0x0100,
        .count = 10,
    };

    ssize_t n = modbus_encode_response(buf, sizeof(buf), &msg);

    /* exception bit set on function code, exception code follows */
    uint8_t expected[] = { 0x81, 0x02 };

    TEST_ASSERT_EQUAL_INT(sizeof(expected), n);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

static void test_modbus_encode_response__unsupported_function(void)
{
    uint8_t buf[8] = { 0 };

    modbus_message_t msg = {
        .func = 0x2B, /* unknown function code */
        .addr = 0,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__buffer_too_small(void)
{
    /* read coils response needs 2 + size bytes */
    uint8_t buf[5];
    uint8_t data[] = { 0xCD, 0x6B, 0xB2, 0x7F };

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31, /* needs 6 bytes */
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_response(buf, sizeof(buf), &msg));

    /* exception response needs at least 2 bytes */
    uint8_t buf2[1];
    msg.exc = MODBUS_EXC_ILLEGAL_ADDRESS;

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_response(buf2, sizeof(buf2), &msg));
}

static void test_modbus_encode_response__buffer_too_small__write(void)
{
    /* all write responses need 5 bytes: func(1) + addr(2) + data_or_count(2) */
    uint8_t buf[4];

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_response(buf, sizeof(buf), &msg));

    msg.func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER;
    msg.addr = 0x0004;
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_response(buf, sizeof(buf), &msg));

    msg.func = MODBUS_FC_WRITE_MULTIPLE_COILS;
    msg.addr = 0x001B;
    msg.count = 9;
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_response(buf, sizeof(buf), &msg));

    msg.func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS;
    msg.addr = 0x0012;
    msg.count = 2;
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_request__read_coils(void)
{
    uint8_t buf[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };

    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(0x001D, msg.addr);
    TEST_ASSERT_EQUAL_INT(31, msg.count);

    /* zero-copy inside the PDU buffer for the callback to fill in */
    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size); /* 31 bits equals 4 bytes */
}

static void test_modbus_decode_request__read_discrete_inputs(void)
{
    uint8_t buf[] = { 0x02, 0x00, 0x7A, 0x00, 0x1C };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_DISCRETE_INPUTS, msg.func);
    TEST_ASSERT_EQUAL_INT(0x007A, msg.addr);
    TEST_ASSERT_EQUAL_INT(28, msg.count);

    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
}

static void test_modbus_decode_request__read_holding_registers(void)
{
    uint8_t buf[] = { 0x03, 0x00, 0x6F, 0x00, 0x03 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_HOLDING_REGISTERS, msg.func);
    TEST_ASSERT_EQUAL_INT(0x006F, msg.addr);
    TEST_ASSERT_EQUAL_INT(3, msg.count);

    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(6, msg.data_size); /* 3 registers equals 6 bytes */
}

static void test_modbus_decode_request__read_input_registers(void)
{
    uint8_t buf[] = { 0x04, 0x00, 0x0A, 0x00, 0x01 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_INPUT_REGISTERS, msg.func);
    TEST_ASSERT_EQUAL_INT(0x000A, msg.addr);
    TEST_ASSERT_EQUAL_INT(1, msg.count);

    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
}

static void test_modbus_decode_request__write_single_coil(void)
{
    uint8_t buf[] = { 0x05, 0x00, 0xBF, 0xFF, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_COIL, msg.func);
    TEST_ASSERT_EQUAL_INT(0x00BF, msg.addr);
    TEST_ASSERT_EQUAL_INT(1, msg.count);

    /* data points into the buffer for the callback to consume */
    TEST_ASSERT(msg.data == buf + 3);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xFF, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0x00, msg.data[1]);
}

static void test_modbus_decode_request__write_single_holding_register(void)
{
    uint8_t buf[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER, msg.func);
    TEST_ASSERT_EQUAL_INT(0x0004, msg.addr);
    TEST_ASSERT_EQUAL_INT(1, msg.count);

    TEST_ASSERT(msg.data == buf + 3);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xAB, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0xCD, msg.data[1]);
}

static void test_modbus_decode_request__write_multiple_coils(void)
{
    uint8_t buf[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09, 0x02, 0x4D, 0x01 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(0x001B, msg.addr);
    TEST_ASSERT_EQUAL_INT(9, msg.count);

    TEST_ASSERT(msg.data == buf + 6);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0x4D, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0x01, msg.data[1]);
}

static void test_modbus_decode_request__write_multiple_holding_registers(void)
{
    uint8_t buf[] = { 0x10, 0x00, 0x12, 0x00, 0x02, 0x04, 0x0B, 0x0A, 0xC1, 0x02 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS, msg.func);
    TEST_ASSERT_EQUAL_INT(0x0012, msg.addr);
    TEST_ASSERT_EQUAL_INT(2, msg.count);

    TEST_ASSERT(msg.data == buf + 6);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0x0B, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0x0A, msg.data[1]);
    TEST_ASSERT_EQUAL_INT(0xC1, msg.data[2]);
    TEST_ASSERT_EQUAL_INT(0x02, msg.data[3]);
}

static void test_modbus_decode_request__unsupported_function(void)
{
    /* unknown function code is reported via message->exc */
    uint8_t buf[] = { 0x2B, 0x00, 0x00, 0x00, 0x01 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_request(buf, sizeof(buf), &msg));
    TEST_ASSERT_EQUAL_INT(0x2B, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_ILLEGAL_FUNCTION, msg.exc);
}

static void test_modbus_decode_request__exception_bit_set(void)
{
    /* the exception bit must not be set on a request */
    uint8_t buf[] = { 0x81, 0x00, 0x1D, 0x00, 0x1F };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_decode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_request__truncated(void)
{
    modbus_message_t msg = { 0 };

    /* empty PDU */
    uint8_t buf1[] = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_request(buf1, 0, &msg));

    /* truncated read coils request */
    uint8_t buf2[] = { 0x01, 0x00, 0x1D };
    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_request(buf2, sizeof(buf2), &msg));

    /* truncated write multiple coils request: header complete but data
     * incomplete (claims 4 bytes of data but only 2 follow) */
    uint8_t buf3[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09, 0x04, 0x4D, 0x01 };
    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_request(buf3, sizeof(buf3), &msg));

    /* truncated write multiple coils request: header itself incomplete */
    uint8_t buf4[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09 };
    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_request(buf4, sizeof(buf4), &msg));
}

static void test_modbus_decode_request__write_multiple_coils__size_mismatch(void)
{
    /* count=9 requires ceil(9 / 8) = 2 bytes, but the byte_count field indicates 3 */
    uint8_t buf[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09, 0x03, 0x4D, 0x01, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_request__write_multiple_holding_registers__size_mismatch(void)
{
    /* count=2 requires 2 * 2 = 4 bytes, but the byte_count field indicates 2 */
    uint8_t buf[] = { 0x10, 0x00, 0x12, 0x00, 0x02, 0x02, 0x0B, 0x0A };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_request__read_coils__invalid_count(void)
{
    /* count 0 */
    uint8_t buf_zero[] = { 0x01, 0x00, 0x1D, 0x00, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_zero, sizeof(buf_zero), &msg));

    /* count 2001 (maximum is 2000) */
    uint8_t buf_over[] = { 0x01, 0x00, 0x1D, 0x07, 0xD1 };
    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_over, sizeof(buf_over), &msg));
}

static void test_modbus_decode_request__read_holding_registers__invalid_count(void)
{
    /* count 0 */
    uint8_t buf_zero[] = { 0x03, 0x00, 0x1D, 0x00, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_zero, sizeof(buf_zero), &msg));

    /* count 126 (maximum is 125) */
    uint8_t buf_over[] = { 0x03, 0x00, 0x1D, 0x00, 0x7E };
    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_over, sizeof(buf_over), &msg));
}

static void test_modbus_decode_request__write_multiple_coils__invalid_count(void)
{
    /* count 0 (declared byte count matches, but the quantity is invalid) */
    uint8_t buf_zero[] = { 0x0F, 0x00, 0x1B, 0x00, 0x00, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_zero, sizeof(buf_zero), &msg));

    /* count 1969 (maximum is 1968) */
    uint8_t buf_over[] = { 0x0F, 0x00, 0x1B, 0x07, 0xB1, 0x02, 0x4D, 0x01 };
    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_over, sizeof(buf_over), &msg));
}

static void test_modbus_decode_request__write_multiple_regs__invalid_count(void)
{
    /* count 0 (declared byte count matches, but the quantity is invalid) */
    uint8_t buf_zero[] = { 0x10, 0x00, 0x12, 0x00, 0x00, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_zero, sizeof(buf_zero), &msg));

    /* count 124 (maximum is 123) */
    uint8_t buf_over[] = { 0x10, 0x00, 0x12, 0x00, 0x7C, 0x04, 0x0B, 0x0A, 0xC1, 0x02 };
    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_over, sizeof(buf_over), &msg));
}

static void test_modbus_decode_response__read_coils__zero_copy(void)
{
    /* when message->data is NULL on entry, decode points it into buf */
    uint8_t buf[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
        .data = NULL,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xCD, msg.data[0]);
    TEST_ASSERT_EQUAL_INT(0x6B, msg.data[1]);
    TEST_ASSERT_EQUAL_INT(0xB2, msg.data[2]);
    TEST_ASSERT_EQUAL_INT(0x7F, msg.data[3]);
}

static void test_modbus_decode_response__read_coils__copy(void)
{
    /* when message->data is non-NULL, decode copies into the caller's
     * buffer and leaves the data pointer unchanged */
    uint8_t buf[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    uint8_t data[4] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT(msg.data == data);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xCD, data[0]);
    TEST_ASSERT_EQUAL_INT(0x6B, data[1]);
    TEST_ASSERT_EQUAL_INT(0xB2, data[2]);
    TEST_ASSERT_EQUAL_INT(0x7F, data[3]);
}

static void test_modbus_decode_response__read_coils__copy_too_small(void)
{
    /* a caller-provided buffer that is too small must be reported */
    uint8_t buf[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    uint8_t data[2];

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_response__read_discrete_inputs__zero_copy(void)
{
    uint8_t buf[] = { 0x02, 0x04, 0xAC, 0xDB, 0xFB, 0x0D };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28,
        .data = NULL,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_DISCRETE_INPUTS, msg.func);
    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
}

static void test_modbus_decode_response__read_discrete_inputs__copy(void)
{
    uint8_t buf[] = { 0x02, 0x04, 0xAC, 0xDB, 0xFB, 0x0D };
    uint8_t data[4] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT(msg.data == data);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xAC, data[0]);
    TEST_ASSERT_EQUAL_INT(0x0D, data[3]);
}

static void test_modbus_decode_response__read_holding_registers__zero_copy(void)
{
    uint8_t buf[] = { 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3,
        .data = NULL,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_HOLDING_REGISTERS, msg.func);
    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(6, msg.data_size);
}

static void test_modbus_decode_response__read_holding_registers__copy(void)
{
    uint8_t buf[] = { 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };
    uint8_t data[6] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT(msg.data == data);
    TEST_ASSERT_EQUAL_INT(6, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xAE, data[0]);
    TEST_ASSERT_EQUAL_INT(0x40, data[5]);
}

static void test_modbus_decode_response__read_input_registers__zero_copy(void)
{
    uint8_t buf[] = { 0x04, 0x02, 0x10, 0x2F };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1,
        .data = NULL,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_INPUT_REGISTERS, msg.func);
    TEST_ASSERT(msg.data == buf + 2);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
}

static void test_modbus_decode_response__read_input_registers__copy(void)
{
    uint8_t buf[] = { 0x04, 0x02, 0x10, 0x2F };
    uint8_t data[2] = { 0 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT(msg.data == data);
    TEST_ASSERT_EQUAL_INT(2, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0x10, data[0]);
    TEST_ASSERT_EQUAL_INT(0x2F, data[1]);
}

static void test_modbus_decode_response__write_single_coil(void)
{
    uint8_t buf[] = { 0x05, 0x00, 0xBF, 0xFF, 0x00 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_COIL, msg.func);
}

static void test_modbus_decode_response__write_single_holding_register(void)
{
    uint8_t buf[] = { 0x06, 0x00, 0x04, 0xAB, 0xCD };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER,
        .addr = 0x0004,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER, msg.func);
}

static void test_modbus_decode_response__write_multiple_coils(void)
{
    uint8_t buf[] = { 0x0F, 0x00, 0x1B, 0x00, 0x09 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_COILS,
        .addr = 0x001B,
        .count = 9,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_COILS, msg.func);
}

static void test_modbus_decode_response__write_multiple_holding_registers(void)
{
    uint8_t buf[] = { 0x10, 0x00, 0x12, 0x00, 0x02 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS,
        .addr = 0x0012,
        .count = 2,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS, msg.func);
}

static void test_modbus_decode_response__exception(void)
{
    /* exception response: 0x81 = 0x01 | 0x80, exception code 0x02 */
    uint8_t buf[] = { 0x81, 0x02 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x0100,
        .count = 10,
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_decode_response(buf, sizeof(buf), &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_ILLEGAL_ADDRESS, msg.exc);
}

static void test_modbus_decode_response__exception_with_none(void)
{
    /* exception bit set but exception code is MODBUS_EXC_NONE (invalid) */
    uint8_t buf[] = { 0x81, 0x00 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x0100,
        .count = 10,
    };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_response__addr_mismatch(void)
{
    /* echo does not match the request's address */
    uint8_t buf[] = { 0x05, 0x00, 0xC0, 0xFF, 0x00 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_response__count_mismatch(void)
{
    /* abbreviated echo does not match the request's count */
    uint8_t buf[] = { 0x0F, 0x00, 0x1B, 0x00, 0x08 };
    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_COILS,
        .addr = 0x001B,
        .count = 9,
    };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_response__size_mismatch(void)
{
    /* size byte (3) does not match expected size for count=31 (4) */
    uint8_t buf[] = { 0x01, 0x03, 0xCD, 0x6B, 0xB2 };
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_response__truncated(void)
{
    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    /* less than minimum response size */
    uint8_t buf1[] = { 0x01 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_response(buf1, sizeof(buf1), &msg));

    /* claimed size byte not satisfied by actual length */
    uint8_t buf2[] = { 0x01, 0x04, 0xCD, 0x6B };
    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_response(buf2, sizeof(buf2), &msg));

    /* truncated write_single_coil echo */
    msg.func = MODBUS_FC_WRITE_SINGLE_COIL;
    msg.addr = 0x00BF;
    msg.count = 1;
    uint8_t buf3[] = { 0x05, 0x00, 0xBF };
    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_decode_response(buf3, sizeof(buf3), &msg));
}

static void test_modbus_decode_response__unsupported_function(void)
{
    uint8_t buf[] = { 0x2B, 0x00, 0x00, 0x00, 0x01 };
    modbus_message_t msg = {
        .func = 0x2B,
        .addr = 0,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_client_request__round_trip(void)
{
    /* end-to-end happy path: encode request → send → recv → decode response */
    modbus_mock_t dev = { 0 };
    modbus_t client;
    uint8_t data[4];

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
        .data = data,
        .data_size = sizeof(data),
    };

    /* mock the response that the transport will return */
    uint8_t response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, response_pdu, sizeof(response_pdu));
    dev.recv_len = sizeof(response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* assert request PDU sent by client */
    uint8_t expected_request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    TEST_ASSERT_EQUAL_INT(sizeof(expected_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, expected_request_pdu,
                                    sizeof(expected_request_pdu)));

    /* assert response decoded into message */
    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_NONE, msg.exc);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
    TEST_ASSERT_EQUAL_INT(0xCD, data[0]);
    TEST_ASSERT_EQUAL_INT(0x7F, data[3]);
}

static void test_modbus_client_request__zero_copy(void)
{
    /* with message->data NULL, the response data must point into the
     * transport buffer (zero-copy) */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    uint8_t response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, response_pdu, sizeof(response_pdu));
    dev.recv_len = sizeof(response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    /* data points into the bus buffer at the data offset */
    TEST_ASSERT(msg.data == client.buffer + 2);
    TEST_ASSERT_EQUAL_INT(4, msg.data_size);
}

static void test_modbus_client_request__copy(void)
{
    /* with message->data set, the response data is copied into the caller's
     * buffer and the data pointer is preserved */
    modbus_mock_t dev = { 0 };
    modbus_t client;
    uint8_t data[4] = { 0 };

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
        .data = data,
        .data_size = sizeof(data),
    };

    uint8_t response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, response_pdu, sizeof(response_pdu));
    dev.recv_len = sizeof(response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    TEST_ASSERT(msg.data == data);
    TEST_ASSERT_EQUAL_INT(0xCD, data[0]);
}

static void test_modbus_client_request__exception_response(void)
{
    /* exception responses are propagated back through msg.exc, no error */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x0100,
        .count = 10,
    };

    uint8_t response_pdu[] = { 0x81, 0x02 };
    memcpy(dev.recv_buf, response_pdu, sizeof(response_pdu));
    dev.recv_len = sizeof(response_pdu);

    TEST_ASSERT_EQUAL_INT(0, modbus_client_request(&client, &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_FC_READ_COILS, msg.func);
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_ILLEGAL_ADDRESS, msg.exc);
}

static void test_modbus_client_request__unsupported_function(void)
{
    /* encode failure (unsupported function) is reported as -EBADMSG */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = 0x2B,
        .addr = 0,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_client_request(&client, &msg));

    /* nothing should have been sent */
    TEST_ASSERT_EQUAL_INT(0, dev.send_len);
}

static void test_modbus_client_request__wrong_id(void)
{
    /* response from a different identifier must be rejected */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    uint8_t response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, response_pdu, sizeof(response_pdu));
    dev.recv_len = sizeof(response_pdu);
    dev.recv_id = 2; /* server replied with a different id */
    dev.recv_id_set = true;

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_client_request(&client, &msg));
}

static void test_modbus_client_request__wrong_function(void)
{
    /* response with a different function code must be rejected */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    /* function code 0x02 (READ_DISCRETE_INPUTS) instead of 0x01 — decodes
     * successfully, but the integration check rejects it */
    uint8_t response_pdu[] = { 0x02, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, response_pdu, sizeof(response_pdu));
    dev.recv_len = sizeof(response_pdu);

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_client_request(&client, &msg));
}

static void test_modbus_client_request__missing_id(void)
{
    /* if the transport fails to fill in the message id, request fails */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    uint8_t response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    memcpy(dev.recv_buf, response_pdu, sizeof(response_pdu));
    dev.recv_len = sizeof(response_pdu);
    dev.recv_skip_set_id = true;

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_client_request(&client, &msg));
}

static void test_modbus_client_request__send_error(void)
{
    /* errors from the transport's send function are propagated */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    dev.send_result = -EBUSY;

    TEST_ASSERT_EQUAL_INT(-EBUSY, modbus_client_request(&client, &msg));
}

static void test_modbus_client_request__timeout(void)
{
    /* errors from the transport's recv function are propagated */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31,
    };

    /* with recv_len=0 the mock returns -ETIMEDOUT */
    dev.recv_len = 0;

    TEST_ASSERT_EQUAL_INT(-ETIMEDOUT, modbus_client_request(&client, &msg));
}

static void test_modbus_client_broadcast__sets_broadcast_id(void)
{
    /* broadcast must overwrite message->id and not wait for a response */
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    uint8_t data[] = { 0xFF, 0x00 };
    modbus_message_t msg = {
        .id = 5, /* will be overwritten with MODBUS_ID_BROADCAST */
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x0050,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(0, modbus_client_broadcast(&client, &msg));

    TEST_ASSERT_EQUAL_INT(MODBUS_ID_BROADCAST, msg.id);

    /* request must have been sent and no response read */
    uint8_t expected_request_pdu[] = { 0x05, 0x00, 0x50, 0xFF, 0x00 };
    TEST_ASSERT_EQUAL_INT(sizeof(expected_request_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, expected_request_pdu,
                                    sizeof(expected_request_pdu)));
}

static void test_modbus_client_broadcast__invalid_function(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    modbus_message_t msg = {
        .id = 1,
        .func = 0x2B,
        .addr = 0,
        .count = 1,
    };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, modbus_client_broadcast(&client, &msg));
    TEST_ASSERT_EQUAL_INT(0, dev.send_len);
}

static void test_modbus_client_broadcast__send_error(void)
{
    modbus_mock_t dev = { 0 };
    modbus_t client;

    modbus_init(&client, &_mock_transport, &dev, NULL);

    uint8_t data[] = { 0xFF, 0x00 };
    modbus_message_t msg = {
        .id = 1,
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x0050,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    dev.send_result = -EBUSY;

    TEST_ASSERT_EQUAL_INT(-EBUSY, modbus_client_broadcast(&client, &msg));
}

static void test_modbus_server_listen__round_trip(void)
{
    /* end-to-end happy path: recv → decode request → dispatch → callback fills
     * response → encode response → send */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_read_coils,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* callback must have been invoked exactly once */
    TEST_ASSERT_EQUAL_INT(1, state.call_count);

    /* assert response PDU was sent */
    uint8_t expected_response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, expected_response_pdu,
                                    sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__zero_copy(void)
{
    /* the request decode places message->data into the bus buffer, where
     * the callback can write its response in-place */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_read_coils,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    /* the callback observed message->data pointing into the bus buffer */
    TEST_ASSERT(state.data_in_bus_buffer);
}

static void test_modbus_server_listen__copy(void)
{
    /* the callback may override message->data with an external buffer; the
     * encoded response PDU must contain the bytes from that buffer */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_read_coils_external,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));
    TEST_ASSERT_EQUAL_INT(1, state.call_count);

    uint8_t expected_response_pdu[] = { 0x01, 0x04, 0xCD, 0x6B, 0xB2, 0x7F };
    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, expected_response_pdu,
                                    sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__no_server_for_id(void)
{
    /* if no server matches the request id, listen short-circuits with
     * -ENOENT (the message could be a response from another bus member) */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_read_coils,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);
    dev.recv_id = 2; /* mismatch with registered server id=1 */
    dev.recv_id_set = true;

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(-ENOENT, modbus_server_listen(&server, &message));
    TEST_ASSERT_EQUAL_INT(0, state.call_count);
    TEST_ASSERT_EQUAL_INT(0, dev.send_len);
}

static void test_modbus_server_listen__broadcast_dispatch(void)
{
    /* broadcast requests are dispatched to servers with the broadcast flag,
     * but the callback should not reply (so dev.send_len stays 0) */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .flags = MODBUS_SERVER_FLAG_RECEIVE_BROADCAST,
        .cb = cb_count_no_reply,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x05, 0x00, 0x50, 0xFF, 0x00 };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);
    dev.recv_id = MODBUS_ID_BROADCAST;
    dev.recv_id_set = true;

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    TEST_ASSERT_EQUAL_INT(1, state.call_count);
    TEST_ASSERT_EQUAL_INT(0, dev.send_len);
}

static void test_modbus_server_listen__broadcast_no_match(void)
{
    /* broadcast with no broadcast-capable server: -ENOENT, no dispatch */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_count_no_reply,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x05, 0x00, 0x50, 0xFF, 0x00 };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);
    dev.recv_id = MODBUS_ID_BROADCAST;
    dev.recv_id_set = true;

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(-ENOENT, modbus_server_listen(&server, &message));
    TEST_ASSERT_EQUAL_INT(0, state.call_count);
}

static void test_modbus_server_listen__broadcast_dispatch_multiple(void)
{
    /* a broadcast request must be delivered to every broadcast-capable
     * server */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state_a = { 0 };
    cb_state_t state_b = { 0 };
    cb_state_t state_c = { 0 };
    modbus_server_t a = {
        .id = 1,
        .flags = MODBUS_SERVER_FLAG_RECEIVE_BROADCAST,
        .cb = cb_count_no_reply,
        .arg = &state_a,
    };
    modbus_server_t b = {
        .id = 2,
        .flags = MODBUS_SERVER_FLAG_RECEIVE_BROADCAST,
        .cb = cb_count_no_reply,
        .arg = &state_b,
    };
    modbus_server_t c = {
        .id = 3,
        /* no broadcast flag — should NOT receive the broadcast */
        .cb = cb_count_no_reply,
        .arg = &state_c,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &a);
    modbus_server_add(&server, &b);
    modbus_server_add(&server, &c);

    uint8_t request_pdu[] = { 0x05, 0x00, 0x50, 0xFF, 0x00 };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);
    dev.recv_id = MODBUS_ID_BROADCAST;
    dev.recv_id_set = true;

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));

    TEST_ASSERT_EQUAL_INT(1, state_a.call_count);
    TEST_ASSERT_EQUAL_INT(1, state_b.call_count);
    TEST_ASSERT_EQUAL_INT(0, state_c.call_count);
}

static void test_modbus_server_listen__receive_any(void)
{
    /* a server with RECEIVE_ANY receives traffic for arbitrary ids */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 99,
        .flags = MODBUS_SERVER_FLAG_RECEIVE_ANY,
        .cb = cb_count_no_reply,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);
    dev.recv_id = 5; /* not 99, but RECEIVE_ANY accepts it */
    dev.recv_id_set = true;

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));
    TEST_ASSERT_EQUAL_INT(1, state.call_count);
}

static void test_modbus_server_listen__callback_sets_exception(void)
{
    /* exception responses from the callback are encoded and sent */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_exception,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x50, 0x00, 0x08 };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(0, modbus_server_listen(&server, &message));
    TEST_ASSERT_EQUAL_INT(MODBUS_EXC_ILLEGAL_ADDRESS, message.exc);

    /* exception response: 0x81 = 0x01 | 0x80, exception code 0x02 */
    uint8_t expected_response_pdu[] = { 0x81, 0x02 };
    TEST_ASSERT_EQUAL_INT(sizeof(expected_response_pdu), dev.send_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(dev.send_buf, expected_response_pdu,
                                    sizeof(expected_response_pdu)));
}

static void test_modbus_server_listen__callback_returns_error(void)
{
    /* a non-zero return value from the callback is propagated unchanged */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_returns_error,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(-1, modbus_server_listen(&server, &message));
}

static void test_modbus_server_listen__decode_error(void)
{
    /* a request with the exception bit set is malformed */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_read_coils,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x81, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_server_listen(&server, &message));
    TEST_ASSERT_EQUAL_INT(0, state.call_count);
}

static void test_modbus_server_listen__missing_id(void)
{
    /* if the transport fails to fill in the message id, listen fails */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    cb_state_t state = { 0 };
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_read_coils,
        .arg = &state,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    uint8_t request_pdu[] = { 0x01, 0x00, 0x1D, 0x00, 0x1F };
    memcpy(dev.recv_buf, request_pdu, sizeof(request_pdu));
    dev.recv_len = sizeof(request_pdu);
    dev.recv_skip_set_id = true;

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(-EPROTO, modbus_server_listen(&server, &message));
    TEST_ASSERT_EQUAL_INT(0, state.call_count);
}

static void test_modbus_server_listen__timeout(void)
{
    /* errors from the transport's recv function are propagated */
    modbus_mock_t dev = { 0 };
    modbus_t server;
    modbus_server_t modbus_server = {
        .id = 1,
        .cb = cb_reply_read_coils,
    };

    modbus_init(&server, &_mock_transport, &dev, NULL);
    modbus_server_add(&server, &modbus_server);

    /* recv_len=0 makes the mock return -ETIMEDOUT */
    dev.recv_len = 0;

    modbus_message_t message = { 0 };

    TEST_ASSERT_EQUAL_INT(-ETIMEDOUT, modbus_server_listen(&server, &message));
}

static void test_modbus_encode_request__write_single_holding_register__invalid_data(void)
{
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xAB, 0xCD };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER,
        .addr = 0x0004,
        .count = 1,
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));

    /* data size too small */
    msg.data = data;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_request__write_multiple_coils__invalid_data(void)
{
    uint8_t buf[8] = { 0 };
    uint8_t data[] = { 0x4D, 0x01 };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_COILS,
        .addr = 0x001B,
        .count = 9, /* needs 2 bytes of data */
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));

    /* data size too small */
    msg.data = data;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_request__write_multiple_holding_registers__invalid_data(void)
{
    uint8_t buf[10] = { 0 };
    uint8_t data[] = { 0x0B, 0x0A }; /* only 2 bytes, but 4 needed */

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS,
        .addr = 0x0012,
        .count = 2, /* needs 4 bytes of data */
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));

    /* data size too small */
    msg.data = data;
    msg.data_size = sizeof(data);
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_request(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__read_coils__invalid_data(void)
{
    uint8_t buf[6] = { 0 };
    uint8_t data[4];

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_COILS,
        .addr = 0x001D,
        .count = 31, /* size = 4 */
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));

    /* data points elsewhere but data_size is too small */
    msg.data = data;
    msg.data_size = 2; /* needs 4 */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__read_discrete_inputs__invalid_data(void)
{
    uint8_t buf[6] = { 0 };
    uint8_t data[4];

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28, /* size = 4 */
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));

    /* data points elsewhere but data_size is too small */
    msg.data = data;
    msg.data_size = 2;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__read_holding_registers__invalid_data(void)
{
    uint8_t buf[8] = { 0 };
    uint8_t data[6];

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3, /* size = 6 */
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));

    /* data points elsewhere but data_size is too small */
    msg.data = data;
    msg.data_size = 4; /* needs 6 */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__read_input_registers__invalid_data(void)
{
    uint8_t buf[4] = { 0 };
    uint8_t data[2];

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1, /* size = 2 */
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));

    /* data points elsewhere but data_size is too small */
    msg.data = data;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__write_single_coil__invalid_data(void)
{
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xFF, 0x00 };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_COIL,
        .addr = 0x00BF,
        .count = 1,
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));

    /* data size too small */
    msg.data = data;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_encode_response__write_single_holding_register__invalid_data(void)
{
    uint8_t buf[5] = { 0 };
    uint8_t data[] = { 0xAB, 0xCD };

    modbus_message_t msg = {
        .func = MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER,
        .addr = 0x0004,
        .count = 1,
        .data = NULL,
        .data_size = 0,
    };

    /* data is NULL */
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));

    /* data size too small */
    msg.data = data;
    msg.data_size = 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, modbus_encode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_request__read_discrete_inputs__invalid_count(void)
{
    /* count 0 */
    uint8_t buf_zero[] = { 0x02, 0x00, 0x7A, 0x00, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_zero, sizeof(buf_zero), &msg));

    /* count 2001 (maximum is 2000) */
    uint8_t buf_over[] = { 0x02, 0x00, 0x7A, 0x07, 0xD1 };
    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_over, sizeof(buf_over), &msg));
}

static void test_modbus_decode_request__read_input_registers__invalid_count(void)
{
    /* count 0 */
    uint8_t buf_zero[] = { 0x04, 0x00, 0x0A, 0x00, 0x00 };
    modbus_message_t msg = { 0 };

    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_zero, sizeof(buf_zero), &msg));

    /* count 126 (maximum is 125) */
    uint8_t buf_over[] = { 0x04, 0x00, 0x0A, 0x00, 0x7E };
    TEST_ASSERT_EQUAL_INT(-EBADMSG,
                          modbus_decode_request(buf_over, sizeof(buf_over), &msg));
}

static void test_modbus_decode_response__read_discrete_inputs__copy_too_small(void)
{
    uint8_t buf[] = { 0x02, 0x04, 0xAC, 0xDB, 0xFB, 0x0D };
    uint8_t data[2]; /* needs 4 bytes */

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_DISCRETE_INPUTS,
        .addr = 0x007A,
        .count = 28,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_response__read_holding_registers__copy_too_small(void)
{
    uint8_t buf[] = { 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40 };
    uint8_t data[4]; /* needs 6 bytes */

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_HOLDING_REGISTERS,
        .addr = 0x006F,
        .count = 3,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_decode_response(buf, sizeof(buf), &msg));
}

static void test_modbus_decode_response__read_input_registers__copy_too_small(void)
{
    uint8_t buf[] = { 0x04, 0x02, 0x10, 0x2F };
    uint8_t data[1]; /* needs 2 bytes */

    modbus_message_t msg = {
        .func = MODBUS_FC_READ_INPUT_REGISTERS,
        .addr = 0x000A,
        .count = 1,
        .data = data,
        .data_size = sizeof(data),
    };

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, modbus_decode_response(buf, sizeof(buf), &msg));
}

static Test *tests_modbus(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_modbus_init),

        new_TestFixture(test_modbus_server_registry),
        new_TestFixture(test_modbus_server_registry__duplicate_id),
        new_TestFixture(test_modbus_server_registry__invalid_id),
        new_TestFixture(test_modbus_server_registry__remove_nonexistent),

        new_TestFixture(test_modbus_bit_count_to_size),
        new_TestFixture(test_modbus_reg_count_to_size),

        new_TestFixture(test_modbus_copy_bits),
        new_TestFixture(test_modbus_copy_bits__zero_count),
        new_TestFixture(test_modbus_copy_bits__no_pad_zeroes),
        new_TestFixture(test_modbus_copy_regs),
        new_TestFixture(test_modbus_copy_regs__zero_count),

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

        new_TestFixture(test_modbus_encode_request__read_coils),
        new_TestFixture(test_modbus_encode_request__read_discrete_inputs),
        new_TestFixture(test_modbus_encode_request__read_holding_registers),
        new_TestFixture(test_modbus_encode_request__read_input_registers),
        new_TestFixture(test_modbus_encode_request__write_single_coil),
        new_TestFixture(test_modbus_encode_request__write_single_holding_register),
        new_TestFixture(test_modbus_encode_request__write_multiple_coils),
        new_TestFixture(test_modbus_encode_request__write_multiple_holding_registers),
        new_TestFixture(test_modbus_encode_request__unsupported_function),
        new_TestFixture(test_modbus_encode_request__buffer_too_small),
        new_TestFixture(test_modbus_encode_request__buffer_too_small__write_single),
        new_TestFixture(test_modbus_encode_request__write_single_coil__invalid_data),
        new_TestFixture(test_modbus_encode_request__write_single_holding_register__invalid_data),
        new_TestFixture(test_modbus_encode_request__write_multiple_coils__invalid_data),
        new_TestFixture(test_modbus_encode_request__write_multiple_holding_registers__invalid_data),

        new_TestFixture(test_modbus_encode_response__read_coils),
        new_TestFixture(test_modbus_encode_response__read_coils__zero_copy),
        new_TestFixture(test_modbus_encode_response__read_discrete_inputs),
        new_TestFixture(test_modbus_encode_response__read_discrete_inputs__zero_copy),
        new_TestFixture(test_modbus_encode_response__read_holding_registers),
        new_TestFixture(test_modbus_encode_response__read_holding_registers__zero_copy),
        new_TestFixture(test_modbus_encode_response__read_input_registers),
        new_TestFixture(test_modbus_encode_response__read_input_registers__zero_copy),
        new_TestFixture(test_modbus_encode_response__write_single_coil),
        new_TestFixture(test_modbus_encode_response__write_single_coil__zero_copy),
        new_TestFixture(test_modbus_encode_response__write_single_holding_register),
        new_TestFixture(test_modbus_encode_response__write_multiple_coils),
        new_TestFixture(test_modbus_encode_response__write_multiple_holding_registers),
        new_TestFixture(test_modbus_encode_response__exception),
        new_TestFixture(test_modbus_encode_response__unsupported_function),
        new_TestFixture(test_modbus_encode_response__buffer_too_small),
        new_TestFixture(test_modbus_encode_response__buffer_too_small__write),
        new_TestFixture(test_modbus_encode_response__read_coils__invalid_data),
        new_TestFixture(test_modbus_encode_response__read_discrete_inputs__invalid_data),
        new_TestFixture(test_modbus_encode_response__read_holding_registers__invalid_data),
        new_TestFixture(test_modbus_encode_response__read_input_registers__invalid_data),
        new_TestFixture(test_modbus_encode_response__write_single_coil__invalid_data),
        new_TestFixture(test_modbus_encode_response__write_single_holding_register__invalid_data),

        new_TestFixture(test_modbus_decode_request__read_coils),
        new_TestFixture(test_modbus_decode_request__read_discrete_inputs),
        new_TestFixture(test_modbus_decode_request__read_holding_registers),
        new_TestFixture(test_modbus_decode_request__read_input_registers),
        new_TestFixture(test_modbus_decode_request__write_single_coil),
        new_TestFixture(test_modbus_decode_request__write_single_holding_register),
        new_TestFixture(test_modbus_decode_request__write_multiple_coils),
        new_TestFixture(test_modbus_decode_request__write_multiple_holding_registers),
        new_TestFixture(test_modbus_decode_request__unsupported_function),
        new_TestFixture(test_modbus_decode_request__exception_bit_set),
        new_TestFixture(test_modbus_decode_request__truncated),
        new_TestFixture(test_modbus_decode_request__write_multiple_coils__size_mismatch),
        new_TestFixture(
            test_modbus_decode_request__write_multiple_holding_registers__size_mismatch),
        new_TestFixture(test_modbus_decode_request__read_coils__invalid_count),
        new_TestFixture(test_modbus_decode_request__read_discrete_inputs__invalid_count),
        new_TestFixture(test_modbus_decode_request__read_holding_registers__invalid_count),
        new_TestFixture(test_modbus_decode_request__read_input_registers__invalid_count),
        new_TestFixture(test_modbus_decode_request__write_multiple_coils__invalid_count),
        new_TestFixture(test_modbus_decode_request__write_multiple_regs__invalid_count),

        new_TestFixture(test_modbus_decode_response__read_coils__zero_copy),
        new_TestFixture(test_modbus_decode_response__read_coils__copy),
        new_TestFixture(test_modbus_decode_response__read_coils__copy_too_small),
        new_TestFixture(test_modbus_decode_response__read_discrete_inputs__copy_too_small),
        new_TestFixture(test_modbus_decode_response__read_holding_registers__copy_too_small),
        new_TestFixture(test_modbus_decode_response__read_input_registers__copy_too_small),
        new_TestFixture(test_modbus_decode_response__read_discrete_inputs__zero_copy),
        new_TestFixture(test_modbus_decode_response__read_discrete_inputs__copy),
        new_TestFixture(test_modbus_decode_response__read_holding_registers__zero_copy),
        new_TestFixture(test_modbus_decode_response__read_holding_registers__copy),
        new_TestFixture(test_modbus_decode_response__read_input_registers__zero_copy),
        new_TestFixture(test_modbus_decode_response__read_input_registers__copy),
        new_TestFixture(test_modbus_decode_response__write_single_coil),
        new_TestFixture(test_modbus_decode_response__write_single_holding_register),
        new_TestFixture(test_modbus_decode_response__write_multiple_coils),
        new_TestFixture(test_modbus_decode_response__write_multiple_holding_registers),
        new_TestFixture(test_modbus_decode_response__exception),
        new_TestFixture(test_modbus_decode_response__exception_with_none),
        new_TestFixture(test_modbus_decode_response__addr_mismatch),
        new_TestFixture(test_modbus_decode_response__count_mismatch),
        new_TestFixture(test_modbus_decode_response__size_mismatch),
        new_TestFixture(test_modbus_decode_response__truncated),
        new_TestFixture(test_modbus_decode_response__unsupported_function),

        new_TestFixture(test_modbus_client_request__round_trip),
        new_TestFixture(test_modbus_client_request__zero_copy),
        new_TestFixture(test_modbus_client_request__copy),
        new_TestFixture(test_modbus_client_request__exception_response),
        new_TestFixture(test_modbus_client_request__unsupported_function),
        new_TestFixture(test_modbus_client_request__wrong_id),
        new_TestFixture(test_modbus_client_request__wrong_function),
        new_TestFixture(test_modbus_client_request__missing_id),
        new_TestFixture(test_modbus_client_request__send_error),
        new_TestFixture(test_modbus_client_request__timeout),

        new_TestFixture(test_modbus_client_broadcast__sets_broadcast_id),
        new_TestFixture(test_modbus_client_broadcast__invalid_function),
        new_TestFixture(test_modbus_client_broadcast__send_error),

        new_TestFixture(test_modbus_server_listen__round_trip),
        new_TestFixture(test_modbus_server_listen__zero_copy),
        new_TestFixture(test_modbus_server_listen__copy),
        new_TestFixture(test_modbus_server_listen__no_server_for_id),
        new_TestFixture(test_modbus_server_listen__broadcast_dispatch),
        new_TestFixture(test_modbus_server_listen__broadcast_no_match),
        new_TestFixture(test_modbus_server_listen__broadcast_dispatch_multiple),
        new_TestFixture(test_modbus_server_listen__receive_any),
        new_TestFixture(test_modbus_server_listen__callback_sets_exception),
        new_TestFixture(test_modbus_server_listen__callback_returns_error),
        new_TestFixture(test_modbus_server_listen__decode_error),
        new_TestFixture(test_modbus_server_listen__missing_id),
        new_TestFixture(test_modbus_server_listen__timeout),
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
