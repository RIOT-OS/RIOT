/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "clist.h"
#include "nomac.h"
#include "netdev_dummy.h"
#include "thread.h"
#include "vtimer.h"

#include "embUnit/embUnit.h"

#include "tests-nomac.h"

#define UNITTEST_DONE               (TEST_UINT16)
#define UNITTESTS_NOMAC_NAME        "unittests_nomac"
#define UNITTESTS_NOMAC_STACKSIZE   (KERNEL_CONF_STACKSIZE_DEFAULT)
#define UNITTESTS_NOMAC_QSIZE       (4)

static char unittests_nomac_stack[UNITTESTS_NOMAC_STACKSIZE];
static char unittests_nomac_rcv_stack[KERNEL_CONF_STACKSIZE_IDLE];
static msg_t unittests_nomac_rcv_queue[UNITTESTS_NOMAC_QSIZE];

static netdev_t *dev = &(unittest_netdev_dummy_devs[0]);
static kernel_pid_t nomac_pid = KERNEL_PID_UNDEF;
static kernel_pid_t nomac_receiver = KERNEL_PID_UNDEF;
static kernel_pid_t main_pid = KERNEL_PID_UNDEF;

static char received_data[UNITTESTS_NETDEV_DUMMY_MAX_PACKET];
static char received_src[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
static char received_dst[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
static size_t received_data_len = 0, received_src_len = 0, received_dst_len = 0;

#define SET_OPTION_NUMBER   (3)
#define GET_OPTION_NUMBER   (7)
#define RANDOM_SET_OPTION   ((TEST_UINT8 % SET_OPTION_NUMBER) + 1)
#define RANDOM_GET_OPTION   (TEST_UINT8 % SET_OPTION_NUMBER)

static void _reset_received(void)
{
    memset(received_data, 0, sizeof(received_data));
    memset(received_src, 0, sizeof(received_src));
    memset(received_dst, 0, sizeof(received_dst));
    received_data_len = 0;
    received_src_len = 0;
    received_dst_len = 0;
}

static void _fill_received(netapi_rcv_pkt_t *rcv)
{
    netapi_ack_t *ack = rcv->ack;
    ack->type = NETAPI_CMD_ACK;
    ack->orig = rcv->type;

    if (rcv->type != NETAPI_CMD_RCV) {
        ack->result = -ENOMSG;
        return;
    }

    memcpy(received_src, rcv->src, rcv->src_len);
    received_src_len = rcv->src_len;
    memcpy(received_dst, rcv->dest, rcv->dest_len);
    received_dst_len = rcv->dest_len;
    memcpy(received_data, rcv->data, rcv->data_len);
    received_data_len = rcv->data_len;

    ack->result = received_data_len;
    return;
}

static void *test_nomac_receive_thread(void *args)
{
    msg_t msg_rcv, msg_ack, done;

    netapi_rcv_pkt_t *rcv;

    (void)args;

    msg_init_queue(unittests_nomac_rcv_queue, UNITTESTS_NOMAC_QSIZE);

    msg_ack.type = NETAPI_MSG_TYPE;
    done.type = UNITTEST_DONE;

    msg_receive(&msg_rcv);

    if (msg_rcv.type != NETAPI_MSG_TYPE) {
        msg_ack.type = 0;   /* Send arbitrary reply, API handles this case */
        msg_reply(&msg_rcv, &msg_ack);
        msg_send(&done, main_pid);
        return NULL;
    }

    rcv = (netapi_rcv_pkt_t *)(msg_rcv.content.ptr);
    msg_ack.content.ptr = (char *)rcv->ack;

    _fill_received(rcv);

    msg_reply(&msg_rcv, &msg_ack);
    msg_send(&done, main_pid);
    return NULL;
}

static void set_up(void)
{
    unittest_netdev_dummy_init();
    dev->driver->init(dev);
    nomac_update_callback(dev);
    nomac_init_module();
}

static void set_up_with_thread(void)
{
    set_up();
    main_pid = thread_getpid();
    nomac_receiver = thread_create(unittests_nomac_rcv_stack,
                                   sizeof(unittests_nomac_stack),
                                   PRIORITY_MAIN - 3, 0,
                                   test_nomac_receive_thread, NULL,
                                   "nomac_rcv");
}

static void tear_down(void)
{
    _reset_received();
    nomac_receiver = KERNEL_PID_UNDEF;
}

/*********************************************
 * thread tests                              *
 *********************************************/
static void test_nomac_thread(void)
{
    TEST_ASSERT(KERNEL_PID_UNDEF != nomac_pid);
}

/*********************************************
 * NETAPI_CMD_SND tests                      *
 *********************************************/
static void test_nomac_send_data_dest_too_long(void)
{
    char dest[] = TEST_STRING8;
    char data[] = TEST_STRING64;
    TEST_ASSERT_EQUAL_INT(-EAFNOSUPPORT, netapi_send_payload(nomac_pid, dest,
                          UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN + TEST_UINT8,
                          data, 1));
}

static void test_nomac_send_data_data_too_long(void)
{
    char dest[] = TEST_STRING8;
    char data[] = TEST_STRING64;
    TEST_ASSERT_EQUAL_INT(-EMSGSIZE, netapi_send_payload(nomac_pid, dest,
                          UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN, data,
                          UNITTESTS_NETDEV_DUMMY_MAX_PACKET + TEST_UINT8));
}

static void test_nomac_send_data_send(void)
{
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING8;
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET < 8
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#else
    size_t data_len = 8;
#endif
    TEST_ASSERT_EQUAL_INT((int)data_len, netapi_send_payload(nomac_pid,
                          dest, dest_len, data, data_len));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_check_transmitted(dev,
                          dest, dest_len, data, data_len));
}

#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET > 4
static void test_nomac_send_data_send2(void)
{
    netdev_hlist_t hlist_node = {NULL, NULL, NETDEV_PROTO_UNKNOWN, TEST_STRING8, 4};
    netdev_hlist_t *hlist = NULL;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING16;
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET < 12
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET - 4;
    char expected[UNITTESTS_NETDEV_DUMMY_MAX_PACKET];
#else
    size_t data_len = 8;
    char expected[12];
#endif

    memcpy(expected, TEST_STRING8, 4);
    memcpy(&(expected[4]), TEST_STRING16, data_len);

    clist_add((clist_node_t **)&hlist, (clist_node_t *)&hlist_node);
    TEST_ASSERT_EQUAL_INT((int)sizeof(expected), netapi_send_packet(nomac_pid,
                          hlist, dest, dest_len, data, data_len));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_check_transmitted(dev,
                          dest, dest_len, expected, data_len + 4));
}
#endif

/*********************************************
 * NETAPI_CMD_REG tests                      *
 *********************************************/
static void test_nomac_reg_buffer_full(void)
{
    for (int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, netapi_register(nomac_pid, (kernel_pid_t)(i + 1), 0));
    }

    TEST_ASSERT_EQUAL_INT(-ENOBUFS,
                          netapi_register_current_thread(nomac_pid, 0));
}

static void test_nomac_register(void)
{
    msg_t done;
    char src[] = TEST_STRING64;
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING8;
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET < 8
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#else
    size_t data_len = 8;
#endif
    int res;

    TEST_ASSERT(nomac_receiver != KERNEL_PID_UNDEF);
    TEST_ASSERT_EQUAL_INT(0, netapi_register(nomac_pid, nomac_receiver, 0));
    res = netapi_fire_receive_event(nomac_pid, src, src_len, dest, dest_len, data, data_len);
    TEST_ASSERT_EQUAL_INT(0, res);

    msg_receive(&done);

    TEST_ASSERT_EQUAL_INT(UNITTEST_DONE, done.type);
    TEST_ASSERT_EQUAL_INT(src_len, received_src_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_src, src, src_len));
    TEST_ASSERT_EQUAL_INT(dest_len, received_dst_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_dst, dest, dest_len));
    TEST_ASSERT_EQUAL_INT(data_len, received_data_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_data, data, data_len));
}

/*********************************************
 * NETAPI_CMD_GET tests                      *
 *********************************************/

static void test_nomac_get_option_null_data(void)
{
    size_t value_len = sizeof(unsigned int);

    TEST_ASSERT_EQUAL_INT(-EFAULT, netapi_get_option(nomac_pid,
                          RANDOM_GET_OPTION, NULL, value_len));
}

static void test_nomac_get_option_overflow(void)
{
    unsigned int value = TEST_UINT32;

    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, netapi_get_option(nomac_pid,
                          RANDOM_GET_OPTION, &value, 0));
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, value);
}

static void test_nomac_get_option_not_supported(void)
{
    unsigned int value = TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, netapi_get_option(nomac_pid,
                          (netapi_conf_type_t)UINT32_MAX, &value,
                          value_len));
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, value);
}

static void test_nomac_get_option_get_protocol(void)
{
    netdev_proto_t value = TEST_UINT8 % 9;
    TEST_ASSERT_EQUAL_INT(sizeof(netdev_proto_t),
                          netapi_get_option(nomac_pid,
                                            NOMAC_PROTO, &value,
                                            sizeof(netdev_proto_t)));
    TEST_ASSERT_EQUAL_INT(NETDEV_PROTO_UNKNOWN, value);
}

static void test_nomac_get_option_get_channel(void)
{
    unsigned int value = TEST_UINT32;
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int),
                          netapi_get_option(nomac_pid,
                                            NOMAC_CHANNEL, &value,
                                            sizeof(unsigned int)));
    TEST_ASSERT_EQUAL_INT(0, value);
}

static void test_nomac_get_option_get_nid(void)
{
    unsigned int value = TEST_UINT32;
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int),
                          netapi_get_option(nomac_pid,
                                            NOMAC_NID, &value,
                                            sizeof(unsigned int)));
    TEST_ASSERT_EQUAL_INT(0, value);
}

static void test_nomac_get_option_get_address(void)
{
    uint8_t value[UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN];
    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN,
                          netapi_get_option(nomac_pid,
                                            NOMAC_ADDRESS, &value,
                                            UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN));

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN; i++) {
        TEST_ASSERT_EQUAL_INT(0, value[i]);
    }
}

static void test_nomac_get_option_get_long_address(void)
{
    uint8_t value[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN,
                          netapi_get_option(nomac_pid,
                                            NOMAC_ADDRESS2, &value,
                                            UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN));

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN; i++) {
        TEST_ASSERT_EQUAL_INT(0, value[i]);
    }
}

/*********************************************
 * NETAPI_CMD_SET tests                      *
 *********************************************/

static void test_nomac_set_option_null_data(void)
{
    size_t value_len = sizeof(unsigned int);

    TEST_ASSERT_EQUAL_INT(-EFAULT, netapi_set_option(nomac_pid,
                          RANDOM_SET_OPTION, NULL, value_len));
}

static void test_nomac_set_option_inval(void)
{
    unsigned int value = TEST_UINT32;
    size_t value_len = 0;

    TEST_ASSERT_EQUAL_INT(-EINVAL, netapi_set_option(nomac_pid,
                          RANDOM_SET_OPTION, &value, value_len));
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, value);
}

static void test_nomac_set_option_overflow(void)
{
    unsigned int value = TEST_UINT32;
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN + TEST_UINT16;

    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, netapi_set_option(nomac_pid,
                          RANDOM_SET_OPTION, &value, value_len));
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, value);
}

static void test_nomac_set_option_not_supported(void)
{
    unsigned int value = TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, netapi_set_option(nomac_pid,
                          (netapi_conf_type_t)UINT32_MAX, &value,
                          value_len));
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, value);
}

static void test_nomac_set_get_channel(void)
{
    unsigned int value = TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(0, netapi_set_option(nomac_pid,
                          NOMAC_CHANNEL, &value, value_len));
    value = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int),
                          netapi_get_option(nomac_pid, NOMAC_CHANNEL,
                                            &value, value_len));
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, value);
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int), value_len);
}

static void test_nomac_set_get_address(void)
{
    char value[UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN];
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
#if UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN < 12
    strncpy(value, TEST_STRING12, UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN);
#else
    strncpy(value, TEST_STRING12, 12);
#endif
    TEST_ASSERT_EQUAL_INT(0, netapi_set_option(nomac_pid,
                          NOMAC_ADDRESS, value,
                          UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN));
#if UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN < 12
    memset(value, 0, UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN);
#else
    memset(value, 0, 12);
#endif
    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN,
                          netapi_get_option(nomac_pid,
                                            NOMAC_ADDRESS, value,
                                            value_len));
#if UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN < 12
    TEST_ASSERT(strncmp(value, TEST_STRING12,
                        UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) == 0);
#else
    TEST_ASSERT(strncmp(value, TEST_STRING12, 12) == 0);
#endif
}

static void test_nomac_set_get_nid(void)
{
    unsigned int value = TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(0, netapi_set_option(nomac_pid,
                          NOMAC_NID, &value, value_len));
    value = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int),
                          netapi_get_option(nomac_pid, NOMAC_NID,
                                            &value, value_len));
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, value);
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int), value_len);
}

static void test_nomac_set_get_long_address(void)
{
    char value[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN;
#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN < 12
    strncpy(value, TEST_STRING12, UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN);
#else
    strncpy(value, TEST_STRING12, 12);
#endif
    TEST_ASSERT_EQUAL_INT(0, netapi_set_option(nomac_pid,
                          NOMAC_ADDRESS2, value,
                          UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN));
#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN < 12
    memset(value, 0, UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN);
#else
    memset(value, 0, 12);
#endif
    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN,
                          netapi_get_option(nomac_pid,
                                            NOMAC_ADDRESS2, value,
                                            value_len));
#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN < 12
    TEST_ASSERT(strncmp(value, TEST_STRING12,
                        UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN) == 0);
#else
    TEST_ASSERT(strncmp(value, TEST_STRING12, 12) == 0);
#endif
    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN, value_len);
}

/****************************
 * ISR event tests          *
 ****************************/
static void test_nomac_event_value(void)
{
    msg_t m;
    m.type = NETDEV_MSG_EVENT_TYPE;
    m.content.value = TEST_UINT32;
    msg_send(&m, nomac_pid);
    vtimer_usleep(1000);
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, unittest_netdev_dummy_get_last_event(dev));
}

Test *tests_nomac_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* thread tests */
        new_TestFixture(test_nomac_thread),
        /* NETAPI_CMD_SND tests */
        new_TestFixture(test_nomac_send_data_dest_too_long),
        new_TestFixture(test_nomac_send_data_data_too_long),
        new_TestFixture(test_nomac_send_data_send),
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET > 4
        new_TestFixture(test_nomac_send_data_send2),
#endif
        /* NETAPI_CMD_REG tests */
        new_TestFixture(test_nomac_reg_buffer_full),
        /* NETAPI_CMD_GET tests */
        new_TestFixture(test_nomac_get_option_null_data),
        new_TestFixture(test_nomac_get_option_overflow),
        new_TestFixture(test_nomac_get_option_not_supported),
        new_TestFixture(test_nomac_get_option_get_protocol),
        new_TestFixture(test_nomac_get_option_get_channel),
        new_TestFixture(test_nomac_get_option_get_address),
        new_TestFixture(test_nomac_get_option_get_nid),
        new_TestFixture(test_nomac_get_option_get_long_address),
        /* NETAPI_CMD_SET tests */
        new_TestFixture(test_nomac_set_option_null_data),
        new_TestFixture(test_nomac_set_option_inval),
        new_TestFixture(test_nomac_set_option_overflow),
        new_TestFixture(test_nomac_set_option_not_supported),
        new_TestFixture(test_nomac_set_get_channel),
        new_TestFixture(test_nomac_set_get_address),
        new_TestFixture(test_nomac_set_get_nid),
        new_TestFixture(test_nomac_set_get_long_address),
        /* ISR event tests */
        new_TestFixture(test_nomac_event_value),
    };

    EMB_UNIT_TESTCALLER(nomac_tests, set_up, tear_down,
                        fixtures);

    return (Test *)&nomac_tests;
}

Test *tests_nomac_threaded_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nomac_register),
    };

    EMB_UNIT_TESTCALLER(nomac_tests, set_up_with_thread, tear_down,
                        fixtures);

    return (Test *)&nomac_tests;
}

void tests_nomac(void)
{
    set_up();
    vtimer_init();

    nomac_pid = nomac_init(unittests_nomac_stack,
                           UNITTESTS_NOMAC_STACKSIZE,
                           PRIORITY_MAIN - 1, UNITTESTS_NOMAC_NAME,
                           dev);

    TESTS_RUN(tests_nomac_tests());
    TESTS_RUN(tests_nomac_threaded_tests());
}
