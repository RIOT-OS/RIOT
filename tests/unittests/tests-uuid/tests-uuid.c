/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Unit tests for uuid module
 *
 * @author     Koen Zandberg <koen@bergzand.net>
 */

#include <stdio.h>
#include "embUnit.h"
#include "uuid.h"
#include "hashes/md5.h"
#include "hashes/sha1.h"

const char riotos_org[] = "riot-os.org";
const char test_str[] = "unittest";

/*
 * Generated with python:
 * import uuid;
 * riot = uuid.uuid3(uuid.NAMESPACE_DNS, "riot-os.org")
 * print(riot.hex)
 * print(uuid.uuid3(riot, "unittest").hex)
 */
const uint8_t v3_check1[] = {0x23, 0x99, 0x0b, 0xda, 0x1e, 0xe7, 0x34, 0x16,
                             0x90, 0xfe, 0x69, 0x30, 0x7d, 0x90, 0x64, 0x0e};
const uint8_t v3_check2[] = {0x20, 0xf5, 0x36, 0x91, 0x91, 0xae, 0x3c, 0xfa,
                             0x99, 0xb5, 0x8e, 0xf9, 0xfa, 0xc2, 0x76, 0x55};

/*
 * Generated with python:
 * import uuid;
 * riot = uuid.uuid5(uuid.NAMESPACE_DNS, "riot-os.org")
 * print(riot.hex)
 * print(uuid.uuid5(riot, "unittest").hex)
 */
const uint8_t v5_check1[] = {0x54, 0x7d, 0x0d, 0x74, 0x6d, 0x3a, 0x5a, 0x92,
                             0x96, 0x62, 0x48, 0x81, 0xaf, 0xd9, 0x40, 0x7b};
const uint8_t v5_check2[] = {0x7a, 0x1b, 0xf5, 0xdb, 0x5e, 0x77, 0x5e, 0x9b,
                             0x80, 0x6f, 0x0f, 0x55, 0x95, 0x58, 0xc9, 0xca};

/*
 * Length of the test strings without zero terminator.
 * Python doesn't feed the zero terminator in the uuid generator, so we test
 * without the zero terminator here too.
 */
#define RIOTOS_ORG_LEN  (sizeof(riotos_org) -1)
#define TEST_STR_LEN    (sizeof(test_str) -1)


void test_uuid_v3(void)
{
    uuid_t uuid, uuid_next;
    uuid_v3(&uuid, &uuid_namespace_dns,
            (uint8_t*)riotos_org, RIOTOS_ORG_LEN);
    uuid_v3(&uuid_next, &uuid,
            (uint8_t*)test_str, TEST_STR_LEN);

    TEST_ASSERT(uuid_equal(&uuid, (uuid_t*)v3_check1));
    TEST_ASSERT(uuid_equal(&uuid_next, (uuid_t*)v3_check2));
    TEST_ASSERT_EQUAL_INT(uuid_version(&uuid), UUID_V3);
    TEST_ASSERT_EQUAL_INT(uuid_version(&uuid_next), UUID_V3);
}

void test_uuid_v4(void)
{
    uuid_t uuid;
    uuid_v4(&uuid);

    TEST_ASSERT_EQUAL_INT(uuid_version(&uuid), 4);
}

void test_uuid_v5(void)
{
    uuid_t uuid, uuid_next;
    uuid_v5(&uuid, &uuid_namespace_dns,
            (uint8_t*)riotos_org, RIOTOS_ORG_LEN);
    uuid_v5(&uuid_next, &uuid,
            (uint8_t*)test_str, TEST_STR_LEN);

    TEST_ASSERT(uuid_equal(&uuid, (uuid_t*)v5_check1));
    TEST_ASSERT(uuid_equal(&uuid_next, (uuid_t*)v5_check2));
    TEST_ASSERT_EQUAL_INT(uuid_version(&uuid), UUID_V5);
    TEST_ASSERT_EQUAL_INT(uuid_version(&uuid_next), UUID_V5);
}

Test *tests_uuid_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_uuid_v3),
        new_TestFixture(test_uuid_v4),
        new_TestFixture(test_uuid_v5),
    };

    EMB_UNIT_TESTCALLER(uuid_tests, NULL, NULL, fixtures);
    return (Test *)&uuid_tests;
}

void tests_uuid(void)
{
    TESTS_RUN(tests_uuid_all());
}
