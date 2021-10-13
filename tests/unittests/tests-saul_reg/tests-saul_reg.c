/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Implementations of unit tests for the SAUL registry
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "saul_reg.h"
#include "tests-saul_reg.h"

static const saul_driver_t s0_dri = { NULL, NULL, SAUL_ACT_SERVO };
static const saul_driver_t s1_dri = { NULL, NULL, SAUL_SENSE_TEMP };
static const saul_driver_t s2_dri = { NULL, NULL, SAUL_SENSE_LIGHT };
static const saul_driver_t s3a_dri = { NULL, NULL, SAUL_ACT_LED_RGB };
static const saul_driver_t s3b_dri = { NULL, NULL, SAUL_ACT_SWITCH };

static saul_reg_t s0 = { NULL, NULL, "S0", &s0_dri };
static saul_reg_t s1 = { NULL, NULL, "S1", &s1_dri };
static saul_reg_t s2 = { NULL, NULL, "S2", &s2_dri };
/* both registrations use the same name intentionally */
static saul_reg_t s3a = { NULL, NULL, "S3", &s3a_dri };
static saul_reg_t s3b = { NULL, NULL, "S3", &s3b_dri };

static int count(void)
{
    int i = 0;
    saul_reg_t *cur = saul_reg;

    while (cur) {
        ++i;
        cur = cur->next;
    }

    return i;
}

static saul_reg_t *last(void)
{
    saul_reg_t *cur = saul_reg;

    if (cur == NULL) {
        return NULL;
    }

    while (cur->next != NULL) {
        cur = cur->next;
    }
    return cur;
}

static void test_reg_initial_size(void)
{
    TEST_ASSERT_NULL(saul_reg);
    TEST_ASSERT_EQUAL_INT(count(), 0);
}

static void test_reg_add(void)
{
    int res;

    res = saul_reg_add(&s0);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(count(), 1);
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S0", last()->name);

    res = saul_reg_add(&s1);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(count(), 2);
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S1", last()->name);

    res = saul_reg_add(&s2);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(count(), 3);
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S2", last()->name);

    res = saul_reg_add(&s3a);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(count(), 4);
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S3", last()->name);

    res = saul_reg_add(&s3b);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(count(), 5);
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S3", last()->name);

    res = saul_reg_add(NULL);
    TEST_ASSERT_EQUAL_INT(-ENODEV, res);
    TEST_ASSERT_EQUAL_INT(count(), 5);
}

static void test_reg_find_nth(void)
{
    saul_reg_t *dev = saul_reg_find_nth(0);
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_STRING("S0", dev->name);

    dev = saul_reg_find_nth(2);
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_STRING("S2", dev->name);

    dev = saul_reg_find_nth(17);
    TEST_ASSERT_NULL(dev);
}

static void test_reg_find_type(void)
{
    saul_reg_t *dev = saul_reg_find_type(SAUL_SENSE_TEMP);
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_STRING("S1", dev->name);

    dev = saul_reg_find_type(SAUL_ACT_LED_RGB);
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_STRING("S3", dev->name);

    dev = saul_reg_find_type(SAUL_ACT_MOTOR);
    TEST_ASSERT_NULL(dev);
}

static void test_reg_find_name(void)
{
    saul_reg_t *dev = saul_reg_find_name("S1");
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_INT(SAUL_SENSE_TEMP, dev->driver->type);

    dev = saul_reg_find_name("S0");
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_INT(SAUL_ACT_SERVO, dev->driver->type);

    dev = saul_reg_find_name("TUUT");
    TEST_ASSERT_NULL(dev);
}

static void test_reg_find_type_and_name(void)
{
    saul_reg_t *dev = saul_reg_find_type_and_name(SAUL_ACT_LED_RGB, "S3");
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_INT(SAUL_ACT_LED_RGB, dev->driver->type);

    dev = saul_reg_find_type_and_name(SAUL_ACT_SWITCH, "S3");
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_INT(SAUL_ACT_SWITCH, dev->driver->type);

    dev = saul_reg_find_type_and_name(SAUL_SENSE_TEMP, "S3");
    TEST_ASSERT_NULL(dev);
}

static void test_reg_rm(void)
{
    int res;

    TEST_ASSERT_EQUAL_INT(5, count());
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S3", last()->name);

    res = saul_reg_rm(&s3b);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(4, count());
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S3", last()->name);

    res = saul_reg_rm(&s3a);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(3, count());
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S2", last()->name);

    res = saul_reg_rm(&s1);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(2, count());
    TEST_ASSERT_EQUAL_STRING("S0", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S2", last()->name);

    res = saul_reg_rm(&s1);
    TEST_ASSERT_EQUAL_INT(-ENODEV, res);

    res = saul_reg_rm(NULL);
    TEST_ASSERT_EQUAL_INT(-ENODEV, res);

    TEST_ASSERT_EQUAL_INT(2, count());

    res = saul_reg_rm(&s0);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(1, count());
    TEST_ASSERT_EQUAL_STRING("S2", saul_reg->name);
    TEST_ASSERT_EQUAL_STRING("S2", last()->name);

    res = saul_reg_rm(&s2);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(0, count());
    TEST_ASSERT_NULL(saul_reg);
}

Test *tests_saul_reg_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_reg_initial_size),
        new_TestFixture(test_reg_add),
        new_TestFixture(test_reg_find_nth),
        new_TestFixture(test_reg_find_type),
        new_TestFixture(test_reg_find_name),
        new_TestFixture(test_reg_find_type_and_name),
        new_TestFixture(test_reg_rm)
    };

    EMB_UNIT_TESTCALLER(pkt_tests, NULL, NULL, fixtures);

    return (Test *)&pkt_tests;
}

void tests_saul_reg(void)
{
    TESTS_RUN(tests_saul_reg_tests());
}
