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

#include "embUnit/embUnit.h"

#include "netdev_dummy.h"

#include "tests-netdev_dummy.h"

static netdev_t *dev = &(unittest_netdev_dummy_devs[0]);
static char received_data[UNITTESTS_NETDEV_DUMMY_MAX_PACKET];
static char received_src[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
static char received_dst[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
static size_t received_data_len = 0, received_src_len = 0, received_dst_len = 0;

#define OPTION_NUMBER   (6)
#define RANDOM_OPTION   (TEST_UINT8 % OPTION_NUMBER) + 1

static void _reset_received(void)
{
    memset(received_data, 0, sizeof(received_data));
    memset(received_src, 0, sizeof(received_src));
    memset(received_dst, 0, sizeof(received_dst));
    received_data_len = 0;
    received_src_len = 0;
    received_dst_len = 0;
}

static void set_up(void)
{
    unittest_netdev_dummy_init();
    dev->driver->init(dev);
}

static void tear_down(void)
{
    _reset_received();
}

/* callback for callback tests */
int _fill_received(netdev_t *dev_rcv, void *src, size_t src_len, void *dest,
                   size_t dest_len, void *payload, size_t payload_len)
{
    if ((dev != dev_rcv) || (payload_len > sizeof(received_data)) ||
        (src_len > sizeof(received_src)) || (dest_len > sizeof(received_dst))) {
        /* should not happen */
        return -1;
    }

    memcpy(received_src, src, src_len);
    received_src_len = src_len;
    memcpy(received_dst, dest, dest_len);
    received_dst_len = dest_len;
    memcpy(received_data, payload, payload_len);
    received_data_len = payload_len;

    return 0;
}

/* callback for callback tests */
int _always_wrong(netdev_t *dev_rcv, void *src, size_t src_len, void *dest,
                  size_t dest_len, void *payload, size_t payload_len)
{
    (void)dev_rcv;
    (void)src;
    (void)src_len;
    (void)dest;
    (void)dest_len;
    (void)payload;
    (void)payload_len;
    return -1;
}

/*********************************************
 * driver::init tests                        *
 *********************************************/
static void test_netdev_dummy_init_dev_null(void)
{
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->init(NULL));
}

static void test_netdev_dummy_init_dev_wrong(void)
{
    netdev_t wrong_dev;

    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->init(&wrong_dev));
}

/*********************************************
 * driver::send_data tests                   *
 *********************************************/

static void test_netdev_dummy_send_data_dev_null(void)
{
    char dest[] = TEST_STRING8;
    char data[] = TEST_STRING64;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->send_data(NULL, dest,
                          UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN, NULL,
                          data, strlen(data)));
}

static void test_netdev_dummy_send_data_dev_wrong(void)
{
    char dest[] = TEST_STRING8;
    char data[] = TEST_STRING64;
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->send_data(&wrong_dev, dest,
                          UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN, NULL,
                          data, strlen(data)));
}

static void test_netdev_dummy_send_data_dest_too_long(void)
{
    char dest[] = TEST_STRING8;
    char data[] = TEST_STRING64;
    TEST_ASSERT_EQUAL_INT(-EAFNOSUPPORT, dev->driver->send_data(dev, dest,
                          UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN + TEST_UINT8,
                          NULL, data, 1));
}

static void test_netdev_dummy_send_data_data_too_long(void)
{
    char dest[] = TEST_STRING8;
    char data[] = TEST_STRING64;
    TEST_ASSERT_EQUAL_INT(-EMSGSIZE, dev->driver->send_data(dev, dest,
                          UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN, NULL,
                          data,
                          UNITTESTS_NETDEV_DUMMY_MAX_PACKET + TEST_UINT8));
}

static void test_netdev_dummy_send_data_no_ulhs(void)
{
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING8;
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET < 8
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#else
    size_t data_len = 8;
#endif
    TEST_ASSERT_EQUAL_INT((int)data_len, dev->driver->send_data(dev, dest,
                          dest_len, NULL, data, data_len));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_check_transmitted(dev,
                          dest, dest_len, data, data_len));
}

#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET > 4
static void test_netdev_dummy_send_data_with_ulhs(void)
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
    TEST_ASSERT_EQUAL_INT((int)sizeof(expected), dev->driver->send_data(dev,
                          dest, dest_len, hlist, data, data_len));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_check_transmitted(dev,
                          dest, dest_len, expected, data_len + 4));
}
#endif

/*********************************************
 * driver::add_receive_data_callback tests   *
 *********************************************/

static void test_netdev_dummy_add_cb_dev_null(void)
{
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->add_receive_data_callback(NULL,
                          _fill_received));
}

static void test_netdev_dummy_add_cb_dev_wrong(void)
{
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->add_receive_data_callback(&wrong_dev,
                          _fill_received));
}

static void test_netdev_dummy_add_cb_buffer_full(void)
{
    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_CB; i++) {

        TEST_ASSERT_EQUAL_INT(0, dev->driver->add_receive_data_callback(dev,
                              (netdev_rcv_data_cb_t)(i + 1)));
    }

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, dev->driver->add_receive_data_callback(dev,
                          _fill_received));
}

static void test_netdev_dummy_add_cb_successful(void)
{
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
    TEST_ASSERT_EQUAL_INT(0, dev->driver->add_receive_data_callback(dev,
                          _fill_received));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_fire_rcv_event(dev, src,
                          src_len, dest, dest_len, data, data_len));
    TEST_ASSERT_EQUAL_INT(src_len, received_src_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_src, src, src_len));
    TEST_ASSERT_EQUAL_INT(dest_len, received_dst_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_dst, dest, dest_len));
    TEST_ASSERT_EQUAL_INT(data_len, received_data_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_data, data, data_len));
}

/*********************************************
 * driver::rem_receive_data_callback tests   *
 *********************************************/

static void test_netdev_dummy_rem_cb_dev_null(void)
{
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->rem_receive_data_callback(NULL,
                          _fill_received));
}

static void test_netdev_dummy_rem_cb_dev_wrong(void)
{
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->rem_receive_data_callback(
                              &wrong_dev, _fill_received));
}

static void test_netdev_dummy_rem_cb_unknown_cb(void)
{
    TEST_ASSERT_EQUAL_INT(0, dev->driver->rem_receive_data_callback(
                              dev, _fill_received));
}

static void test_netdev_dummy_rem_cb_successful(void)
{
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
    TEST_ASSERT_EQUAL_INT(0, dev->driver->add_receive_data_callback(dev,
                          _fill_received));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_fire_rcv_event(dev, src,
                          src_len, dest, dest_len, data, data_len));
    _reset_received();
    TEST_ASSERT_EQUAL_INT(0, dev->driver->rem_receive_data_callback(dev,
                          _fill_received));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_fire_rcv_event(dev, src,
                          src_len, dest, dest_len, data, data_len));
    TEST_ASSERT_EQUAL_INT(0, received_src_len);
    TEST_ASSERT_EQUAL_INT(0, received_dst_len);
    TEST_ASSERT_EQUAL_INT(0, received_data_len);

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN; i++) {
        TEST_ASSERT_EQUAL_INT(0, received_src[i]);
    }

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN; i++) {
        TEST_ASSERT_EQUAL_INT(0, received_dst[i]);
    }

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_PACKET; i++) {
        TEST_ASSERT_EQUAL_INT(0, received_data[i]);
    }
}

static void test_netdev_dummy_rem_cb_multiple_added(void)
{
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
    TEST_ASSERT_EQUAL_INT(0, dev->driver->add_receive_data_callback(dev,
                          _fill_received));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_fire_rcv_event(dev, src,
                          src_len, dest, dest_len, data, data_len));
    _reset_received();
    TEST_ASSERT_EQUAL_INT(0, dev->driver->add_receive_data_callback(dev,
                          _always_wrong));
    TEST_ASSERT_EQUAL_INT(-ECANCELED, unittest_netdev_dummy_fire_rcv_event(
                              dev, src, src_len, dest, dest_len, data, data_len));
    TEST_ASSERT_EQUAL_INT(0, dev->driver->rem_receive_data_callback(dev,
                          _always_wrong));
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_fire_rcv_event(dev, src,
                          src_len, dest, dest_len, data, data_len));
    TEST_ASSERT_EQUAL_INT(src_len, received_src_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_src, src, src_len));
    TEST_ASSERT_EQUAL_INT(dest_len, received_dst_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_dst, dest, dest_len));
    TEST_ASSERT_EQUAL_INT(data_len, received_data_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(received_data, data, data_len));
}

/*********************************************
 * driver::get_option tests                  *
 *********************************************/

static void test_netdev_dummy_get_option_dev_null(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->get_option(NULL, RANDOM_OPTION,
                          &value, &value_len));
}

static void test_netdev_dummy_get_option_dev_wrong(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->get_option(&wrong_dev,
                          RANDOM_OPTION, &value, &value_len));
}

static void test_netdev_dummy_get_option_null_value(void)
{
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(-EFAULT, dev->driver->get_option(dev,
                          RANDOM_OPTION, NULL, &value_len));
}

static void test_netdev_dummy_get_option_null_value_len(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    TEST_ASSERT_EQUAL_INT(-EFAULT, dev->driver->get_option(dev,
                          RANDOM_OPTION, &value, NULL));
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
}

static void test_netdev_dummy_get_option_overflow(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = 0;
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, dev->driver->get_option(dev,
                          RANDOM_OPTION, &value, &value_len));
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
}

static void test_netdev_dummy_get_option_not_supported(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = 0;
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, dev->driver->get_option(dev,
                          (netdev_opt_t)0xffffffff, &value, &value_len));
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
}

static void test_netdev_dummy_get_channel(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = sizeof(unsigned int) + TEST_UINT8;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_CHANNEL,
                          &value, &value_len));
    TEST_ASSERT_EQUAL_INT(0, value);
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int), value_len);
}

static void test_netdev_dummy_get_address(void)
{
    uint8_t value[UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN];
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN + TEST_UINT8;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_ADDRESS,
                          value, &value_len));

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN; i++) {
        TEST_ASSERT_EQUAL_INT(0, value[i]);
    }

    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN, value_len);
}

static void test_netdev_dummy_get_nid(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = sizeof(unsigned int) + TEST_UINT8;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_NID,
                          &value, &value_len));
    TEST_ASSERT_EQUAL_INT(0, value);
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int), value_len);
}

static void test_netdev_dummy_get_long_address(void)
{
    uint8_t value[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN + TEST_UINT8;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_ADDRESS_LONG,
                          value, &value_len));

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN; i++) {
        TEST_ASSERT_EQUAL_INT(0, value[i]);
    }

    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN, value_len);
}

static void test_netdev_dummy_get_tx_power(void)
{
    int value = TEST_INT;
    size_t value_len = sizeof(int) + TEST_UINT8;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_TX_POWER,
                          &value, &value_len));
    TEST_ASSERT_EQUAL_INT(0, value);
    TEST_ASSERT_EQUAL_INT(sizeof(int), value_len);
}

/*********************************************
 * driver::set_option tests                  *
 *********************************************/

static void test_netdev_dummy_set_option_dev_null(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    int res = -ENOTSUP;
    netdev_opt_t opt = RANDOM_OPTION;

    while (res == -ENOTSUP) {
        res = dev->driver->set_option(NULL, opt, &value, sizeof(unsigned int));

        opt = (opt + 1) % OPTION_NUMBER;
    }

    TEST_ASSERT_EQUAL_INT(-ENODEV, res);
}

static void test_netdev_dummy_set_option_dev_wrong(void)
{
    int value = (unsigned int)TEST_INT;
    netdev_t wrong_dev;
    int res = -ENOTSUP;
    netdev_opt_t opt = RANDOM_OPTION;

    while (res == -ENOTSUP) {
        res = dev->driver->set_option(&wrong_dev, opt, &value,
                                      sizeof(unsigned int));

        opt = (opt + 1) % OPTION_NUMBER;
    }

    TEST_ASSERT_EQUAL_INT(-ENODEV, res);
}

static void test_netdev_dummy_set_option_null_value(void)
{
    int res = -ENOTSUP;
    netdev_opt_t opt = RANDOM_OPTION;

    while (res == -ENOTSUP) {
        res = dev->driver->set_option(dev, opt, NULL, sizeof(unsigned int));

        opt = (opt + 1) % OPTION_NUMBER;
    }

    TEST_ASSERT_EQUAL_INT(-EFAULT, res);
}

static void test_netdev_dummy_set_option_inval(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    int res = -ENOTSUP;
    netdev_opt_t opt = RANDOM_OPTION;

    while (res == -ENOTSUP) {
        res = dev->driver->set_option(dev, opt, &value, 0);

        opt = (opt + 1) % OPTION_NUMBER;
    }

    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
}

static void test_netdev_dummy_set_option_overflow(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN + TEST_UINT8 + 1;
    int res = -ENOTSUP;
    netdev_opt_t opt = RANDOM_OPTION;

    while (res == -ENOTSUP) {
        res = dev->driver->set_option(dev, opt, &value, value_len);

        opt = (opt + 1) % OPTION_NUMBER;
    }

    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, res);

    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
}

static void test_netdev_dummy_set_option_not_supported(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, dev->driver->set_option(dev,
                          (netdev_opt_t)0xffffffff, &value, sizeof(unsigned int)));
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
}

static void test_netdev_dummy_set_get_channel(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(0, dev->driver->set_option(dev, NETDEV_OPT_CHANNEL,
                          &value, sizeof(unsigned int)));
    value = 0;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_CHANNEL,
                          &value, &value_len));
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int), value_len);
}

static void test_netdev_dummy_set_get_address(void)
{
    char value[UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN];
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
#if UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN < 12
    strncpy(value, TEST_STRING12, UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN);
#else
    strncpy(value, TEST_STRING12, 12);
#endif
    TEST_ASSERT_EQUAL_INT(0, dev->driver->set_option(dev, NETDEV_OPT_ADDRESS,
                          value, UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN));
#if UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN < 12
    memset(value, 0, UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN);
#else
    memset(value, 0, 12);
#endif
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_ADDRESS,
                          value, &value_len));
#if UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN < 12
    TEST_ASSERT(strncmp(value, TEST_STRING12,
                        UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) == 0);
#else
    TEST_ASSERT(strncmp(value, TEST_STRING12, 12) == 0);
#endif
}

static void test_netdev_dummy_set_get_nid(void)
{
    unsigned int value = (unsigned int)TEST_UINT32;
    size_t value_len = sizeof(unsigned int);
    TEST_ASSERT_EQUAL_INT(0, dev->driver->set_option(dev, NETDEV_OPT_NID,
                          &value, sizeof(unsigned int)));
    value = 0;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_NID,
                          &value, &value_len));
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, value);
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned int), value_len);
}

static void test_netdev_dummy_set_get_long_address(void)
{
    char value[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    size_t value_len = UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN;
#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN < 12
    strncpy(value, TEST_STRING12, UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN);
#else
    strncpy(value, TEST_STRING12, 12);
#endif
    TEST_ASSERT_EQUAL_INT(0, dev->driver->set_option(dev,
                          NETDEV_OPT_ADDRESS_LONG, value,
                          UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN));
#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN < 12
    memset(value, 0, UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN);
#else
    memset(value, 0, 12);
#endif
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev,
                          NETDEV_OPT_ADDRESS_LONG, value, &value_len));
#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN < 12
    TEST_ASSERT(strncmp(value, TEST_STRING12,
                        UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN) == 0);
#else
    TEST_ASSERT(strncmp(value, TEST_STRING12, 12) == 0);
#endif
    TEST_ASSERT_EQUAL_INT(UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN, value_len);
}

static void test_netdev_dummy_set_get_tx_power(void)
{
    int value = TEST_INT;
    size_t value_len = sizeof(int);
    TEST_ASSERT_EQUAL_INT(0, dev->driver->set_option(dev, NETDEV_OPT_TX_POWER,
                          &value, sizeof(int)));
    value = 0;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_option(dev, NETDEV_OPT_TX_POWER,
                          &value, &value_len));
    TEST_ASSERT_EQUAL_INT(TEST_INT, value);
    TEST_ASSERT_EQUAL_INT(sizeof(int), value_len);
}

/*********************************************
 * driver::get_state tests                   *
 *********************************************/

static void test_netdev_dummy_get_state_dev_null(void)
{
    netdev_state_t state;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->get_state(NULL, &state));
}

static void test_netdev_dummy_get_state_dev_wrong(void)
{
    netdev_state_t state;
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->get_state(&wrong_dev, &state));
}

static void test_netdev_dummy_get_state_null(void)
{
    TEST_ASSERT_EQUAL_INT(-EFAULT, dev->driver->get_state(dev, NULL));
}

static void test_netdev_dummy_get_state_value(void)
{
    netdev_state_t state;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_state(dev, &state));
    TEST_ASSERT_EQUAL_INT(NETDEV_STATE_POWER_OFF, state);
}

/*********************************************
 * driver::set_state tests                   *
 *********************************************/

static void test_netdev_dummy_set_state_dev_null(void)
{
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->set_state(NULL,
                          NETDEV_STATE_RX_MODE));
}

static void test_netdev_dummy_set_state_dev_wrong(void)
{
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(-ENODEV, dev->driver->set_state(&wrong_dev,
                          NETDEV_STATE_RX_MODE));
}

static void test_netdev_dummy_set_state_max(void)
{
    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          dev->driver->set_state(dev, (netdev_state_t)0xffffffff));
}

static void test_netdev_dummy_set_state_value(void)
{
    TEST_ASSERT_EQUAL_INT(0,
                          dev->driver->set_state(dev, NETDEV_STATE_RX_MODE));
}

static void test_netdev_dummy_set_get_state_value(void)
{
    netdev_state_t state;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_state(dev, &state));
    TEST_ASSERT(NETDEV_STATE_POWER_IDLE != state);
    state = NETDEV_STATE_POWER_IDLE;
    TEST_ASSERT_EQUAL_INT(0, dev->driver->set_state(dev, state));
    TEST_ASSERT_EQUAL_INT(0, dev->driver->get_state(dev, &state));
    TEST_ASSERT_EQUAL_INT(NETDEV_STATE_POWER_IDLE, state);
}

/*********************************************
 * driver::event tests                       *
 *********************************************/

static void test_netdev_dummy_event_dev_null(void)
{
    dev->driver->event(NULL, TEST_UINT32);
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_get_last_event(dev));
}

static void test_netdev_dummy_event_dev_wrong(void)
{
    netdev_t wrong_dev;
    dev->driver->event(&wrong_dev, TEST_UINT32);
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_get_last_event(dev));
}

static void test_netdev_dummy_event_value(void)
{
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_get_last_event(dev));
    dev->driver->event(dev, TEST_UINT32);
    TEST_ASSERT_EQUAL_INT((unsigned int)TEST_UINT32, unittest_netdev_dummy_get_last_event(dev));
}

/***********************************************
 * unittest_netdev_dummy_fire_rcv_event tests *
 ***********************************************/

static void test_netdev_dummy_fire_rcv_event_dev_null(void)
{
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
    TEST_ASSERT_EQUAL_INT(-ENODEV, unittest_netdev_dummy_fire_rcv_event(NULL,
                          src, src_len, dest, dest_len, data, data_len));
}

static void test_netdev_dummy_fire_rcv_event_dev_wrong(void)
{
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
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(-ENODEV, unittest_netdev_dummy_fire_rcv_event(
                              &wrong_dev, src, src_len, dest, dest_len, data,
                              data_len));
}

static void test_netdev_dummy_fire_rcv_event_data_too_long(void)
{
    char src[] = TEST_STRING64;
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING64;
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET + 1;
    TEST_ASSERT_EQUAL_INT(-EMSGSIZE, unittest_netdev_dummy_fire_rcv_event(dev,
                          src, src_len, dest, dest_len, data, data_len));
}

static void test_netdev_dummy_fire_rcv_event_data_null(void)
{
    char src[] = TEST_STRING64;
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
    TEST_ASSERT_EQUAL_INT(-EINVAL, unittest_netdev_dummy_fire_rcv_event(dev,
                          src, src_len, dest, dest_len, NULL, data_len));
}

static void test_netdev_dummy_fire_rcv_event_data_null_data_len_null(void)
{
    char src[] = TEST_STRING64;
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_fire_rcv_event(dev,
                          src, src_len, dest, dest_len, NULL, 0));
}

static void test_netdev_dummy_fire_rcv_event_src_too_long(void)
{
    char src[] = TEST_STRING64;
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN + 1;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING8;
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET < 8
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#else
    size_t data_len = 8;
#endif
    TEST_ASSERT_EQUAL_INT(-EAFNOSUPPORT, unittest_netdev_dummy_fire_rcv_event(
                              dev, src, src_len, dest, dest_len, data, data_len));
}

static void test_netdev_dummy_fire_rcv_event_src_null(void)
{
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING64;
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
    TEST_ASSERT_EQUAL_INT(-EINVAL, unittest_netdev_dummy_fire_rcv_event(dev,
                          NULL, src_len, dest, dest_len, data, data_len));
}

static void test_netdev_dummy_fire_rcv_event_dest_too_long(void)
{
    char src[] = TEST_STRING64;
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char dest[] = TEST_STRING64;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN + 1;
    char data[] = TEST_STRING8;
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET < 8
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#else
    size_t data_len = 8;
#endif
    TEST_ASSERT_EQUAL_INT(-EAFNOSUPPORT, unittest_netdev_dummy_fire_rcv_event(
                              dev, src, src_len, dest, dest_len, data,
                              data_len));
}

static void test_netdev_dummy_fire_rcv_event_dest_null(void)
{
    char src[] = TEST_STRING64;
    size_t src_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    size_t dest_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
    char data[] = TEST_STRING64;
    size_t data_len = UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
    TEST_ASSERT_EQUAL_INT(-EINVAL, unittest_netdev_dummy_fire_rcv_event(dev,
                          src, src_len, NULL, dest_len, data, data_len));
}

static void test_netdev_dummy_fire_rcv_event_no_cb(void)
{
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
    TEST_ASSERT_EQUAL_INT(0, unittest_netdev_dummy_fire_rcv_event(dev, src,
                          src_len, dest, dest_len, data, data_len));
}

static void test_netdev_dummy_fire_rcv_event_failing_cb(void)
{
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
    TEST_ASSERT_EQUAL_INT(0, dev->driver->add_receive_data_callback(dev,
                          _always_wrong));
    TEST_ASSERT_EQUAL_INT(-ECANCELED, unittest_netdev_dummy_fire_rcv_event(
                              dev, src, src_len, dest, dest_len, data, data_len));
}

/***********************************************
 * unittest_netdev_dummy_get_last_event tests *
 ***********************************************/

static void test_netdev_dummy_last_event_dev_null(void)
{
    TEST_ASSERT_EQUAL_INT(ENODEV, unittest_netdev_dummy_get_last_event(NULL));
}

static void test_netdev_dummy_last_event_dev_wrong(void)
{
    netdev_t wrong_dev;
    TEST_ASSERT_EQUAL_INT(ENODEV, unittest_netdev_dummy_get_last_event(&wrong_dev));
}

Test *tests_netdev_dummy_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* driver::init tests */
        new_TestFixture(test_netdev_dummy_init_dev_null),
        new_TestFixture(test_netdev_dummy_init_dev_wrong),
        /* driver::send_data tests */
        new_TestFixture(test_netdev_dummy_send_data_dev_null),
        new_TestFixture(test_netdev_dummy_send_data_dev_wrong),
        new_TestFixture(test_netdev_dummy_send_data_dest_too_long),
        new_TestFixture(test_netdev_dummy_send_data_data_too_long),
        new_TestFixture(test_netdev_dummy_send_data_no_ulhs),
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET > 4
        new_TestFixture(test_netdev_dummy_send_data_with_ulhs),
#endif
        /* driver::add_receive_data_callback tests */
        new_TestFixture(test_netdev_dummy_add_cb_dev_null),
        new_TestFixture(test_netdev_dummy_add_cb_dev_wrong),
        new_TestFixture(test_netdev_dummy_add_cb_buffer_full),
        new_TestFixture(test_netdev_dummy_add_cb_successful),
        /* driver::rem_receive_data_callback tests */
        new_TestFixture(test_netdev_dummy_rem_cb_dev_null),
        new_TestFixture(test_netdev_dummy_rem_cb_dev_wrong),
        new_TestFixture(test_netdev_dummy_rem_cb_unknown_cb),
        new_TestFixture(test_netdev_dummy_rem_cb_successful),
        new_TestFixture(test_netdev_dummy_rem_cb_multiple_added),
        /* driver::get_option tests */
        new_TestFixture(test_netdev_dummy_get_option_dev_null),
        new_TestFixture(test_netdev_dummy_get_option_dev_wrong),
        new_TestFixture(test_netdev_dummy_get_option_null_value),
        new_TestFixture(test_netdev_dummy_get_option_null_value_len),
        new_TestFixture(test_netdev_dummy_get_option_overflow),
        new_TestFixture(test_netdev_dummy_get_option_not_supported),
        new_TestFixture(test_netdev_dummy_get_channel),
        new_TestFixture(test_netdev_dummy_get_address),
        new_TestFixture(test_netdev_dummy_get_nid),
        new_TestFixture(test_netdev_dummy_get_long_address),
        new_TestFixture(test_netdev_dummy_get_tx_power),
        /* driver::set_option tests */
        new_TestFixture(test_netdev_dummy_set_option_dev_null),
        new_TestFixture(test_netdev_dummy_set_option_dev_wrong),
        new_TestFixture(test_netdev_dummy_set_option_null_value),
        new_TestFixture(test_netdev_dummy_set_option_inval),
        new_TestFixture(test_netdev_dummy_set_option_overflow),
        new_TestFixture(test_netdev_dummy_set_option_not_supported),
        new_TestFixture(test_netdev_dummy_set_get_channel),
        new_TestFixture(test_netdev_dummy_set_get_address),
        new_TestFixture(test_netdev_dummy_set_get_nid),
        new_TestFixture(test_netdev_dummy_set_get_long_address),
        new_TestFixture(test_netdev_dummy_set_get_tx_power),
        /* driver::get_state tests */
        new_TestFixture(test_netdev_dummy_get_state_dev_null),
        new_TestFixture(test_netdev_dummy_get_state_dev_wrong),
        new_TestFixture(test_netdev_dummy_get_state_null),
        new_TestFixture(test_netdev_dummy_get_state_value),
        /* driver::set_state tests */
        new_TestFixture(test_netdev_dummy_set_state_dev_null),
        new_TestFixture(test_netdev_dummy_set_state_dev_wrong),
        new_TestFixture(test_netdev_dummy_set_state_max),
        new_TestFixture(test_netdev_dummy_set_state_value),
        new_TestFixture(test_netdev_dummy_set_get_state_value),
        /* driver::event tests */
        new_TestFixture(test_netdev_dummy_event_dev_null),
        new_TestFixture(test_netdev_dummy_event_dev_wrong),
        new_TestFixture(test_netdev_dummy_event_value),
        /* unittest_netdev_dummy_fire_rcv_event tests */
        new_TestFixture(test_netdev_dummy_fire_rcv_event_dev_null),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_dev_wrong),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_data_too_long),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_data_null),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_data_null_data_len_null),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_src_too_long),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_src_null),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_dest_too_long),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_dest_null),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_no_cb),
        new_TestFixture(test_netdev_dummy_fire_rcv_event_failing_cb),
        /* unittest_netdev_dummy_get_last_event tests */
        new_TestFixture(test_netdev_dummy_last_event_dev_null),
        new_TestFixture(test_netdev_dummy_last_event_dev_wrong),
    };

    EMB_UNIT_TESTCALLER(netdev_dummy_tests, set_up, tear_down,
                        fixtures);

    return (Test *)&netdev_dummy_tests;
}

void tests_netdev_dummy(void)
{
    TESTS_RUN(tests_netdev_dummy_tests());
}
