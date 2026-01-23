/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Tests for the yxml XML parser library package
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */
#include <string.h>
#include <stdio.h>

#include "embUnit.h"
#include "yxml.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static char _yxml_buf[1024];

static const char *_doc = "<foo>bar</foo>";

static void test_yxml(void)
{
    yxml_t yxml;
    char content[64];

    /* initialize yxml instance */
    yxml_init(&yxml, _yxml_buf, sizeof(_yxml_buf));

    /* parse XML document bytewise */
    for (const char *doc = _doc; *doc; doc++) {
        yxml_ret_t r = yxml_parse(&yxml, *doc);
        TEST_ASSERT(r >= 0);

        /* handle tokens */
        switch (r) {
            case YXML_OK:
                break;
            case YXML_ELEMSTART:
                DEBUG("YXML_ELEMSTART \"%s\"\n", yxml.elem);
                TEST_ASSERT_EQUAL_STRING("foo", yxml.elem);
                memset(content, 0, sizeof(content));
                break;
            case YXML_CONTENT:
                DEBUG_PUTS("YXML_CONTENT");
                strcat(content, yxml.data);
                break;
            case YXML_ELEMEND:
                DEBUG("YXML_ELEMEND content=\"%s\"\n", content);
                TEST_ASSERT_EQUAL_STRING("bar", content);
                break;
            default:
                DEBUG("unhandled token %i\n", r);
        }
    }

    /* let yxml know the document has ended */
    TEST_ASSERT_EQUAL_INT(0, yxml_eof(&yxml));
}

TestRef tests_yxml(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_yxml),
    };

    EMB_UNIT_TESTCALLER(YxmlTest, 0, 0, fixtures);
    return (TestRef) & YxmlTest;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_yxml());
    TESTS_END();

    return 0;
}
